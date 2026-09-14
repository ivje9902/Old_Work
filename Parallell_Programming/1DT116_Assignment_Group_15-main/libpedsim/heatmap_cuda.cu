// heatmap_cuda.cu
// CUDA heatmap pipeline for pedsim Assignment 4
// Assumes:
//   SIZE = 1024
//   CELLSIZE = 5
//   SCALED_SIZE = 5120

#include <cuda_runtime.h>
#include <cstdio>
#include <cstdint>

#ifndef SIZE
#define SIZE 1024
#endif

#ifndef CELLSIZE
#define CELLSIZE 5
#endif

#ifndef SCALED_SIZE
#define SCALED_SIZE (SIZE * CELLSIZE) // 5120
#endif

#define TIME_KERNEL(name, launch) \
{ \
    cudaEventRecord(start, stream0); \
    launch; \
    cudaEventRecord(stop, stream0); \
    cudaEventSynchronize(stop); \
    float ms; \
    cudaEventElapsedTime(&ms, start, stop); \
    printf("%s: %f ms\n", name, ms); \
}

#include <cuda_runtime.h>

// ---------------------------
// Optional CUDA error checking
// ---------------------------
static inline void cudaCheck(cudaError_t e, const char* file, int line) {
    if (e != cudaSuccess) {
        std::fprintf(stderr, "CUDA error %s:%d: %s\n", file, line, cudaGetErrorString(e));
    }
}
#define CUDA_CHECK(x) cudaCheck((x), __FILE__, __LINE__)

// 5x5 blur weights in constant memory (broadcast-friendly).
__constant__ int kBlurW5[25] = {
    1, 4, 7, 4, 1,
    4, 16, 26, 16, 4,
    7, 26, 41, 26, 7,
    4, 16, 26, 16, 4,
    1, 4, 7, 4, 1
};

__global__ void fadeKernel(int* heat)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    const int n = SIZE * SIZE;
    if (idx >= n) return;

    // Exact round(v * 0.8) for non-negative integers: (4*v + 2) / 5
    int v = heat[idx];
    heat[idx] = (4 * v + 2) / 5;
}

__global__ void addAgentsKernel(int* heat,
                               const int* desX,
                               const int* desY,
                               int nAgents)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= nAgents) return;

    int x = desX[i];
    int y = desY[i];

    // Match your bounds check
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return;

    atomicAdd(&heat[y * SIZE + x], 40);
}

__global__ void clampAfterAddKernel(int* heat)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    const int n = SIZE * SIZE;
    if (idx >= n) return;

    int v = heat[idx];
    if (v > 255) v = 255;
    heat[idx] = v;
}

__global__ void scaleKernel(const int* heat, int* scaled)
{
    int ox = blockIdx.x * blockDim.x + threadIdx.x;
    int oy = blockIdx.y * blockDim.y + threadIdx.y;

    if (ox >= SCALED_SIZE || oy >= SCALED_SIZE) return;

    int ix = ox / CELLSIZE;
    int iy = oy / CELLSIZE;

    // ix,iy are guaranteed in range because SCALED_SIZE == SIZE*CELLSIZE
    scaled[oy * SCALED_SIZE + ox] = heat[iy * SIZE + ix];
}

__global__ void blurPackKernel(const int* __restrict__ scaled,
                              int* __restrict__ blurred)
{
    constexpr int BLOCK_X = 16;
    constexpr int BLOCK_Y = 16;
    constexpr int R = 2;
    constexpr int TILE_W = BLOCK_X + 2 * R; // 20
    constexpr int TILE_H = BLOCK_Y + 2 * R; // 20

    // +1 padding on X to reduce bank conflict patterns on stencil reads.
    __shared__ int tile[TILE_H][TILE_W + 1];

    int tx = threadIdx.x; // 0..15
    int ty = threadIdx.y; // 0..15

    const int blockBaseX = blockIdx.x * BLOCK_X;
    const int blockBaseY = blockIdx.y * BLOCK_Y;

    // Cooperative contiguous tile load (center + halo).
    const int linearTid = ty * BLOCK_X + tx;
    const int threadsPerBlock = BLOCK_X * BLOCK_Y;
    const int tileElems = TILE_W * TILE_H;

    for (int e = linearTid; e < tileElems; e += threadsPerBlock) {
        int ly = e / TILE_W;
        int lx = e - ly * TILE_W;

        int gx = blockBaseX + lx - R;
        int gy = blockBaseY + ly - R;

        int v = 0;
        if ((unsigned)gx < (unsigned)SCALED_SIZE && (unsigned)gy < (unsigned)SCALED_SIZE) {
            v = scaled[gy * SCALED_SIZE + gx];
        }
        tile[ly][lx] = v;
    }

    __syncthreads();

    const int x = blockBaseX + tx;
    const int y = blockBaseY + ty;

    // Match CPU bounds: i,j in [2 .. SCALED_SIZE-3]
    if (x < 2 || x >= SCALED_SIZE - 2 || y < 2 || y >= SCALED_SIZE - 2)
        return;

    int sum = 0;
    const int tileX = tx + R;
    const int tileY = ty + R;
    #pragma unroll
    for (int dy = -2; dy <= 2; ++dy) {
        #pragma unroll
        for (int dx = -2; dx <= 2; ++dx) {
            sum += kBlurW5[(dy + 2) * 5 + (dx + 2)] * tile[tileY + dy][tileX + dx];
        }
    }

    int value = sum / 273; // WEIGHTSUM
    if (value < 0) value = 0;
    if (value > 255) value = 255;

    // ARGB: alpha=value, red=255
    blurred[y * SCALED_SIZE + x] = 0x00FF0000 | (value << 24);
}

// Device buffers kept by this module (simple approach)
static int* d_heat = nullptr;
static int* d_scaled = nullptr;
static int* d_blurred = nullptr;
static int* d_desX = nullptr;
static int* d_desY = nullptr;
static int  desCap = 0;

static cudaStream_t stream0 = nullptr;

extern "C" void heatmapCudaInit(int maxAgents)
{
    if (stream0 == nullptr) {
        CUDA_CHECK(cudaStreamCreate(&stream0));
    }

    // Allocate heatmap buffers
    if (!d_heat)    CUDA_CHECK(cudaMalloc(&d_heat,    SIZE * SIZE * sizeof(int)));
    if (!d_scaled)  CUDA_CHECK(cudaMalloc(&d_scaled,  SCALED_SIZE * SCALED_SIZE * sizeof(int)));
    if (!d_blurred) CUDA_CHECK(cudaMalloc(&d_blurred, SCALED_SIZE * SCALED_SIZE * sizeof(int)));

    CUDA_CHECK(cudaMemsetAsync(d_heat, 0, SIZE * SIZE * sizeof(int), stream0));
    // scaled/blurred will be overwritten every tick, no need to memset

    // Desired position buffers
    if (maxAgents > desCap) {
        if (d_desX) CUDA_CHECK(cudaFree(d_desX));
        if (d_desY) CUDA_CHECK(cudaFree(d_desY));
        CUDA_CHECK(cudaMalloc(&d_desX, maxAgents * sizeof(int)));
        CUDA_CHECK(cudaMalloc(&d_desY, maxAgents * sizeof(int)));
        desCap = maxAgents;
    }
}

extern "C" void heatmapCudaFree()
{
    if (d_heat)    CUDA_CHECK(cudaFree(d_heat));
    if (d_scaled)  CUDA_CHECK(cudaFree(d_scaled));
    if (d_blurred) CUDA_CHECK(cudaFree(d_blurred));
    if (d_desX)    CUDA_CHECK(cudaFree(d_desX));
    if (d_desY)    CUDA_CHECK(cudaFree(d_desY));
    d_heat = d_scaled = d_blurred = d_desX = d_desY = nullptr;
    desCap = 0;

    if (stream0) {
        CUDA_CHECK(cudaStreamDestroy(stream0));
        stream0 = nullptr;
    }
}

// Run the full pipeline for one tick.
// Inputs: h_desX/h_desY (host arrays), nAgents
// Output: copies final blurred ARGB image into h_blurredOut (host buffer of size SCALED_SIZE^2 ints)
extern "C" void heatmapCudaTick(const int* h_desX,
                                const int* h_desY,
                                int nAgents,
                                int* h_blurredOut)
{
    if (!d_heat || !d_scaled || !d_blurred || !d_desX || !d_desY || !stream0) {
        std::fprintf(stderr, "heatmapCudaTick called before heatmapCudaInit\n");
        return;
    }
    if (nAgents > desCap) {
        std::fprintf(stderr, "heatmapCudaTick: nAgents exceeds init capacity\n");
        return;
    }

    // Copy desired arrays to device (async)
    CUDA_CHECK(cudaMemcpyAsync(d_desX, h_desX, nAgents * sizeof(int), cudaMemcpyHostToDevice, stream0));
    CUDA_CHECK(cudaMemcpyAsync(d_desY, h_desY, nAgents * sizeof(int), cudaMemcpyHostToDevice, stream0));
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Launch fade
    {
        int threads = 256;
        int blocks = (SIZE * SIZE + threads - 1) / threads;
        TIME_KERNEL("fadeKernel",
            (fadeKernel<<<blocks, threads, 0, stream0>>>(d_heat))
        );
    }

    // Launch add agents (atomic)
    {
        int threads = 256;
        int blocks = (nAgents + threads - 1) / threads;
        TIME_KERNEL("addAgentsKernel",
            (addAgentsKernel<<<blocks, threads, 0, stream0>>>(d_heat, d_desX, d_desY, nAgents))
        );
    }

    // Clamp after adds
    {
        int threads = 256;
        int blocks = (SIZE * SIZE + threads - 1) / threads;
        TIME_KERNEL("clampAfterAddKernel",
            (clampAfterAddKernel<<<blocks, threads, 0, stream0>>>(d_heat))
        );
    }

    // Scale
    {
        dim3 block(16, 16);
        dim3 grid((SCALED_SIZE + block.x - 1) / block.x,
                  (SCALED_SIZE + block.y - 1) / block.y);
        TIME_KERNEL("scaleKernel",
            (scaleKernel<<<grid, block, 0, stream0>>>(d_heat, d_scaled))
        );
    }

    // Blur + pack
    {
        dim3 block(16, 16);
        dim3 grid((SCALED_SIZE + block.x - 1) / block.x,
                  (SCALED_SIZE + block.y - 1) / block.y);
        TIME_KERNEL("blurPackKernel",
            (blurPackKernel<<<grid, block, 0, stream0>>>(d_scaled, d_blurred))
        );
    }

    // Copy result back (ARGB int image)
    CUDA_CHECK(cudaMemcpyAsync(h_blurredOut,
                               d_blurred,
                               SCALED_SIZE * SCALED_SIZE * sizeof(int),
                               cudaMemcpyDeviceToHost,
                               stream0));
}

// Expose the stream if you want to sync later elsewhere.
extern "C" cudaStream_t heatmapCudaStream()
{
    return stream0;
}

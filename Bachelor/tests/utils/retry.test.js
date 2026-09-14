const retry = require('../../utils/retry');

describe('function retryApiCall', () => {
    const mockFn = jest.fn();
    const action = 'testAction';
    const retries = 3;
    const delay = 1000;

    beforeEach(() => {
        jest.clearAllMocks();
    });

    afterAll(() => {
        mockFn.mockRestore();
    })

    it('should call the function and return its result', async () => {
        mockFn.mockResolvedValue('result');
        const result = await retry.retryApiCall(mockFn, action, retries, delay);
        expect(result).toBe('result');
        expect(mockFn).toHaveBeenCalledTimes(1);
    });

    it('should retry the function on failure', async () => {
        mockFn.mockRejectedValueOnce(new Error('error'));
        mockFn.mockResolvedValueOnce('result');
        const result = await retry.retryApiCall(mockFn, action, retries, delay);
        expect(result).toBe('result');
        expect(mockFn).toHaveBeenCalledTimes(2);
    });

    it('should throw an error after max retries', async () => {
        mockFn.mockRejectedValue(new Error('error'));
        await expect(retry.retryApiCall(mockFn, action, retries, delay)).rejects.toThrow('error');
        expect(mockFn).toHaveBeenCalledTimes(retries);
    });

    it('should double the delay after each retry', async () => {
        mockFn.mockRejectedValueOnce(new Error('error'));
        mockFn.mockResolvedValueOnce('result');
        const startTime = Date.now();
        await retry.retryApiCall(mockFn, action, retries, delay);
        const endTime = Date.now();
        expect(endTime - startTime).toBeGreaterThanOrEqual(delay);
        expect(endTime - startTime).toBeLessThanOrEqual(delay * 2);
    });
});
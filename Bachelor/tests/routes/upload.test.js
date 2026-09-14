const request = require('supertest');

const mockUploadedFile = { data: 'Mocked file', uri: 'Mock-URI' };
const mockResponse = { text: JSON.stringify([{title: "Some title", description: "Some description"}]) };
const mockEmbeddingResponse = {
    embeddings: [
        {
            values: Array(768).fill(0.1) // Mock embedding with 768 dimensions
        }
    ]
};

jest.mock("@google/genai", () => {
    return {
        GoogleGenAI: jest.fn().mockImplementation(() => ({ 
            models: {
                embedContent: jest.fn().mockResolvedValue(mockEmbeddingResponse),
                generateContent: jest.fn().mockResolvedValue(mockResponse)
            },
            files: {
                upload: jest.fn().mockResolvedValue(mockUploadedFile)
            }
        })),
        createPartFromUri: jest.requireActual('@google/genai').createPartFromUri
    };
});

const mockItemCreateResponse = "Mock item.create response";
let mockFetchAll = { resources: ["Mock fetchAll 1", "Mock fetchAll 2", "Mock fetchAll 3", "Mock fetchAll 4", "Mock fetchAll 5"]};

const mockFetchAllFn = jest.fn().mockResolvedValue(mockFetchAll);

jest.mock("@azure/cosmos", () => {
    const mockQuery = {
        fetchAll: mockFetchAllFn
    };
    const mockItems = {
        create: jest.fn().mockResolvedValue(mockItemCreateResponse),
        query: jest.fn().mockImplementation(() => mockQuery)
    };
    const mockContainer = {
        items: mockItems
    };
    const mockDatabase = {
        container: jest.fn().mockReturnValue(mockContainer)
    };
    return {
        CosmosClient: jest.fn().mockImplementation(() => ({
            database: jest.fn().mockReturnValue(mockDatabase)
        }))
    };
});

const app = require('../../app');
const path = require('path');
const fs = require('fs');
const {createTestXlsxFile, createTestPdfFile} = require('../createTestFiles')

describe('POST /upload-file', () => {;
    test('should throw error for no file', async () => {
        let emptyFile;
        const response = await request(app)
        .post('/upload/upload-file')
        .attach('file', emptyFile)
        .expect('Content-Type', /.*json/)
        .expect(400);
    })
    test('should throw error for invalid file type', async () => {
        const mockFileNameTxt = "loremTest.txt"
        const mockPathTxt = path.join(__dirname, "../material/", mockFileNameTxt)
        const response = await request(app)
        .post('/upload/upload-file')
        .attach('file', mockPathTxt) // Attach the mock file
        .expect('Content-Type', /.*json/)
        .expect(400);
    })
    test('should call mock function for upload pdf', async () => {
        const mockPathPdf = await createTestPdfFile('moxkpdf.pdf');
        const response = await request(app)
            .post('/upload/upload-file')
            .attach('file', mockPathPdf)
            .expect('Content-Type', /.*json/)
            .expect(200);
        if(fs.existsSync(mockPathPdf)){
            fs.unlinkSync(mockPathPdf);
        }
    })
    test('should call mock function for upload xlsx', async () => {
        const mockPathXlsx = await createTestXlsxFile('mockxslsx.xlsx');
        const response = await request(app)
            .post('/upload/upload-file')
            .attach('file', mockPathXlsx)
            .expect('Content-Type', /.*json/)
            .expect(200);
        if(fs.existsSync(mockPathXlsx)){
            fs.unlinkSync(mockPathXlsx)
        }
    })
})
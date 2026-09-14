process.env.TEST_FOLDER_PATH = 'tests/material/'
const { Document } = require('@langchain/core/documents');
const fs = require('fs');
const assert = require('assert');
const path = require('path');
const { createTestPdfFile } = require('./createTestFiles')

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
                embedContent: jest.fn().mockResolvedValue(mockEmbeddingResponse)
            }
        }))
    };
});

let mockFetchAll = { resources: [] };
const mockFetchAllFn = jest.fn().mockResolvedValue(mockFetchAll);
const mockCreateFn = jest.fn().mockResolvedValue("Mock item.create response");
jest.mock("@azure/cosmos", () => {
    const mockQuery = {
        fetchAll: mockFetchAllFn
    };
    const mockItems = {
        create: mockCreateFn,
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

const insertDatabase = require('../insertDatabase');
const { CosmosClient } = require('@azure/cosmos');
const embeddingHandle = require('../embeddingHandle')

const pathPdf = path.join(__dirname, './material/beteendeforandringar_energi.pdf');
const pathShortPdf = path.join(__dirname, './material/pdf_for_testing.pdf');
const pathTxt = path.join(__dirname, './material/loremTest.txt');

beforeEach(() => {
    jest.clearAllMocks();
})

describe('insertDatabase.loadPDF', () => {
    it('should return a Document object', async () => {
        let path = pathPdf;
        assert(fs.existsSync(path), 'File does not exist');
        let result = await insertDatabase.loadPDF(path);
        expect(result).toBeInstanceOf(Document);
        expect(result).toHaveProperty('pageContent'); 
        expect(result).toHaveProperty('metadata'); 
        expect(result).toHaveProperty('id'); 
    });
    it('should throw an error if file does not exist', async () => {
        let path = 'nonexistent.pdf';
        assert(!fs.existsSync(path), 'File exists');
        await expect(insertDatabase.loadPDF(path)).rejects.toThrow('File not found');
    });
    it('should throw an error if file is not a .pdf file', async () => {
        let path = pathTxt;
        assert((fs.existsSync(path)), 'File does not exist');
        await expect(insertDatabase.loadPDF(path)).rejects.toThrow('File is not a .pdf file');
    });
    it('should throw an error if path is empty', async () => {
        await expect(insertDatabase.loadPDF('')).rejects.toThrow('Path is required');
    });
    it('should throw an error if path is not a string', async () => {
        await expect(insertDatabase.loadPDF(123)).rejects.toThrow('Path must be a string');
    });
    it('should throw an error if path is not a file', async () => {
        await expect(insertDatabase.loadPDF('./material')).rejects.toThrow('Path is not a file: ./material');
    })
});

describe('insertDatabase.chunkDocument', () => {
    let doc = new Document({ pageContent: fs.readFileSync(pathTxt) });
    it('should return an array of Document objects', async () => {
        let result = await insertDatabase.chunkDocument(doc);
        expect(Array.isArray(result)).toBe(true);
        expect(result).toHaveLength(5);
        for (let i = 0; i < result.length; i++) {
            expect(result[i]).toBeInstanceOf(Document);
        }
        
    });
    it('should throw an error if document is not provided', async () => {
        await expect(insertDatabase.chunkDocument()).rejects.toThrow('Document is required');
    });
    it('should throw an error if document is not an object', async () => {
        await expect(insertDatabase.chunkDocument('test')).rejects.toThrow('Document must be of type Document');
    });
    it('should throw an error if document is empty', async () => {
        let emptyDoc = new Document({ pageContent: '' });
        await expect(insertDatabase.chunkDocument(emptyDoc)).rejects.toThrow('Document must have text');
    });
    it('should throw an error if document is not a Document object', async () => {
        let invalidDoc = { pageContent: 'test', metadata: { source: './material/AI-generated/test' } };
        await expect(insertDatabase.chunkDocument(invalidDoc)).rejects.toThrow('Document must be of type Document');
    });
});

describe('insertDatabase.insertDatabase', () => {
    it('should insert a document into the database', async () => {
        let doc = new Document({ pageContent: 'test', metadata: { source: './material/AI-generated/test' } });
        let result = await insertDatabase.insertDatabase(doc);
        expect(result).toBeDefined();
        expect(result).toHaveProperty('id');
        expect(result).toHaveProperty('dummyData');
        expect(result).toHaveProperty('metadata');
        expect(result).toHaveProperty('text');
        expect(result).toHaveProperty('embedding');
    });
    it('should throw an error if document is not provided', async () => {
        await expect(insertDatabase.insertDatabase()).rejects.toThrow('Document is required');
    });
    it('should throw an error if document is not an object', async () => {
        await expect(insertDatabase.insertDatabase('test')).rejects.toThrow('Document must be of type Document');
    });
    it('should throw an error if document is empty', async () => {
        let emptyDoc = new Document({ pageContent: '' });
        await expect(insertDatabase.insertDatabase(emptyDoc)).rejects.toThrow('Document must have text');
    });
    it('should throw an error if document is not a Document object', async () => {
        let invalidDoc = { pageContent: 'test', metadata: { source: './material/AI-generated/test' } };
        await expect(insertDatabase.insertDatabase(invalidDoc)).rejects.toThrow('Document must be of type Document');
    });
});

describe('function main', () => {
    it('should "upload" many files by calling mockCreateFn', async () => {
        process.argv[2] = null;
        const callsBefore = mockCreateFn.mock.calls.length;
        expect(mockCreateFn).not.toHaveBeenCalled();
        expect(callsBefore).toBe(0);
        const result = await insertDatabase.main();
        expect(mockCreateFn).toHaveBeenCalled();
        const callsAfter = mockCreateFn.mock.calls.length;
        expect(callsAfter).toBeGreaterThan(callsBefore);
    })
    it('should return undefined', async () => {  
        process.argv[2] = './material/non-existent.pdf';
        const result = await insertDatabase.main();
        expect(result).toBeUndefined();
    })
    it('should "upload" file by calling mockCreateFn', async () => {
        process.argv[2] = pathShortPdf
        const result = await insertDatabase.main();
        expect(mockFetchAllFn).toHaveBeenCalled();
        expect(mockCreateFn).toHaveBeenCalled();
    })
    it('should trigger checkIfDocumentInDatabase', async () => {
        mockFetchAllFn.mockResolvedValueOnce({ resources: ['this', 'should', 'make', 'checkIfDocumentInDatabase', 'return', 'true'] });
        process.argv[2] = pathPdf
        const result = await insertDatabase.main();
        expect(mockCreateFn).not.toHaveBeenCalled();
        expect(mockFetchAllFn).toHaveBeenCalled();
        
    })
})

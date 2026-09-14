process.env.TEST_COSMOS_DATABASE = 'rag-database-test';
process.env.TEST_COSMOS_CONTAINER = 'chunks-test';
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

const mockItemCreateResponse = "Mock item.create response";
let mockFetchAll = { resources: ["Mock fetchAll 1", "Mock fetchAll 2", "Mock fetchAll 3", "Mock fetchAll 4", "Mock fetchAll 5"]};

const mockFetchAllFn = jest.fn().mockResolvedValue(mockFetchAll);

jest.mock("@azure/cosmos", () => {
    const mockQuery = {
        fetchAll: mockFetchAllFn
    };
    const mockItems = {
        create: jest.fn().mockResolvedValue("Mock item.create response"),
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


const { GoogleGenAI } = require("@google/genai");
const { CosmosClient } = require("@azure/cosmos");
const assert = require('assert');
const fs = require('fs');
const path = require('path');
const embeddingHandle = require('../embeddingHandle');
const insertDatabase = require('../insertDatabase');

beforeEach(() => {
    jest.clearAllMocks();
});

afterAll(async () => {
    GoogleGenAI.mockRestore();
    CosmosClient.mockRestore();
});


describe ('embeddingHandle.generateEmbeddings', () => {
    it('should return an array of numbers', async () => {
        const text = 'test text';
        const result = await embeddingHandle.generateEmbeddings(text);
        expect(Array.isArray(result)).toBe(true);
        expect(result.length).toBe(768); // Assuming the embedding size is 768
        expect(result[0]).toBe(0.1)
    });
    
    it('should throw an error for empty input', async () => {
        await expect(embeddingHandle.generateEmbeddings('')).rejects.toThrow();
    });
    it('should throw an error if the input is not a string', async () => {
        await expect(embeddingHandle.generateEmbeddings(123)).rejects.toThrow('Invalid input: text must be a non-empty string');
    })
}
);
describe ('embeddingHandle.vectorSearch', () => {
    it('should return an array of results', async () => {
        const queryEmbedding = [0.1, 0.2, 0.3]; // Example embedding
        const topK = 5;
        const result = await embeddingHandle.vectorSearch(queryEmbedding, topK);
        expect(Array.isArray(result)).toBe(true);
        expect(result.length).toBe(topK);
        expect(result).toBe(mockFetchAll.resources);
        expect(result[0]).toBe("Mock fetchAll 1");
    });
    
    it('should throw an error for invalid input', async () => {
        await expect(embeddingHandle.vectorSearch([], 5)).rejects.toThrow();
    });
}
);
describe ('embeddingHandle.saveEmbedding', () => {
    it('should save an embedding to the database', async () => {
        const text = 'test text';
        const embedding = [0.1, 0.2, 0.3]; // Example embedding
        const metadata = { source: 'test' };
        const id = 'test-id';
        const result = await embeddingHandle.saveEmbedding(text, embedding, metadata, true, id);
        expect(result).toBeDefined();
        expect(result).toStrictEqual({
            id: id,
            dummyData: true,
            metadata: metadata,
            text: text,
            embedding: embedding
        });
    });
    
    it('should throw an error for invalid input', async () => {
        await expect(embeddingHandle.saveEmbedding('', [], {})).rejects.toThrow();
    });
    it('should throw an error for invalid embedding', async () => {
        const text = 'test text';
        await expect(embeddingHandle.saveEmbedding(text, '', {})).rejects.toThrow();
    });
    it('should throw an error for invalid dummyData', async () => {
        const text = 'test text';
        const embedding = [0.1, 0.2, 0.3];
        const metadata = { source: 'test' };
        await expect(embeddingHandle.saveEmbedding(text, embedding, metadata, true)).rejects.toThrow();
    });
    it ('should throw an error for invalid id', async () => {
        const text = 'test text';
        const embedding = [0.1, 0.2, 0.3];
        const metadata = { source: 'test' };
        await expect(embeddingHandle.saveEmbedding(text, embedding, metadata, true, '')).rejects.toThrow();
    });
});

describe('insertDatabase.checkIfDocumentInDatabase', () => {
    it('should return true if document exists in database', async () => {
        const pathPDF = 'path-to-some-pdf.pdf'
        let result = await embeddingHandle.checkIfDocumentInDatabase(pathPDF);
        expect(result).toBe(true);
    });
    it('should return false if document does not exist in database', async () => {
        mockFetchAllFn.mockResolvedValueOnce({ resources: [] });
        let path = 'nonexistent.pdf';
        let result = await embeddingHandle.checkIfDocumentInDatabase(path);
        expect(result).toBe(false);
    });
    
    it('should throw an error if path is empty', async () => {
        await expect(embeddingHandle.checkIfDocumentInDatabase('')).rejects.toThrow('Path is required');
    }); 
    it('should throw an error if path is not a string', async () => {
        await expect(embeddingHandle.checkIfDocumentInDatabase(123)).rejects.toThrow('Path must be a string');
    });
});
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
let mockFetchAll = { 
    resources: [
        { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
        { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' },
        { id: 3, similarity: 0.7, metadata: { source: 'source3' }, text: 'text3' },
        { id: 4, similarity: 0.6, metadata: { source: 'source4' }, text: 'text4' },
        { id: 5, similarity: 0.5, metadata: { source: 'source5' }, text: 'text5' },
    ]
};
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

const { CosmosClient } = require("@azure/cosmos");
const searchDatabase = require('../searchDatabase');

describe ('searchDatabase.displayResults', () => {
    it('should return a string', () => {
        const results = [
            { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
            { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' }
        ];
        const result = searchDatabase.displayResults(results);
        expect(typeof result).toBe('string');
    });

    it('should format the results correctly', () => {
        const results = [
            { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
            { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' }
        ];
        const result = searchDatabase.displayResults(results);
        expect(result).toContain('Top 2 relevant results');
        expect(result).toContain('ID: 1');
        expect(result).toContain('Similarity: 0.9000');
        expect(result).toContain('Source: source1');
        expect(result).toContain('Text: text1...');
    });
}
);
describe('searchDatabase.formatResults', () => {
    it('should return an array of formatted results', () => {
        const results = [
            { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
            { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' }
        ];
        const result = searchDatabase.formatResults(results);
        expect(Array.isArray(result)).toBe(true);
        expect(result.length).toBe(2);
    });

    it('should format the results correctly', () => {
        const results = [
            { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
            { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' }
        ];
        const result = searchDatabase.formatResults(results);
        expect(result[0].id).toBe(1);
        expect(result[0].similarity).toBe('0.9000');
        expect(result[0].text).toBe('text1');
        expect(result[0].source).toEqual({ source: 'source1' });
    });
});

describe('searchDatabase.searchDatabase', () => {
    it('should return an array of results', async () => {
        const queryPrompt = 'test query';
        const topK = 5;
        const result = await searchDatabase.searchDatabase(queryPrompt, topK);
        expect(Array.isArray(result)).toBe(true);
        expect(result.length).toBe(topK);
        expect(result).toBe(mockFetchAll.resources);
    });

    it('should throw an error for invalid input', async () => {
        await expect(searchDatabase.searchDatabase('', 5)).rejects.toThrow();
    });
    it('should throw an error for invalid topK', async () => {  
        const queryPrompt = 'test query';
        await expect(searchDatabase.searchDatabase(queryPrompt, -1)).rejects.toThrow();
    });
});
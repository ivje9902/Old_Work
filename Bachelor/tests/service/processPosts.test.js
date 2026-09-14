const processPosts = require('../../service/processPosts');
const geminiAPI = require('../../service/googleGeminiAPI');
const searchDatabase = require('../../searchDatabase')

jest.mock('../../service/googleGeminiAPI', () => ({
    generateContentWithGemini: jest.fn()
}));

jest.mock('../../searchDatabase', () => ({
    searchDatabase: jest.fn()
}));

const mockApiResponse = JSON.stringify({ 'data': 'Mock API response'});
const mockDatabaseResult = [
    { id: 1, similarity: 0.9, metadata: { source: 'source1' }, text: 'text1' },
    { id: 2, similarity: 0.8, metadata: { source: 'source2' }, text: 'text2' }
];

beforeEach(() => {
    jest.restoreAllMocks();
});
afterEach(() => {
    jest.clearAllMocks();
});

afterAll(() => {
    geminiAPI.generateContentWithGemini.mockRestore();
    searchDatabase.searchDatabase.mockRestore();
})

describe('function generateAndParseContent', () => {
    test('should generate and parse content', async () => {
        const prompt = "Test prompt";
        const action = "Test function generateAndParseContent";

        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        const result = await processPosts.generateAndParseContent(prompt, action);
        expect(result).toBeDefined();
        expect(result).toStrictEqual({ data: 'Mock API response'});
    });
});

describe('function processQuestion', () => {
    let mockInputs = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "both"
    };
    
    test('should return tips both with and without RAG', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processQuestion(mockInputs);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("tips");
        expect(result.tips).toHaveProperty('data')
        expect(result.tips).toStrictEqual({ data: 'Mock API response'});
        expect(result).toHaveProperty("tipsRAG");
        expect(result.tipsRAG).toHaveProperty('data');
        expect(result.tipsRAG).toStrictEqual({ data: 'Mock API response'});
    });
    test('should return tips without RAG', async () => {
        mockInputs.mode = "no-RAG";
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processQuestion(mockInputs);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("tips");
        expect(result.tips).toHaveProperty('data');
        expect(result.tips).toStrictEqual({ data: 'Mock API response'});
        expect(result).toHaveProperty("tipsRAG");
        expect(result.tipsRAG).toHaveLength(0);
    });
    test('should return tips with RAG', async () => {
        mockInputs.mode = "with-RAG";
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processQuestion(mockInputs);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("tips");
        expect(result.tips).toHaveLength(0);
        expect(result).toHaveProperty("tipsRAG");
        expect(result.tipsRAG).toHaveProperty('data')
        expect(result.tipsRAG).toStrictEqual({ data: 'Mock API response'});
    });
    test('should throw error if no inputs provided', async () => {
    await expect(processPosts.processQuestion()).rejects.toThrow("No inputs provided");
    }); 
});

describe('function processExpand', () => {
    let mode = 'both';
    const mockInput = [
        { title: "Mock title 1",
            description: 'Mock description 1'
        },
        { title: "Mock title 2",
            description: 'Mock description 2'
        }
    ]
    const mockContext = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "both"
    };
    
    test('should return expanded tips both with and without RAG', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        searchDatabase.searchDatabase.mockResolvedValue(mockDatabaseResult);
        result = await processPosts.processExpand(mockInput, mockInput, mockInput, mockContext, mode);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("expanded");
        expect(typeof result.expanded).toBe('string')
        expect(JSON.parse(result.expanded)).toHaveProperty('data')
        expect(JSON.parse(result.expanded)).toStrictEqual({ data: 'Mock API response'});
        expect(result).toHaveProperty("expandedRAG");
        expect(typeof result.expandedRAG).toBe('string')
        expect(JSON.parse(result.expandedRAG)).toHaveProperty('data')
        expect(JSON.parse(result.expandedRAG)).toStrictEqual({ data: 'Mock API response'});
    });
    test('should return tips without RAG', async () => {
        mode = "no-RAG";
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        searchDatabase.searchDatabase.mockResolvedValue(mockDatabaseResult);
        result = await processPosts.processExpand(mockInput, mockInput, mockInput, mockContext, mode);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("expanded");
        expect(result.expandedRAG).toBeNull();
        expect(result).toHaveProperty("expandedRAG");
        expect(typeof result.expanded).toBe('string')
        expect(JSON.parse(result.expanded)).toHaveProperty('data')
        expect(JSON.parse(result.expanded)).toStrictEqual({ data: 'Mock API response'});
    });
    test('should return tips with RAG', async () => {
        mode = "with-RAG";
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        searchDatabase.searchDatabase.mockResolvedValue(mockDatabaseResult);
        result = await processPosts.processExpand(mockInput, mockInput, mockInput, mockContext, mode);
        expect(result).toBeDefined();
        expect(result).toHaveProperty("expanded");
        expect(result.expanded).toBeNull();
        expect(result).toHaveProperty("expandedRAG");
        expect(typeof result.expandedRAG).toBe('string')
        expect(JSON.parse(result.expandedRAG)).toHaveProperty('data')
        expect(JSON.parse(result.expandedRAG)).toStrictEqual({ data: 'Mock API response'});
    });
});

describe('function processGeneration', () => {
    const mockInput = [
        { title: "Mock title 1",
            description: 'Mock description 1'
        },
        { title: "Mock title 2",
            description: 'Mock description 2'
        }
    ]
    const mockContext = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "no-RAG"
    };
    test('should return mocked response without RAG', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processGeneration(mockInput, mockContext, 'Guide', 'no-RAG');
        expect(result).toBeDefined();
        expect(result).toHaveProperty('answer');
        expect(result.answer).toBe(mockApiResponse)
    });
    test('should return mocked response with RAG', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processGeneration(mockInput, mockContext, 'Guide', 'with-RAG');
        expect(result).toBeDefined();
        expect(result).toHaveProperty('answer');
        expect(result.answer).toBe(mockApiResponse)
    });
});

describe('function processOffert', () => {
    const mockInput = [
        { title: "Mock title 1",
            description: 'Mock description 1'
        },
        { title: "Mock title 2",
            description: 'Mock description 2'
        }
    ]
    const mockContext = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "no-RAG"
    };
    test('should return mocked response', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processOffert(mockInput, mockContext, "no-RAG");
        expect(result).toBeDefined();
        expect(result).toHaveProperty('answer');
        expect(result.answer).toBe(mockApiResponse)
    });
});

describe('function processGuide', () => {
    const mockInput = [
        { title: "Mock title 1",
            description: 'Mock description 1'
        },
        { title: "Mock title 2",
            description: 'Mock description 2'
        }
    ]
    const mockContext = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "both"
    };
    test('should return mocked response', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processGuide(mockInput, mockContext, "no-RAG");
        expect(result).toBeDefined();
        expect(result).toHaveProperty('answer');
        expect(result.answer).toBe(mockApiResponse)
    });
});

describe('function processSaving', () => {
    const mockInput = [
        { title: "Mock title 1",
            description: 'Mock description 1'
        },
        { title: "Mock title 2",
            description: 'Mock description 2'
        }
    ]
    const mockContext = {
        byggnadstyp: "Enfamiljshus",
        Uppvärmningsmetod: "Luft/vattenvärmepump",
        energisystem: "El",
        energikonsumption: 200,
        storlek: 150,
        byggnadsår: 1999,
        mode: "both"
    };
    test('should return mocked response', async () => {
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        result = await processPosts.processSaving(mockInput, mockContext, "no-RAG");
        expect(result).toBeDefined();
        expect(result).toHaveProperty('answer');
        expect(result.answer).toBe(mockApiResponse)
    });
});

describe('function processForm', () => {
    test('should print three lines and return nothing', async () => {
        const inputs = {
            satisfaction: "satisfaction",
            relevance: "relevance",
            additional_info: "additional_info"
        }
        const result = await processPosts.processForm(inputs)
        expect(result).toBeUndefined();
    })
})
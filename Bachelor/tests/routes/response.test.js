const request = require('supertest');

const mockUploadedFile = { id: 'file123', name: 'file.pdf' };
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

// Test suite for the backend API
describe('POST /response/question', () => {
    it('Check if function responds with error code on invalid input', async () => {
        const response = await request(app)
            .post('/response/question')
            .send({ inputs: null })
            .expect('Content-Type', /.*json/)
            .expect(400);

        expect(response.body).not.toHaveProperty('tips');
    });

    it('Check if "no-RAG" request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999,
        };

        const response = await request(app)
            .post('/response/question')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /.*json/)
            .expect(200);

        expect(response).toBeDefined();
        expect(response).toHaveProperty('body');
        expect(response.body).not.toHaveProperty('tipsRAG');
        expect(response.body).toHaveProperty('tips');
        expect(response.body.tips).not.toBeNull();
        expect(response.body.tips[0]).toHaveProperty('title');
        expect(response.body.tips[0]).toHaveProperty('description');
        expect(response.body.tips[0].title).not.toBeNull();
        expect(response.body.tips[0].description).not.toBeNull();
        expect(typeof response.body.tips[0].title).toBe('string');
        expect(typeof response.body.tips[0].description).toBe('string');

        for (const answer of response.body.tips) {
            expect(answer).not.toBeNull();
        }
    });
    it('Check if "with-RAG" request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999,
            mode: "with-RAG"
        };

        const response = await request(app)
            .post('/response/question')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /.*json/)
            .expect(200);

        expect(response).toBeDefined();
        expect(response).toHaveProperty('body');
        expect(response.body).not.toHaveProperty('tips');
        expect(response.body).toHaveProperty('tipsRAG');
        expect(response.body.tipsRAG).not.toBeNull();
        expect(response.body.tipsRAG[0]).toHaveProperty('title');
        expect(response.body.tipsRAG[0]).toHaveProperty('description');
        expect(response.body.tipsRAG[0].title).not.toBeNull();
        expect(response.body.tipsRAG[0].description).not.toBeNull();
        expect(typeof response.body.tipsRAG[0].title).toBe('string');
        expect(typeof response.body.tipsRAG[0].description).toBe('string');

        for (const answer of response.body.tipsRAG) {
            expect(answer).not.toBeNull();
        }

    });
    it('Check if "both" request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999,
            mode: "both"
        };

        const response = await request(app)
            .post('/response/question')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /.*json/)
            .expect(200);

        expect(response).toBeDefined();
        expect(response).toHaveProperty('body');
        expect(response.body).toHaveProperty('tips');
        expect(response.body.tips).not.toBeNull();
        expect(response.body.tips[0]).toHaveProperty('title');
        expect(response.body.tips[0]).toHaveProperty('description');
        expect(response.body.tips[0].title).not.toBeNull();
        expect(response.body.tips[0].description).not.toBeNull();
        expect(typeof response.body.tips[0].title).toBe('string');
        expect(typeof response.body.tips[0].description).toBe('string');

        for (const answer of response.body.tips) {
            expect(answer).not.toBeNull();
        }

        expect(response).toBeDefined();
        expect(response).toHaveProperty('body');
        expect(response.body).toHaveProperty('tipsRAG');
        expect(response.body.tipsRAG).not.toBeNull();
        expect(response.body.tipsRAG[0]).toHaveProperty('title');
        expect(response.body.tipsRAG[0]).toHaveProperty('description');
        expect(response.body.tipsRAG[0].title).not.toBeNull();
        expect(response.body.tipsRAG[0].description).not.toBeNull();
        expect(typeof response.body.tipsRAG[0].title).toBe('string');
        expect(typeof response.body.tipsRAG[0].description).toBe('string');

        for (const answer of response.body.tipsRAG) {
            expect(answer).not.toBeNull();
        }

    });
});


// Test suite for the backend API
describe('POST /response/expand', () => {
    it('Check if function responds with error code on invalid input', async () => {
        const response = await request(app)
            .post('/response/expand')
            .send({ inputs: { context: "", input: null} }) // context kan vara tomt eller relevant text
            .expect('Content-Type', /json/)
            .expect(400);

        expect(response.body).not.toHaveProperty('expanded');
    });
    // Need to add more tests for the expand function
});

describe('POST /response/offert', () => {
    it('Check if function responds with error code on invalid input', async () => {
        const response = await request(app)
            .post('/response/offert')
            .send({ inputs: null })
            .expect('Content-Type', /json/)
            .expect(400);
        expect(response.body).not.toBeNull();
        expect(response.body).not.toHaveProperty('answer');
    });

    it('Check if request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999
        };

        const response = await request(app)
            .post('/response/offert')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /json/)
            .expect(200);

        expect(response.body).toHaveProperty('answer');
        expect(response.body.expandedText).not.toBeNull();
        expect(typeof response.body.answer).toBe('string');
    });
});

describe('POST /response/guide', () => {
    it('Check if function responds with error code on invalid input', async () => {
        const response = await request(app)
            .post('/response/guide')
            .send({ inputs: null })
            .expect('Content-Type', /json/)
            .expect(400);

        expect(response.body).not.toHaveProperty('answer');
    });

    it('Check if request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999
        };

        const response = await request(app)
            .post('/response/guide')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /json/)
            .expect(200);

        expect(response.body).toHaveProperty('answer');
        expect(response.body.expandedText).not.toBeNull();
        expect(typeof response.body.answer).toBe('string');
    });
});

describe('POST /response/saving', () => {
    it('Check if function responds with error code on invalid input', async () => {
        const response = await request(app)
            .post('/response/saving')
            .send({ inputs: null })
            .expect('Content-Type', /json/)
            .expect(400);

        expect(response.body).not.toHaveProperty('answer');
    });

    it('Check if request is answered in correct format and contains the correct values', async () => {
        const mockInputs = {
            byggnadstyp: "Enfamiljshus",
            Uppvärmningsmetod: "Luft/vattenvärmepump",
            energisystem: "El",
            energikonsumption: 200,
            storlek: 150,
            byggnadsår: 1999
        };

        const response = await request(app)
            .post('/response/saving')
            .send({ inputs: mockInputs })
            .expect('Content-Type', /json/)
            .expect(200);

        expect(response.body).toHaveProperty('answer');
        expect(response.body.expandedText).not.toBeNull();
        expect(typeof response.body.answer).toBe('string');
    });
});
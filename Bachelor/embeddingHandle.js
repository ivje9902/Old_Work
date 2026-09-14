const { GoogleGenAI } = require("@google/genai");
require('dotenv').config();
const apiKey = process.env.GEMINI_API_KEY;
const client = new GoogleGenAI({ apiKey: apiKey });
const { v4: uuidv4 } = require('uuid');
const { CosmosClient } = require('@azure/cosmos');
const cosmosClient = new CosmosClient({
    endpoint: process.env.COSMOS_ENDPOINT,
    key: process.env.COSMOS_KEY
});

const databaseName = process.env.TEST_COSMOS_DATABASE || 'rag-database';
const containerName = process.env.TEST_COSMOS_CONTAINER || 'chunks';

const database = cosmosClient.database(databaseName);
const container = database.container(containerName);


/**
 * Generates a vector embedding for the given text using the Google GenAI API.
 * 
 * @param {string} text - The input text to generate the embedding for. Must be a non-empty string.
 * @returns {Promise<number[]>} A promise that resolves to an array of numbers representing the embedding vector.
 * @throws {Error} If the input text is invalid or the embedding generation fails.
 */
async function generateEmbeddings(text) {
    if (!text || typeof text !== 'string' || text === '') {
        throw new Error('Invalid input: text must be a non-empty string');
    }
    const response = await client.models.embedContent({
        model: 'text-embedding-004',
        contents: [text],
        embeddingConfig: {
            outputDimensionality: 768
        }
    });
    return response.embeddings[0].values;
}


/**
 * Saves a vector embedding along with its associated metadata and text to the Cosmos DB.
 * 
 * @param {string} text - The input text associated with the embedding. Must be a non-empty string.
 * @param {number[]} embedding - The embedding vector to save. Must be a non-empty array of numbers.
 * @param {Object} metadata - Metadata associated with the embedding, such as source or date accessed.
 * @param {boolean} [dummyData=false] - Indicates whether the data is dummy/test data.
 * @param {string} [id] - The unique identifier for the embedding. Required if `dummyData` is `true`.
 * @returns {Promise<Object>} A promise that resolves to the saved item object.
 * @throws {Error} If the input is invalid or saving to the database fails.
 */
async function saveEmbedding(text, embedding, metadata, dummyData = false, id) {
    if (!text || typeof text !== 'string' || text === '') {
        throw new Error('Invalid input: text must be a non-empty string');
    }
    if (!Array.isArray(embedding) || embedding.length === 0) {
        throw new Error('Invalid input: embedding must be a non-empty array');
    }
    if (dummyData && !id) {
        throw new Error('Invalid input: id must be provided for dummy data');
    }
    if (dummyData && typeof id !== 'string') {
        throw new Error('Invalid input: id must be a string for dummy data');
    }
    if (!dummyData && !id){
        id = uuidv4();
    }
    const item = {
        id: id,
        dummyData: dummyData,
        metadata: metadata,
        text: text,
        embedding: embedding
    };
    try {
        await container.items.create(item);
        return item;
    } catch (error) {
        throw new Error(`Error saving embedding: ${error.message}`);
    }

}

/**
 * Performs a vector similarity search in the Cosmos DB to find the most relevant items.
 * 
 * @param {number[]} queryEmbedding - The embedding vector to search for. Must be a non-empty array of numbers.
 * @param {number} [topK=5] - The number of top results to return. Must be a positive number.
 * @returns {Promise<Object[]>} A promise that resolves to an array of objects representing the search results, 
 * each containing the item's ID, text, metadata, and similarity score.
 * @throws {Error} If the input is invalid or the database query fails.
 */
async function vectorSearch(queryEmbedding, topK = 5) {
    if (!Array.isArray(queryEmbedding) || queryEmbedding.length === 0) {
        throw new Error('Invalid input: queryEmbedding must be a non-empty array');
    }
    if (typeof topK !== 'number' || topK <= 0) {
        throw new Error('Invalid input: topK must be a positive number');
    }
    const query = {
        query: `
        SELECT TOP @topK c.id, c.text, c.metadata, VectorDistance(c.embedding, @queryVector) AS similarity
        FROM c
        ORDER BY VectorDistance(c.embedding, @queryVector)
        `,
        parameters: [
            { name: '@topK', value: topK },
            { name: '@queryVector', value: queryEmbedding }
        ]
    };
    
    const { resources: results } = await container.items
    .query(query, { enableCrossPartitionQuery: true })
    .fetchAll();
    
    return results;
}

/**
 * Checks if a document with the given path already exists in the Cosmos DB.
 * 
 * @param {string} path - The file path to check in the database.
 * @returns {Promise<boolean>} A promise that resolves to `true` if the document exists, otherwise `false`.
 * @throws {Error} If the path is invalid or the database query fails.
 */
async function checkIfDocumentInDatabase(path) {
    if (!path) {
        throw Error('Path is required');
    }
    if (typeof path !== 'string') {
        throw Error('Path must be a string');
    }
    if (path === '') {
        throw Error('Path cannot be empty');
    }
    const query = {
        query: `SELECT * FROM c WHERE c.metadata.source = @path`,
        parameters: [
            { name: '@path', value: path }
        ]
    };
    return await container.items
        .query(query, { enableCrossPartitionQuery: true })
        .fetchAll()
        .then(result => {
            return result.resources.length > 0;
        });
}

module.exports = {
    generateEmbeddings,
    saveEmbedding,
    vectorSearch,
    checkIfDocumentInDatabase 
};
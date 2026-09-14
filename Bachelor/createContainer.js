require('dotenv').config();
const { CosmosClient } = require('@azure/cosmos');

/**
 * Initializes the Cosmos DB client using credentials from environment variables.
 * 
 * @constant {CosmosClient} client - The Cosmos DB client instance.
 */
const client = new CosmosClient({
    endpoint: process.env.COSMOS_ENDPOINT,
    key: process.env.COSMOS_KEY
});

const databaseId = 'rag-database';
const containerId = 'chunks';
const partitionKeyPath = '/id';
// This value much match the embedding dimensions used in createEmbedding function
// in embeddingHandle.js
const embeddingDimensions = 768;    

/**
 * Sets up the Cosmos DB database and container with the specified configuration.
 * 
 * - Creates the database if it does not already exist.
 * - Creates the container with vector indexing on the `/embedding` path if it does not already exist.
 * 
 * @async
 * @function setup
 * @returns {Promise<void>} A promise that resolves when the setup is complete.
 * @throws {Error} If there is an error during database or container creation.
 */
async function setup() {
    const { database } = await client.databases.createIfNotExists({ id: databaseId });
    console.log(`✅ Database ready: ${databaseId}`);

    const containerDef = {
        id: containerId,
        partitionKey: {
            paths: [partitionKeyPath],
            kind: 'Hash'
        },
        indexingPolicy: {
            indexingMode: 'consistent',
            automatic: true,
            includedPaths: [
                { path: '/*' }
            ],
            excludedPaths: [
                { path: '/_etag/?' }
            ]
        },
        vectorIndexes: [
            {
                path: '/embedding',
                kind: 'quantized-flat',
                dataType: 'Number',
                dimensions: embeddingDimensions,
                metric: 'cosine'
            }
        ]
    };

    const { container } = await database.containers.createIfNotExists(containerDef);
    console.log(`✅ Container ready: ${containerId} with vector indexing on /embedding`);
}

setup().catch((err) => {
    console.error('❌ Error setting up Cosmos DB:', err.message || err);
});

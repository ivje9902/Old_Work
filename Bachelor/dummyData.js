require('dotenv').config();
const { CosmosClient } = require('@azure/cosmos');
const cosmosClient = new CosmosClient({
    endpoint: process.env.COSMOS_ENDPOINT,
    key: process.env.COSMOS_KEY
});

const databaseName = process.env.TEST_COSMOS_DATABASE || 'rag-database';
const containerName = process.env.TEST_COSMOS_CONTAINER || 'chunks';
const database = cosmosClient.database(databaseName);
const container = database.container(containerName);




async function deleteDummyData() {
    const query = 'SELECT * FROM c WHERE c.dummyData = true';
    const { resources: items } = await container.items.query(query).fetchAll();
    for (const item of items) {
        await container.item(item.id, item.id).delete();
        console.log(`Deleted dummy data item with ID: ${item.id}`);
    }
}

async function main() {
    await deleteDummyData();
}

module.exports = {
    deleteDummyData
};

// main()
//     .then(() => console.log('Dummy data deletion completed.'))
//     .catch(err => console.error(err));
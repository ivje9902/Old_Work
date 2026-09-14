const fs = require('fs');
const {generateEmbeddings, saveEmbedding, checkIfDocumentInDatabase} = require('./embeddingHandle');
const { Document } = require('@langchain/core/documents');
const { RecursiveCharacterTextSplitter } = require('@langchain/textsplitters');
const { PDFLoader } = require("@langchain/community/document_loaders/fs/pdf");
const { v4: uuidv4 } = require('uuid');
const pdf = require('pdf-parse');
const path = require('path');

/**
 * Inserts a document into the Cosmos DB by splitting it into chunks and generating embeddings for each chunk.
 * 
 * @param {Document} doc - The document to insert. Must be an instance of the `@langchain/core/documents` class.
 * @returns {Promise<Object>} A promise that resolves to the last inserted chunk object.
 * @throws {Error} If the document is invalid or the insertion process fails.
 */
async function insertDatabase(doc) {
    if (!doc) {
        throw Error('Document is required');
    }
    if (!(doc instanceof Document)) {
        throw Error('Document must be of type Document');
    }
    if (!doc.pageContent || doc.pageContent === '') {
        throw Error('Document must have text');
    }
    const chunks = await chunkDocument(doc);
    const noChunks = chunks.length;
    process.stdout.write('\n');
    let currentChunk = 0;
    for (const documentChunk of chunks) {
        currentChunk ++;
        process.stdout.write(`🔄 Processing chunk ${currentChunk} of ${noChunks}\r`); // Overwrite the current line
        const embedding = await generateEmbeddings(documentChunk.pageContent); 
        let dummyData = false;
        let id = null;
        if (doc.metadata.source.includes('./material/AI-generated/' || doc)) {
            dummyData = true;
            id = 'test-id-';
            id += uuidv4();
        }
        inserted = await saveEmbedding(documentChunk.pageContent, embedding, documentChunk.metadata, dummyData, id);
        await sleep(600);
    }
    return inserted;
}

/**
 * Splits a document into smaller chunks using a recursive character text splitter.
 * 
 * @param {Document} doc - The document to split. Must be an instance of the `@langchain/core/documents` class.
 * @returns {Promise<Document[]>} A promise that resolves to an array of document chunks.
 * @throws {Error} If the document is invalid or the splitting process fails.
 */
async function chunkDocument(doc) {
    if (!doc) {
        throw Error('Document is required');
    }
    if (!(doc instanceof Document)) {
        throw Error('Document must be of type Document');
    }
    if (!doc.pageContent) {
        throw Error('Document must have text');
    }
    const splitter = new RecursiveCharacterTextSplitter({
        chunkSize: 1000,
        chunkOverlap: 200
    });

    const documentChunks = await splitter.splitDocuments([doc]);
    return documentChunks;
}

/**
 * Loads a PDF file and extracts its content into a @langchain/core/documents object.
 * 
 * @param {string} filePath - The full file path of the PDF to load.
 * @returns {Promise<Document>} A promise that resolves to a @langchain/core/documents object containing the PDF content and metadata.
 * @throws {Error} If the file does not exist, is not a valid PDF, or the loading process fails.
 */
async function loadPDF(filePath) {
    if (!filePath || filePath === '') {
        throw new Error('Path is required');
    }
    if (typeof filePath !== 'string') {
        throw new Error('Path must be a string');
    }
    if (!fs.existsSync(filePath)) { 
        throw new Error(`File not found: ${filePath}`);
    }
    if (!fs.lstatSync(filePath).isFile()) {
        throw new Error(`Path is not a file: ${filePath}`);
    }
    if (!filePath.endsWith('.pdf')) {
        throw new Error(`File is not a .pdf file: ${filePath}`);
    }

    const loader = new PDFLoader(filePath, {
        splitPages: false,
    });
    const docs = await loader.load();

    const dataBuffer = fs.readFileSync(filePath);
    
    // Suppress warnings during pdf-parse execution
    const originalStderrWrite = process.stderr.write;
    process.stderr.write = () => {}; // Suppress stderr output
    const pdfData = await pdf(dataBuffer);
    process.stderr.write = originalStderrWrite; // Restore stderr

    let extractedUrl = '';
    let extractedDateAccessed = '';

    const subject = pdfData.info?.Subject || '';
    if (subject.includes('Source:') && subject.includes('Date Accessed:')) {
        const parts = subject.split(',');
        extractedUrl = parts[0].replace('Source:', '').trim();
        extractedDateAccessed = parts[1].replace('Date Accessed:', '').trim();
    } else {
        extractedUrl = '';
        extractedDateAccessed = new Date().toISOString().split('T')[0];
    }

    docs[0].metadata = {
        ...docs[0].metadata,
        source: filePath,
        url: extractedUrl,
        date_accessed: extractedDateAccessed
    };

    return docs[0];
}

/**
 * Pauses execution for a specified amount of time.
 * 
 * @param {number} ms - The number of milliseconds to sleep.
 * @returns {Promise<void>} A promise that resolves after the specified time.
 */
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

/**
 * Main function to process PDF files and insert their content into the Cosmos DB.
 * 
 * - If a specific file path is provided as a command-line argument, only that file is processed.
 * - Otherwise, all PDF files in the default folder are processed.
 * 
 * @returns {Promise<void>} A promise that resolves when all files are processed.
 * @throws {Error} If any file processing or database operation fails.
 */
async function main() {
    const specificFile = process.argv[2]; // Get the full file path from the command line argument
    let files;

    if (specificFile) {
        // Use the provided full file path directly
        if (!fs.existsSync(specificFile)) {
            console.error(`❌ File not found: ${specificFile}`);
            return;
        }
        files = [specificFile];
    } else {
        // Default to processing all files in the folder
        console.log("HERE")
        const folderPath = process.env.TEST_FOLDER_PATH || 'material/data/';
        files = fs.readdirSync(folderPath).map(file => path.join(folderPath, file));
    }

    for (const file of files) {
        if (!file.endsWith('.pdf')) continue;

        const filePath = file; // Use the full file path directly
        const filename = path.basename(filePath);

        if (await checkIfDocumentInDatabase(filePath)) {
            console.log(`Document already exists in the database: ${filename}`);
            continue;
        }

        console.log(`🔄 Processing file: ${filename}`);
        let document;
        try {
            document = await loadPDF(filePath);
        } catch (error) {
            console.error(`Error loading PDF: ${error.message}`);
            continue;
        }
        if (!document) {
            console.log(`❌ Failed to load document: ${filename}`);
        } else {
            await insertDatabase(document);
            console.log(`✅ Finished processing file: ${filename}`);
        }
    }
}

if (require.main === module) {
    main()
        .catch(err => console.error(err));
}

module.exports = {
    insertDatabase,
    chunkDocument,
    loadPDF,
    main
};

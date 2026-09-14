const fs = require('fs');
const path = require('path');
const { searchDatabase, formatResults } = require('../searchDatabase');
const { createPartFromUri } = require('./googleGeminiAPI');

/**
 * Retrieves a prompt by its ID from the prompts.txt file.
 * @async
 * @param {string} promptId - The ID of the prompt to retrieve.
 * @returns {Promise<string>} The content of the prompt.
 * @throws {Error} If the prompt is not found or the file cannot be read.
 */
async function getPromptById(promptId) {
    try {
        const filePath = path.join(__dirname, '../prompts.txt');
        const fileContent = await fs.promises.readFile(filePath, 'utf-8');

        const regex = new RegExp(`Prompt${promptId}Start\\s*([\\s\\S]*?)\\s*Prompt${promptId}End`);
        const match = fileContent.match(regex);

        if (match && match[1].trim()) {
            return match[1].trim();
        } else {
            throw new Error(`Prompt '${promptId}' not found or empty.`);
        }
    } catch (err) {
        throw new Error(`Failed to load prompt '${promptId}': ${err.message}`);
    }
}

/**
 * Creates a prompt message based on the given ID, input, and context.
 * @async
 * @param {string} ID - The ID of the prompt to retrieve.
 * @param {string} input - The input data for the prompt, between 1-6 tips. 
 * @param {string} context - The context related to the prompt, usually building information.
 * @returns {Promise<string[]>} A promise that resolves to an array containing the generated prompt message.
 * @throws {Error} If the prompt cannot be retrieved.
 */
async function makePrompt(ID, input, context) {
    const prompt = await getPromptById(ID);
    const message = `${prompt}
    Åtgärder:
    ${input}
    Här är kontext gällande bostaden ${context}`
    return [message];
}

/**
 * Creates a prompt message specifically for asking a question based on building information.
 * This is a specialized edge case of the `makePrompt` function.
 * @async
 * @param {Object} buildingInfo - The building information to include in the prompt.
 * @returns {Promise<string[]>} A promise that resolves to an array containing the generated prompt message.
 * @throws {Error} If the prompt cannot be retrieved.
 */
async function makePromptQuestion(buildingInfo) {
    const prompt = await getPromptById("Question");
    const message = `${prompt}
    Fastighetsinformation:
    ${JSON.stringify(buildingInfo)}`;
    return [message];
}

/**
 * Creates a prompt message for expanding answers by combining predefined prompt text with input and context.
 * @async
 * @param {Object} context - The context related to the prompt, usually building information.
 * @param {Object} input - The input data for the prompt, between 1-6 tips.
 * @returns {Promise<string[]>} - A promise that resolves to an array containing the generated prompt message.
 */
async function makePromptExpand(context, input) {
    const prompt = await getPromptById('Expand');
    const message = `
    ${prompt}
    ${JSON.stringify(input)}
    Kontext från tidigare konversation:
    ${JSON.stringify(context)}

    `;

    console.log("Message expand", message);
    return [message];

}

/** 
 * Creates a prompt message for generating energy savings calculations by combining predefined prompt text with input and context. 
 * @param {Object} context - The context related to the prompt, usually building information.
 * @param {Object} input - The input data for the prompt, usually between 1-6 tips. 
 * @returns {Array<string>} - A promise that resolves to an array containing the generated prompt message.
 */
function makePromptSaving(input, context) {

    const message = `Generera en beräkning av energibesparingar för följande åtgärder på en bostad:
    ${input}
    Här är kontext gällande bostaden ${context}
    Printa inte kontexten i svaret.
    Svara på svenska och skriv med tydlig struktur:
    Skriv inte "Hej" eller "Med vänlig hälsning" i början eller slutet av meddelandet.`
    return [message];
}

/**
 * Enhances a prompt message by adding relevant context from the database.
 * This function searches the database for entries similar to the provided inputs
 * and appends the formatted results to the prompt message.
 * @async
 * @param {string} promptMessage - The original prompt message to enhance.
 * @param {Object[]} inputs - An array of input objects used to search the database.
 * @returns {Promise<string[]>} A promise that resolves to an array containing the enhanced prompt message.
 * @throws {Error} If the database search or formatting fails.
 */
async function addRAGContext(promptMessage, inputs) {
    const resultsString = JSON.stringify(inputs);
    const searchResults = await searchDatabase(resultsString, 5);
    // const formattedResults = formatResults(searchResults);
    // formattedResults was removed because of missmatch with new pdf metadata implementation, if anything breaks it might be because this needs to be used.

    const messageAddon =
        `\nHär är de inlägg i vår databas som har bäst similarity-score i relation till fastighetsinformationen: 
    ${JSON.stringify(searchResults)}

    Använd endast informationen från databasen för att ge ett relevant svar på ovanstående. 
    Om du inte kan ge 6 tips baserat på denna information, ge så många tips som du kan.
    Om du inte kan ge ett svar baserat på denna information, skriv "Jag kan tyvärr inte ge något svar på detta."
    Efter varje punkt i punktlistan ska du skriva "Källa: [filnamnet som finns i "source" fältet]".
    `;
    return [promptMessage + messageAddon];
}

/**
 * Creates a prompt message for uploading files by combining predefined prompt text with inserted file data.
 * @param {string} fileType - The type of the file being uploaded.
 * @param {Object} fileData - The data of the file being uploaded.
 * @param {string} fileName - The name of the file being uploaded.
 * @returns {Array<string>} - An array containing the generated prompt message.
 * @throws {Error} If the file type is unsupported.
 */
async function constructUploadPrompt(fileType, fileData, fileName) {
    let contents = [];

    switch (fileType) {
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
        case "application/vnd.ms-excel":
            const excelPrompt = await getPromptById("Excel");
            contents = [
                `Här är en Excelfil med timvis förbrukningsdata: ${fileName}`,
                JSON.stringify(fileData.data),
                "\n\n",
                excelPrompt,
            ];
            break;

        case "application/pdf":
            const pdfPrompt = await getPromptById("pdf");
            contents = [
                `Analysera följande PDF fil: ${fileName}`,
                createPartFromUri(fileData.uri, fileType),
                "\n\n",
                pdfPrompt,
            ];
            break;

        default:
            throw new Error("Unsupported file type");
    }
    return contents;
}


module.exports = {
    getPromptById,
    makePrompt,
    makePromptQuestion,
    makePromptExpand,
    makePromptSaving,
    addRAGContext,
    constructUploadPrompt,
};

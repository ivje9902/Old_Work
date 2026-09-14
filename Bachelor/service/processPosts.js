const { makePromptQuestion, addRAGContext, makePromptExpand, makePrompt, makePromptSaving } = require('./promptService');
const { generateContentWithGemini } = require('./googleGeminiAPI');
const { cleanJsonString } = require('../utils/cleanUp');
const { retryApiCall } = require('../utils/retry');


/**
 * Generates content using the Google Gemini API and parses the response into JSON.
 * @async
 * @param {string} prompt - The input prompt to send to the API.
 * @param {string} action - A description of the action being performed (used for logging or debugging).
 * @returns {Promise<Object>} A promise that resolves to the parsed JSON response.
 * @throws {Error} If the API call or JSON parsing fails.
 */
async function generateAndParseContent(prompt, action) {
    return await retryApiCall(async () => {
        const response = await generateContentWithGemini(prompt);
        const cleanJsonText = cleanJsonString(response);
        return JSON.parse(cleanJsonText);
    }, action);
}

/**
 * Processes a question by generating prompts and responses using the API.
 * @async
 * @param {Object} inputs - The input data for the question. Contains building information and other parameters.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `tips`: The tips generated without using RAG (retrieval-augmented generation).
 * - `tipsRAG`: The tips generated using RAG.
 * @throws {Error} If there is an error during prompt creation, response generation, or JSON parsing.
 */
async function processQuestion(inputs) {
    if (!inputs) {
        throw new Error("No inputs provided");
    }
    
    try {
        const mode = inputs.mode || "no-RAG";

        let tips = [];
        let tipsRAG = [];

        if (mode === "both") {
            console.log("Process Question with both RAG and no-RAG");

            const promptWithoutContext = await makePromptQuestion(inputs);
            const promptRAG = await addRAGContext(promptWithoutContext, inputs);

            tips = await generateAndParseContent(promptWithoutContext, "Question without RAG");
            tipsRAG = await generateAndParseContent(promptRAG, "Question with RAG");

            console.log("Prompt with RAG created successfully");
        } else if (mode === "no-RAG") {
            console.log("Process Question without RAG");

            const promptWithoutContext = await makePromptQuestion(inputs);

            tips = await generateAndParseContent(promptWithoutContext, "Question without RAG");
            console.log("Prompt without RAG created successfully");

        } else if (mode === "with-RAG") {

            console.log("Process Question with RAG");

            const promptWithoutContext = await makePromptQuestion(inputs);
            const promptRAG = await addRAGContext(promptWithoutContext, inputs);
            

            tipsRAG = await generateAndParseContent(promptRAG, "Question with RAG");
            console.log("Prompt with RAG created successfully");
        }

        return { tips, tipsRAG };

    } catch (error) {
        console.error("Error processing question:", error);
        throw new Error("Error processing question");
    }
}

/**
 * Processes an expansion request by generating prompts and responses using the API.
 * @async
 * @param {string} input - The specific tip or input to expand upon.
 * @param {string} noRAGInput - The input data for the non-RAG expansion.
 * @param {string} RAGInput - The input data for the RAG expansion.
 * @param {string} context - The context from a previous conversation to include in the prompt.
 * Usualy building information.
 * @param {string} mode - The mode of operation: "both", "no-RAG", or "with-RAG".
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `expanded`: The expanded content generated without using RAG.
 * - `expandedRAG`: The expanded content generated using RAG.
 * @throws {Error} If there is an error during prompt creation, response generation, or JSON parsing.
 */
async function processExpand(input, noRAGInput, RAGInput, context, mode) {
    try {
        // Construct prompts
        let promptWithoutRAG = null;
        let promptRAG = null;
        let expanded = null;
        let expandedRAG = null;

        
        console.log(mode, "mode");
        if (mode === "both") {
            console.log("Using both RAG and no-RAG");
            promptWithoutRAG = await makePromptExpand(context, noRAGInput);
            promptRAG = await addRAGContext(await makePromptExpand("", RAGInput), RAGInput);

            expanded = await retryApiCall(() => generateContentWithGemini(promptWithoutRAG), "Expand without RAG");
            expandedRAG = await retryApiCall(() => generateContentWithGemini(promptRAG), "Expand with RAG");

        } else if (mode === "no-RAG") {
            console.log("Using only no-RAG");
            promptWithoutRAG = await makePromptExpand(context, input);

            expanded = await retryApiCall(() => generateContentWithGemini(promptWithoutRAG), "Expand without RAG");


        } else if (mode === "with-RAG") {
            console.log("Using only  RAG");
            promptRAG = await addRAGContext(await makePromptExpand(context, input), input);

            expandedRAG = await retryApiCall(() => generateContentWithGemini(promptRAG), "Expand with RAG");
        }

        console.log("Expanded responses generated successfully");

        return { expanded, expandedRAG };
    } catch (error) {
        console.error("Error processing expand:", error);
        throw new Error("Error processing expand");
    }
}

/**
 * Processes the generation of content based on the provided input, context, and process type.
 * @async
 * @param {Object} input - The input data used to construct the prompt.
 * @param {Object} context - The context information to include in the prompt.
 * @param {string} processType - The type of processing to perform.
 * @returns {Promise<Object>} A promise that resolves to an object containing the generated answer.
 * @throws {Error} Throws an error if the generation process fails.
 */
async function processGeneration(input, context, processType, mode) {
    try {
        let answer = "";
        if(mode === "no-RAG") {
            console.log("Using no-RAG");

            const prompt = await makePrompt(processType, input, JSON.stringify(context));
            console.log("Prompt created successfully");

            console.log("Generating responses...");
            const response = await retryApiCall(() => generateContentWithGemini(prompt), processType);
            console.log("Response generated successfully");

            answer = response;
        } else if (mode === "with-RAG") {
            console.log("Using RAG");
            const prompt = await addRAGContext(await makePrompt(processType, input, JSON.stringify(context)), input)
            console.log("Prompt created successfully");
            
            console.log("Generating responses...");
            const response = await retryApiCall(() => generateContentWithGemini(prompt), processType);
            console.log("Response generated successfully");

            answer = response
        }       

        return { answer };

    } catch (error) {
        console.error(`Error processing ${processType}`, error);
        throw new Error(`Error processing ${processType}`);
    }
}


/**
 * Processes an offert request by generating a prompt and response using Gemini API.
 * @async
 * @param {string} input - The input data for the offert request.
 * @param {Object} context - The context related to the offert request.
 * Usualy building information.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `answer`: The response generated by the API.
 * @throws {Error} If there is an error during prompt creation or response generation.
 */
async function processOffert(input, context, mode) {
    try {
        return await processGeneration(input, context, "Offert", mode);

    } catch (error) {
        console.error("Error processing offert:", error);
        throw new Error("Error processing offert");
    }
}

/**
 * Processes a guide request by generating a prompt and response using the API.
 * @async
 * @param {string} input - The input data for the guide request.
 * @param {Object} context - The context related to the guide request.
 * Usualy building information.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `answer`: The response generated by the API.
 * @throws {Error} If there is an error during prompt creation or response generation.
 */
async function processGuide(input, context, mode) {
    try {
        return await processGeneration(input, context, "Guide", mode);

    } catch (error) {
        console.error("Error processing guide:", error);
        throw new Error("Error processing guide");
    }
}


/**
 * Processes a saving request by generating a prompt and response using the API.
 * @async
 * @param {string} input - The input data for the saving request.
 * @param {Object} context - The context related to the saving request.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `answer`: The response generated by the API.
 * @throws {Error} If there is an error during prompt creation or response generation.
 */
async function processSaving(input, context, mode) {
    try {
        return await processGeneration(input, context, "Saving", mode);

    } catch (error) {
        console.error("Error processing saving:", error);
        throw new Error("Error processing saving");
    }
}

async function processForm(inputs) {
    try {
        console.log("Satisfaction:", inputs.satisfaction);
        console.log("Relevance:", inputs.relevance);
        console.log("Additional Info:", inputs.additional_info);

        return;
        
    } catch (error) {
        console.error("Error processing form:", error);
        throw new Error("Error processing form");
    }
}

module.exports = {
    processQuestion,
    processExpand,
    processOffert,
    processGuide,
    processSaving,
    generateAndParseContent, 
    processGeneration, 
    processForm,
};
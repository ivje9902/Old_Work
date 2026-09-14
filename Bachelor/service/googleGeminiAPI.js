const { GoogleGenAI, createPartFromUri } = require("@google/genai");
require("dotenv").config();


const gemAI = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });

// Required model for Google Gemini
const model = "gemini-2.0-flash-lite";

/**
 * Uploads a file to Google Gemini.
 * 
 * @param {string} filePath - The path to the file to upload.
 * @param {string} mimeType - The MIME type of the file.
 * @returns {Promise<Object>} A promise that resolves to the uploaded file information.
 * @throws {Error} If the file upload fails.
 */
async function uploadFileToGemini(filePath, mimeType, ai=gemAI) { 
    try {
        const uploadedFile = await ai.files.upload({
            file: filePath,
            config: { mimeType },
        });
        return uploadedFile;
    } catch (error) {
        console.error("Error uploading file to Google Gemini:", error);
        throw error;
    }
}

/**
 * Generates content using the Google Gemini API.
 * 
 * @param {string} contents - The input content to process with the Gemini model.
 * @returns {Promise<string>} A promise that resolves to the generated content as a string.
 * @throws {Error} If content generation fails.
 */
async function generateContentWithGemini(contents, ai=gemAI) {
    try {
        const response = await ai.models.generateContent({
            model: model,
            contents,
        });
        return response.text;
    } catch (error) {
        console.error("Error generating content with Google Gemini:", error);
        throw error;
    }
}

module.exports = {
    uploadFileToGemini,
    generateContentWithGemini,
    createPartFromUri,
};
const fs = require('fs');

/**
 * Cleans a JSON string by removing unnecessary characters and formatting issues.
 * 
 * @param {string} jsonString - The JSON string to clean.
 * @returns {string} The cleaned JSON string.
 * @throws {Error} If the input string is empty or undefined.
 */
function cleanJsonString(jsonString) {
    if(!jsonString) {
        throw new Error("Input string is empty or undefined");
    }
    if (typeof jsonString !== 'string') {
        throw new Error("Input must be a string");
    }
    if (jsonString.trim() === '') {
        throw new Error("Input string is empty or undefined");
    }
    return jsonString
        .replace(/```json|```/g, '') // Remove code block markers
        .replace(/[\u0000-\u001F]+/g, '') // Remove control characters
        .trim(); // Trim whitespace
}


/**
 * Deletes a temporary file if it exists.
 * 
 * @param {string} filePath - The path to the file to delete.
 * @returns {void}
 */
function cleanUpFile(filePath) {
    if (fs.existsSync(filePath)) {
        fs.unlinkSync(filePath);
    }
  }

module.exports = {
    cleanJsonString,
    cleanUpFile,
};
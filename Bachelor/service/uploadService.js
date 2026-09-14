const { uploadFileToGemini, generateContentWithGemini, } = require("../service/googleGeminiAPI");
const { constructUploadPrompt } = require("../service/promptService");
const ExcelJS = require("exceljs");
const fs = require("fs");
const path = require("path");
const { retryApiCall } = require("../utils/retry");


/**
 * Validates the MIME type of a file to ensure it is an allowed type.
 * 
 * @param {string} mimetype - The MIME type of the file to validate.
 * @returns {boolean} `true` if the file type is valid, otherwise `false`.
 */
function isValidFileType(mimetype) {
    const allowedTypes = [
        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        "application/vnd.ms-excel",
        "application/pdf",
    ];
    return allowedTypes.includes(mimetype);
  }

/**
 * Converts an Excel file to a JSON object.
 * @async
 * @param {string} tempFilePath - The temporary file path of the Excel file to process.
 * @returns {Promise<Object[]>} A promise that resolves to an array of objects, 
 * each containing the sheet name and its data.
 * @throws {Error} If the file cannot be read or processed.
 */
async function excelToJson(tempFilePath) {
    const workbook = new ExcelJS.Workbook();
    await workbook.xlsx.readFile(tempFilePath);

    const allData = [];

    workbook.worksheets.forEach((worksheet) => {
        const sheetData = [];
        let headers = [];

        worksheet.eachRow((row, rowNumber) => {
            if (rowNumber === 1) {
                headers = row.values.slice(1);
            } else {
                const rowData = {};
                row.eachCell((cell, colNumber) => {
                    const header = headers[colNumber - 1] || `Column${colNumber}`;
                    rowData[header] = cell.value;
                });
                sheetData.push(rowData);
            }
        });

        allData.push({ sheetName: worksheet.name, data: sheetData });
    });

    return allData;
}

/**
 * Processes an Excel file by converting it to JSON and generating a prompt for the API.
 * @async
 * @param {string} tempFilePath - The temporary file path of the Excel file to process.
 * @param {string} mimetype - The MIME type of the file.
 * @param {string} originalname - The original name of the file.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `excelData`: The JSON representation of the Excel file.
 * - `apiResponse`: The response from the API after processing the prompt.
 * @throws {Error} If the file cannot be processed or the API call fails.
 */
async function processExcelFile(tempFilePath, mimetype, originalname) {
    console.log("Processing file:", tempFilePath);
    console.log("MIME type:", mimetype);
    const excelData = await excelToJson(tempFilePath); 
    const prompt = await constructUploadPrompt(mimetype, { data: excelData }, originalname);
    const apiResponse = await retryApiCall(() => generateContentWithGemini(prompt), "Processing Excel file");
    return { excelData, apiResponse };
  }
  
/**
 * Processes a PDF file by uploading it and generating a prompt for the API.
 * @async
 * @param {string} tempFilePath - The temporary file path of the PDF file to process.
 * @param {string} mimetype - The MIME type of the file.
 * @param {string} originalname - The original name of the file.
 * @returns {Promise<Object>} A promise that resolves to an object containing:
 * - `uploadedFile`: The uploaded file information.
 * - `apiResponse`: The response from the API after processing the prompt.
 * @throws {Error} If the file cannot be uploaded or the API call fails.
 */
async function processPdfFile(tempFilePath, mimetype, originalname) {
    const uploadedFile = await uploadFileToGemini(tempFilePath, mimetype);
    const prompt = await constructUploadPrompt(mimetype, uploadedFile, originalname);
    const apiResponse = await retryApiCall(() => generateContentWithGemini(prompt), "Processing PDF file");
    return { uploadedFile, apiResponse };
}


module.exports = {
    isValidFileType,
    excelToJson,
    processExcelFile,
    processPdfFile,
};


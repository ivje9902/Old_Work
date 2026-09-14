const uploadService = require('../../service/uploadService');
const geminiAPI = require('../../service/googleGeminiAPI');
const ExcelJS = require('exceljs');
const fs = require('fs');
const path = require('path');
const { createTestPdfFile, createTestXlsxFile } = require('../createTestFiles');

jest.mock('../../service/googleGeminiAPI');

describe('function isValidFileType', () => {
    test('should return true for valid file types', () => {
        expect(uploadService.isValidFileType('application/vnd.openxmlformats-officedocument.spreadsheetml.sheet')).toBe(true);
        expect(uploadService.isValidFileType('application/vnd.ms-excel')).toBe(true);
        expect(uploadService.isValidFileType('application/pdf')).toBe(true);
    });

    test('should return false for invalid file types', () => {
        expect(uploadService.isValidFileType('text/plain')).toBe(false);
        expect(uploadService.isValidFileType('image/jpeg')).toBe(false);
    });
});

describe('function excelToJson', () => {
    let testFilePath;

    beforeEach(async () => {
        testFilePath = await createTestXlsxFile('test1.xlsx');
    });

    afterEach(() => {
        fs.unlinkSync(testFilePath);
    });
    test('should convert Excel file to JSON', async () => {
        const result = await uploadService.excelToJson(testFilePath);;
        expect(result).toHaveLength(1);
        expect(result[0].sheetName).toBe('Test Sheet');
        expect(result[0].data).toHaveLength(2);
        expect(result[0].data[0]).toHaveProperty('Date');
        expect(result[0].data[0]).toHaveProperty('Value');
        expect(result[0].data[0]).toHaveProperty('Description');
        expect(result[0].data[1]).toHaveProperty('Date');
        expect(result[0].data[1]).toHaveProperty('Value');
        expect(result[0].data[1]).toHaveProperty('Description');
        expect(result[0].data[0]).toEqual({ Date: '2023-01-01', Value: 100, Description: 'Test entry 1' });
        expect(result[0].data[1]).toEqual({ Date: '2023-01-02', Value: 200, Description: 'Test entry 2' });
    });
    test('should handle empty Excel files', async () => {
        const emptyFilePath = path.join(__dirname, 'empty.xlsx');
        const workbook = new ExcelJS.Workbook();
        await workbook.xlsx.writeFile(emptyFilePath);
        const result = await uploadService.excelToJson(emptyFilePath);
        expect(result).toHaveLength(0);
        fs.unlinkSync(emptyFilePath);
    });
});

describe('function processExcelFile', () => {
    let testFilePath;

    beforeEach(async () => {
        testFilePath = await createTestXlsxFile('test.xlsx');
        jest.restoreAllMocks();
    });

    afterEach(() => {
        fs.unlinkSync(testFilePath);
        jest.clearAllMocks();
    });
    test('should process Excel file and return API response', async () => {
        const mockApiResponse = { data: 'Mock API response' };
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        expect(testFilePath).toBeDefined();
        const result = await uploadService.processExcelFile(testFilePath, 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet', 'test.xlsx');
        expect(result.excelData).toHaveLength(1);
        expect(result.excelData[0].sheetName).toBe('Test Sheet');
        expect(result.excelData[0].data).toHaveLength(2);
        expect(result.apiResponse).toEqual(mockApiResponse);
        expect(geminiAPI.generateContentWithGemini).toHaveBeenCalledTimes(1);
        expect(result.excelData[0].data[0]).toEqual({ Date: '2023-01-01', Value: 100, Description: 'Test entry 1' });
        expect(result.excelData[0].data[1]).toEqual({ Date: '2023-01-02', Value: 200, Description: 'Test entry 2' });
    });
});

describe('function processPdfFile', () => {
    let testFilePath;

    beforeEach(() => {
        testFilePath = createTestPdfFile('test.pdf');
        jest.restoreAllMocks();
    });

    afterEach(() => {
        fs.unlinkSync(testFilePath);
        jest.clearAllMocks();
    });
    test('should process PDF file and return API response', async () => {
        const mockUploadedFile = { data: 'Mocked file', uri: 'Mock-URI' };
        geminiAPI.uploadFileToGemini.mockResolvedValue(mockUploadedFile);
        const mockApiResponse = { 'data': 'Mock API response'};
        geminiAPI.generateContentWithGemini.mockResolvedValue(mockApiResponse);
        expect(testFilePath).toBeDefined();
        const result = await uploadService.processPdfFile(testFilePath, 'application/pdf', 'test.pdf');
        expect(result.uploadedFile).toEqual(mockUploadedFile);
        expect(result.apiResponse).toEqual(mockApiResponse);
    });
});

module.exports = {
    createTestPdfFile,
    createTestXlsxFile
}
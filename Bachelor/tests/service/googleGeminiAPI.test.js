const e = require('express');
const { uploadFileToGemini, generateContentWithGemini } = require('../../service/googleGeminiAPI');
const { GoogleGenAI } = require('@google/genai');

jest.mock('@google/genai');

describe('Google Gemini API', () => {
    let aiMock;

    beforeEach(() => {
        aiMock = {
            files: {
                upload: jest.fn(),
            },
            models: {
                generateContent: jest.fn(),
            },
        };
        GoogleGenAI.mockImplementation(() => aiMock);
    });

    afterEach(() => {
        jest.clearAllMocks();
    });

    afterAll(() => {
        GoogleGenAI.mockRestore();
    })

    test('uploadFileToGemini uploads a file successfully', async () => {
        const filePath = 'path/to/file';
        const mimeType = 'application/pdf';
        const uploadedFile = { id: 'file123', name: 'file.pdf' };

        aiMock.files.upload.mockResolvedValue(uploadedFile);

        const result = await uploadFileToGemini(filePath, mimeType, aiMock);

        expect(aiMock.files.upload).toHaveBeenCalledWith({
            file: filePath,
            config: { mimeType },
        });
        expect(aiMock.files.upload).toHaveBeenCalledTimes(1);
        expect(aiMock.models.generateContent).not.toHaveBeenCalled();
    });

    test('uploadFileToGemini handles errors', async () => {
        const filePath = 'path/to/file';
        const mimeType = 'application/pdf';
        const error = new Error('Mock upload failed');

        aiMock.files.upload.mockRejectedValue(error);

        await expect(uploadFileToGemini(filePath, mimeType, aiMock)).rejects.toThrow('Mock upload failed');
        expect(aiMock.files.upload).toHaveBeenCalledWith({
            file: filePath,
            config: { mimeType },
        });
        expect(aiMock.files.upload).toHaveBeenCalledTimes(1);
        expect(aiMock.models.generateContent).not.toHaveBeenCalled();
    });

    test('generateContentWithGemini generates content successfully', async () => {
        const contents = 'Some input text';
        const response = { text: 'Generated content' };

        aiMock.models.generateContent.mockResolvedValue(response);

        const result = await generateContentWithGemini(contents, aiMock);

        expect(aiMock.models.generateContent).toHaveBeenCalledWith({
            model: 'gemini-2.0-flash-lite',
            contents,
        });
        expect(result).toEqual(response.text);
        expect(aiMock.models.generateContent).toHaveBeenCalledTimes(1);
        expect(aiMock.files.upload).not.toHaveBeenCalled();
    });

    test('generateContentWithGemini handles errors', async () => {
        const contents = 'Some input text';
        const error = new Error('Mock generation failed');

        aiMock.models.generateContent.mockRejectedValue(error);

        await expect(generateContentWithGemini(contents, aiMock)).rejects.toThrow('Mock generation failed');
        expect(aiMock.models.generateContent).toHaveBeenCalledWith({
            model: 'gemini-2.0-flash-lite',
            contents,
        });
        expect(aiMock.models.generateContent).toHaveBeenCalledTimes(1);
        expect(aiMock.files.upload).not.toHaveBeenCalled();
    });
});

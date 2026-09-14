const cleanUp = require('../../utils/cleanUp');


describe('function cleanJsonString', () => {
    it('should clean the dirty JSON string', () => {
        const dirtyJsonString = "```json\n{\n  \"name\": \"Alice\",\u0000\n  \"age\": 30\n}\n```";
        const result = cleanUp.cleanJsonString(dirtyJsonString);
        expect(result).toBe('{  "name": "Alice",  "age": 30}');
    });

    it('should throw an error if input is empty or undefined', () => {
        expect(() => cleanUp.cleanJsonString()).toThrow("Input string is empty or undefined");
        expect(() => cleanUp.cleanJsonString("  ")).toThrow("Input string is empty or undefined");
    });
    it('should throw an error if input is not a string', () => {
        expect(() => cleanUp.cleanJsonString(123)).toThrow("Input must be a string");
        expect(() => cleanUp.cleanJsonString({})).toThrow("Input must be a string");
    });
});

describe('function cleanUpFile', () => {
    const fs = require('fs');
    const path = require('path');

    it('should delete the file if it exists', () => {
        const filePath = path.join(__dirname, 'testFile.txt');
        fs.writeFileSync(filePath, 'Test content');
        cleanUp.cleanUpFile(filePath);
        expect(fs.existsSync(filePath)).toBe(false);
    });

    it('should not throw an error if the file does not exist', () => {
        const filePath = path.join(__dirname, 'nonExistentFile.txt');
        expect(() => cleanUp.cleanUpFile(filePath)).not.toThrow();
    });
}
);
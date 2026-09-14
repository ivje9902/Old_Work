const promptService = require('../../service/promptService');
const searchDatabase = require('../../searchDatabase');
const fs = require('fs');
const path = require('path');

jest.mock('path', () => ({
    join: jest.fn(),
    resolve: jest.requireActual('path').resolve,
    dirname: jest.requireActual('path').dirname,
}));

beforeEach(() => {
    jest.clearAllMocks();
    path.join.mockReturnValue(path.resolve(__dirname, '../material/test-prompts.txt'));
});

afterEach(() => {
    jest.restoreAllMocks();
});

afterAll(() => {
    path.join.mockRestore();
})

describe('function getPromptById', () => {
    test('should return the correct prompt for a given ID', async () => {
        const promptId = 'Guide';
        const result = await promptService.getPromptById(promptId);
        expect(result).toMatch(`Generera en steg-för-steg-guide för att utföra följande åtgärder på en bostad som ges i slutet av denna text.
Svara på svenska och skriv med tydlig struktur:
Skriv inte "Hej" eller "Med vänlig hälsning" i början eller slutet av meddelandet.
Skriv inte ut kontexten i svaret, utan bara stegen.`);
    });
    test('should return the correct prompt for a given ID with different content', async () => {
        const promptID = 'Expand';
        const result = await promptService.getPromptById(promptID);
        expect(result).toContain(`Du är en expert på energieffektivisering och representerar Energikontoret Region Örebro län i Sverige.`);
        expect(result).toContain(`Utveckla följande punkter och returnera en giltig JSON-array där varje objekt har egenskaperna "title" och "description" (där "description" är den utvecklade texten för punkten):`);
        expect(result).toContain(`- För varje utveckling av punkt, skriv mellan 300-500 ord.`);
        expect(result).toContain(`- Använd inte ogiltiga tecken, radbrytningar eller tabbar inne i strängar.`);
        expect(result).toContain(`- Skriv inte ut JSON som en sträng, utan som en faktisk array.`);
        expect(result).toContain(`- Använd inte fetstil, alltså **text**.`);
    });

    test('should throw an error if the prompt is not found', async () => {
        const promptId = '999';
        await expect(promptService.getPromptById(promptId)).rejects.toThrow(`Prompt '${promptId}' not found or empty.`);
    });

    test('should throw an error if the file cannot be read', async () => {
        const promptId = '1';
        jest.spyOn(fs.promises, 'readFile').mockRejectedValue(new Error('File not found'));
        await expect(promptService.getPromptById(promptId)).rejects.toThrow(`Failed to load prompt '${promptId}': File not found`);
    });
});

describe('function makePrompt', () => {
    let expectedPromptGuide;
    let expectedPromptExpand;
    beforeAll(async () => {
        expectedPromptGuide = await promptService.getPromptById("Guide");
        expectedPromptExpand = await promptService.getPromptById("Expand");
    });
    
    test('should return the correct prompt with input and context', async () => {
        const ID = 'Guide';
        const input = 'Test input';
        const context = 'Test context';
        const result = await promptService.makePrompt(ID, input, context);
        expect(result[0]).toContain(expectedPromptGuide);
        expect(result[0]).toContain(`Åtgärder:`);
        expect(result[0]).toContain(input);
        expect(result[0]).toContain(`Här är kontext gällande bostaden`);
        expect(result[0]).toContain(context);
    });
    test('should throw an error if the prompt is not found', async () => {
        const ID = 999;
        const input = 'Test input'
        const context = 'Test context';
        await expect(promptService.makePrompt(ID, input, context)).rejects.toThrow(`Failed to load prompt '${ID}': Prompt '${ID}' not found or empty.`);
    });
});

describe('function makePromptQuestion', () => {
    let questionPrompt;
    beforeAll(async () => {
        questionPrompt = await promptService.getPromptById("Question");
    });
    test('should return the correct prompt with input and context', async () => {
        const buildingInfo = { 'building': 'info'};
        const result = await promptService.makePromptQuestion(buildingInfo);
        expect(result[0]).toContain(questionPrompt);
        expect(result[0]).toContain(JSON.stringify(buildingInfo));
    });
});

describe('function makePromptExpand', () => {
    let expandPrompt;
    beforeAll(async () => {
        expandPrompt = await promptService.getPromptById("Expand");
    });

    test('should return the correct prompt with input and context', async () => {
        const input = {'Test': 'input'};
        const context = 'Test context';
        const result = await promptService.makePromptExpand(context, input);
        expect(result[0]).toContain(context);
        expect(result[0]).toContain(JSON.stringify(input));
    });
})

describe('function makePromptSaving', () => {
    test('should return the prompt with input and context', () => {
        const input = 'Test input';
        const context = 'Test context';
        const result = promptService.makePromptSaving(input, context);
        expect(result[0]).toContain(input);
        expect(result[0]).toContain(context);
    });
});

jest.mock('../../searchDatabase');

describe('function addRAGContext', () => {
    afterEach(() => {
        jest.clearAllMocks();
    });
    test('should return a string containing both the promptMessage and the added context with the input', async () => {
        const promptMessage = 'Test prompt message';
        const input = { 'Test': 'input'};
        const mockedSearchResults = [
            {
              id: 'test-id-1',
              text: 'test text 1',
              metadata: {
                source: 'test-source-1.pdf',
                pdf: 'test pdf metadata 1',
                url: 'test url 1',
                date_accessed: '2000-01-01',
                loc: 'test loc 1'
              },
              similarity: 0.99
            }, 
            {
                id: 'test-id-2',
                text: 'test text 2',
                metadata: {
                  source: 'test-source-2.pdf',
                  pdf: 'test pdf metadata 2',
                  url: 'test url 2',
                  date_accessed: '2000-01-01',
                  loc: 'test loc 2'
                },
                similarity: 0.01
              }
        ];
        searchDatabase.searchDatabase.mockResolvedValue(mockedSearchResults);
        const result = await promptService.addRAGContext(promptMessage, input);
        console.log(result);
        expect(result[0]).toContain(promptMessage);
        expect(result[0]).toContain(mockedSearchResults[0].text);
        expect(result[0]).toContain(mockedSearchResults[0].id);
        expect(result[0]).toContain(mockedSearchResults[0].metadata.source);

    });
});

describe('function constructUploadPrompt', () => {
    const filetypes = {
        'sheet': "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        'excel': "application/vnd.ms-excel",
        'pdf'  : "application/pdf"
    };
    const fileData = { 'data': 'test data', 'uri': 'test uri' };

    test('should return the prompt for a pdf', async () => {
        const result = await promptService.constructUploadPrompt(filetypes.pdf, fileData, 'test.pdf');
        expect(result).toBeDefined();
        expect(result).toContain('Analysera följande PDF fil: test.pdf');
        expect(result).toContain('Kan du analysera innehållet i PDF-filen utifrån ett energieffektiviseringsperspektiv? Börja med att skriva - Här är en analys av pdf-filen: Skriv ingen hälsningsfras och skriv inte information om vad du tänker analysera');
    });

    test('should return the prompt for an excel document', async () => {
        const result = await promptService.constructUploadPrompt(filetypes.excel, fileData, 'test.excel');
        expect(result).toBeDefined();
        expect(result).toContain(`Här är en Excelfil med timvis förbrukningsdata: test.excel`);
        expect(result).toContain(JSON.stringify(fileData.data));
    });

    test('should return the prompt for a sheet', async () => {
        const result = await promptService.constructUploadPrompt(filetypes.sheet, fileData, 'test.sheet');
        expect(result).toBeDefined();
        expect(result).toContain(`Här är en Excelfil med timvis förbrukningsdata: test.sheet`);
        expect(result).toContain(JSON.stringify(fileData.data));
    });

    test('should throw an error if filetype is unsupported', async () => {
        await expect(promptService.constructUploadPrompt('image/jpeg', fileData, 'test.jpeg'))
            .rejects.toThrow("Unsupported file type");
    });
});
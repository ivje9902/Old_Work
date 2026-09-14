const ExcelJS = require('exceljs');
const fs = require('fs');
const path = require('path');

function createTestXlsxFile(filename){
    const workbook = new ExcelJS.Workbook();
    const worksheet = workbook.addWorksheet('Test Sheet');
    worksheet.columns = [
        { header: 'Date', key: 'date' },
        { header: 'Value', key: 'value' },
        { header: 'Description', key: 'description' },
    ];
    worksheet.addRow({ date: '2023-01-01', value: 100, description: 'Test entry 1' });
    worksheet.addRow({ date: '2023-01-02', value: 200, description: 'Test entry 2' });
    const filePath = path.join(__dirname, filename);
    return workbook.xlsx.writeFile(filePath).then(() => filePath);
}
function createTestPdfFile(filename) {
    const filePath = path.join(__dirname, filename);
    fs.writeFileSync(filePath, 'This is a test PDF file.');
    return filePath;
}

module.exports = {
    createTestPdfFile,
    createTestXlsxFile
}
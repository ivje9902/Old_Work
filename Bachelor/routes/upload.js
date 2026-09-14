const express = require("express");
const router = express.Router();
const path = require("path");
const multer = require("multer");
const { isValidFileType } = require("../service/uploadService");
const { cleanUpFile } = require("../utils/cleanUp");
const { processExcelFile, processPdfFile } = require("../service/uploadService");


/**
 * GET /upload
 * Renders upload page.
 * @param {Object} req - The request object.
 * @param {Object} res - The response object.
 * @param {Function} next - The next middleware function.
 * @returns {void}
 */
router.get('/', function (req, res, next) {
  res.render('upload', { title: 'Express' });
});

module.exports = router;

// Multer configuration for file upload
const upload = multer({
    dest: path.join(__dirname, "uploads"),
    limits: { fileSize: 10 * 1024 * 1024 }, // 10 MB
});

/**
 * POST /upload-file
 * Handles file upload and processing.
 * @param {Object} req - The request object.
 * @param {Object} res - The response object.
 * @returns {void}
 * @throws {Error} If there is an error uploading or processing file.
 */
router.post("/upload-file", upload.single("file"), async (req, res) => {
  const { file } = req;

  if (!file) {
    return res.status(400).json({ error: "Ingen fil har laddats upp" });
  }
  const { mimetype, path: tempFilePath, originalname } = file;
  
  if (!isValidFileType(mimetype)) {
    cleanUpFile(tempFilePath);
    return res.status(400).json({ error: "Ogiltig filtyp. Endast Excel- och PDF-filer är tillåtna." });
  }

  try {
      let responseData;

      if (mimetype === "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" || mimetype === "application/vnd.ms-excel") {
          const { excelData, apiResponse } = await processExcelFile(tempFilePath, mimetype, originalname);
          responseData = { message: "Excel file processed and analyzed", data: excelData, apiResponse };
      } else if (mimetype === "application/pdf") {
          const { uploadedFile, apiResponse } = await processPdfFile(tempFilePath, mimetype, originalname);
          responseData = { message: "PDF file uploaded and analyzed", file: uploadedFile, apiResponse };
      }

      cleanUpFile(tempFilePath);
      res.status(200).json(responseData);
  } catch (error) {
      console.error("Error uploading or processing file:", error);
      cleanUpFile(tempFilePath);
      res.status(500).json({ error: "Error uploading or processing file" });
  }
});

module.exports = router;
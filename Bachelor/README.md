# AI-Powered Energy Efficiency Recommendations Tool for Property Owners

## Overview

This Node.js-based web application provides property-specific energy-saving suggestions using AI. It supports both standard LLM prompting and Retrieval-Augmented Generation (RAG) via a vector database. The frontend is a user-friendly form system and document uploader; the backend handles API communication, vector similarity search, and AI response formatting.

Developed for handover to Region Örebro County's IT Department.

---

## Table of Contents

- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Documentation (JSDoc)](#documentation-jsdoc)
- [Prompts](#prompts)
- [Inserting Documents To Database](#inserting-documents-in-database)
- [API Services Used](#api-services-used)
- [Testing](#testing)
- [Maintenance & Handover Notes](#maintenance--handover-notes)
- [Contact](#contact)

---

## System Requirements

- Node.js 18.x or newer
- npm 9.x or newer
- Microsoft Azure account (for RAG model since we use Cosmos DB)
- Google Cloud project (for Gemini API access)

---

## Installation

To install and prepare the application locally:

### 1. Prerequisites

- **Node.js** (version 18.x or higher is recommended)
- **npm**
---

### 2. Install Dependencies

From the root directory of the project (where `package.json` is located), run:

```bash
npm install
```

This will install all necessary backend, frontend, and development packages.

---

### 3. Environment Setup

We used a `.env` file in the root of the project and added all required environment variables.  
This is not a required solution.

```env
GOOGLE_API_KEY=your_google_api_key
COSMOS_DB_URI=your_cosmos_connection_string
COSMOS_DB_NAME=your_database_name
```

Add a PORT:<number> to specify port.

---

### 4. Tailwind CSS Build

To generate the required CSS from Tailwind:

```bash
npm run build
```

If you're actively developing and want Tailwind to rebuild on file changes (optional):

```bash
npm run watch:css
```

---

### 5. Create database container

Set required environment variables:
COSMOS_DB_URI=your_cosmos_connection_string
COSMOS_DB_NAME=your_database_name

# Run the script:
```bash
node createContainer.js
```

---

### 6. Start the Application

To launch the server in development mode:

```bash
npm start
```

This starts the Express.js backend and watches for file changes automatically.

---

## Project Structure

Below is an overview of the folder and file layout, including key responsibilities of each component:

```
/AI_CHATBOT
│
├── bin/                            # Application entry point (used by npm start)
│   └── www                         # Initializes the server
│
├── public/                         # Frontend assets
│   ├── images/                     # Images used in frontend
│   ├── javascripts/               #
│   │   ├── dropdown.js            # Filters and updates dropdowns dynamically on the index page
│   │   ├── eval.js                # Manages expert evaluation form behavior and validation
│   │   ├── generator.js           # Handles button actions for generating guides, savings, and offers
│   │   ├── inputPage.js           # Controls field activation and input restoration on the main form
│   │   ├── response.js            # Manages interaction and expansion logic on the response page
│   │   ├── results.js             # Renders AI analysis results after file upload
│   │   ├── script.js              # Shared helpers for forms, animations, and UI reset
│   │   └── upload.js              # Handles file selection and upload interactions
│   └── stylesheets/
│       └── style.css              # Tailwind source CSS (input)
│       └── output.css             # Generated CSS (output from Tailwind)
│
├── routes/
│   ├── comparison.js     # Loads stored AI results for a file and renders the comparison view (with and without RAG)
│   ├── evaluation.js     # Renders the expert evaluation form and saves submitted evaluations to file
│   ├── index.js          # Redirects root route '/' to the home form page
│   ├── response.js       # Handles prompt construction, sends request to Gemini, saves results, and renders the response view
│   ├── style.js          # Serves CSS styling for views (added as a route-based asset)
│   ├── upload.js         # Handles file upload, PDF parsing, document chunking, embedding, and saving to database
│
├── service/
│   ├── googleGeminiAPI.js     # Sends prompts to the Gemini API and returns generated responses; used for generating tips and follow-up actions
│   ├── processPosts.js        # Parses POST form data into structured input (actor, building type, energy system, etc.)
│   ├── promptService.js       # Builds prompt templates based on input type (tips, guide, saving, offer); handles prompt logic
│   └── uploadService.js       # Processes uploaded files, extracts text and metadata, checks for duplicates, and inserts into the database
│
├── views/
│   ├── comparison.ejs     # Displays side-by-side comparison of AI responses with and without RAG; includes tip selection and guide/download options
│   ├── error.ejs          # Fallback error page showing error message, status, and stack trace
│   ├── evaluation.ejs     # Expert evaluation form for actor type, building info, energy use, and satisfaction rating
│   ├── index.ejs          # Main entry view with form UI for selecting building type and entering property details
│   ├── response.ejs       # Displays AI-generated tips and offers further actions (expand, guide, saving, offer); includes feedback form
│   ├── style.ejs          # Standalone results page for detailed tips; supports LaTeX rendering and PDF export
│   └── upload.ejs         # Upload interface for PDF/Excel files; parsed results displayed live after upload
|
├── tests/
│   ├── material/                          # Test documents and prompts
│   │   ├── beteendeforandring.txt         # Swedish sample input for testing behavior change prompts
│   │   ├── loremTest.txt                  # Generic lorem ipsum test content
│   │   ├── pdf_for_testing.pdf            # Sample PDF used for upload and PDF parsing tests
│   │   └── test-prompts.txt               # Prompt samples for use in prompt-related tests
│
│   ├── routes/
│   │   ├── response.test.js               # Tests the /response route for correct prompt handling and output
│   │   └── upload.test.js                 # Tests file upload route and processing behavior
│
│   ├── service/
│   │   ├── googleGeminiAPI.test.js        # Mocks Gemini calls and tests prompt/response interactions
│   │   ├── processPosts.test.js           # Unit tests for form data parsing logic
│   │   ├── promptService.test.js          # Tests prompt creation and formatting for all supported types
│   │   ├── test-prompts.txt               # Fixture used during prompt service testing
│   │   └── uploadService.test.js          # Tests document parsing, deduplication, and database insertion
│
│   ├── utils/
│   │   ├── cleanUp.js                     # Utility to clean test output or remove inserted DB records
│   │   ├── retry.test.js                  # Unit test for retry logic (e.g., exponential backoff)
│   │   └── createTestFiles.js             # Script to auto-generate test input files
│
│   └── searchDatabase.test.js             # Tests for semantic search queries against the vector DB
|
├── app.js                          # Sets up the Express app, view engine, middleware, and routes
├── createContainer.js              # Creates and configures the Cosmos DB container with vector indexing
├── dummyData.js                    # Contains mock data for local testing and development
├── embeddingHandle.js              # Generates vector embeddings using Gemini API
├── insertDatabase.js               # Loads PDFs, chunks content, embeds, and inserts into Cosmos DB
├── pdfToTxt.py                     # Converts PDF files to plain text using PyPDF2
├── prompts.txt                     # Prompt templates used with the LLM (e.g., tips, guides, savings)
├── searchDatabase.js               # Runs vector similarity search queries on the Cosmos DB container
└── tailwind.config.js              # TailwindCSS setup for compiling frontend styles

```

> **Tip:** Routes call into services (like `googleGeminiAPI.js`) and render EJS views. Cosmos DB and Gemini API are used behind the scenes for energy tip generation and retrieval.

---

## Documentation (JSDoc)

JSDoc documentation is pre-generated and included in the `/docs` folder.

To view it:

1. Clone or download the repository
2. Open the file `docs/index.html` in your web browser

> No installation or build is needed to view the documentation.

If updates are needed, run the following to regenerate the documentation:

```bash
npm run docs
```

## Prompts

Prompts are stored in "prompts.txt".
The program reads from this txt file so any changes there will be changes made to the program.
Each prompt starts with "Prompt<ID>Start" and ends with "Prompts<ID>End".
All text between these 2 lines are included in the prompt.
Any text outside of these lines will not be included in any prompts.

---

## Inserting Documents into the Database

Processes `.pdf` files from `material/data/`, generates embeddings using Gemini, and inserts them into the `chunks` container in Cosmos DB.

.env must include:
COSMOS_ENDPOINT=...
COSMOS_KEY=...
GOOGLE_API_KEY=...

# Run:
node insertDatabase.js

- Only processes `.pdf` files in `material/data/`
- Skips files already in the database (based on file path)

---

## API Services Used

# Google Gemini API
Used to generate text responses, including energy-saving tips, expanded explanations, guides, savings calculations, and offer requests.

# Azure Cosmos DB (Vector Indexing)
Used as a vector-enabled NoSQL database to store embedded document chunks and perform semantic search in the RAG pipeline.

---

### Testing

The project includes unit and integration tests covering services, routes, and utility logic. Tests are written using Jest and structured under the `/tests` directory.

---

## 1. Run All Tests
```bash
npm test
```

This runs all test files using Jest and outputs coverage information.

## Notes

- Mocks are used where external services (Gemini, Cosmos DB) are involved.
- Test prompts and input data are stored in `tests/material/`.

---

## Maintenance & Handover Notes

This project should not require any maintenance.

Make sure the current API is in use always and changing the Gemini model (e.g., from gemini-pro to another) can be done by updating the model name in googleGeminiAPI.js.
The Google API integration uses official client libraries. Any issues or changes should be referred to the official Google Cloud documentation.

If RAG version wants to be implemented, we would first recommend getting a proper database with extensive information,
and in the code to switch from non-RAG to rag you have to change a variable in script.js called "window.currentMode" from no-RAG to with-RAG.
We also have a "both" mode supporting both versions at the same time. Was only made for testing purposes.

## Contact
Abdulfattah Morad abdulfattah.morad.4486@student.uu.se

Adar Dönmez adar.donmez.9517@student.uu.se

Henrik Langlott henrik.langlott.4581@student.uu.se

Ivar Jensen ivar.jensen.9902@student.uu.se

Jacob Klintskär jacob.klintskar.5613@student.uu.se

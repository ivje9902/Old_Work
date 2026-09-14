var express = require('express');
var router = express.Router();
const { processQuestion,
    processExpand,
    processOffert,
    processGuide,
    processSaving,
    processForm } = require('../service/processPosts');

/**
 * GET /response
 * 
 * Renders the response page.
 * 
 * @function
 * @name getResponse
 * @param {Object} req - The Express request object.
 * @param {Object} res - The Express response object.
 * @param {Function} next - The next middleware function.
 * @returns {void}
 */
router.get('/', function (req, res, next) {
    res.render('response', { title: 'Express' });
});

/**
 * POST /question
 * Handles a question submission and returns tips based on the provided mode.
 *
 * @param {Object} req - The request object.
 * @param {Object} req.body.inputs - The input data.
 * @param {string} [req.body.inputs.mode] - Processing mode: "no-RAG", "with-RAG", or "both".
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with `tips`, `tipsRAG`, or both, depending on mode.
 * @returns {Object} 400 - Bad request if `inputs` is missing or mode is invalid.
 * @returns {Object} 500 - Internal server error if something goes wrong during processing.
 */

router.post('/question', async (req, res) => {
    try {
        console.log("Question post received");
        if (!req.body.inputs) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        console.log("Request is being processed...");
        const inputs = req.body.inputs;

        const { tips, tipsRAG } = await processQuestion(inputs);

        console.log("Request processed successfully");

        const mode = inputs.mode || "no-RAG";
        // Adjust the response based on the mode
        if (mode === "both") {
            return res.status(200).json({ tips, tipsRAG, mode });
        } else if (mode === "no-RAG") {
            return res.status(200).json({ tips, mode });
        } else if (inputs.mode === "with-RAG") {
            return res.status(200).json({ tipsRAG, mode });
        } else {
            return res.status(400).json({ error: "Invalid mode" });
        }

    } catch (error) {
        console.error('Error handling question:', error);
        return res.status(500).json({ error: 'Error generating response' });
    }
});

/**
 * POST /expand
 * Processes expansion input using different input types and returns expanded results.
 *
 * @param {Object} req - The request object.
 * req.body.inputs should contain: { input?: string | string[], ansInput?: string | string[], ragInput?: string | string[], context?: string, mode?: string }
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with `expanded` and `expandedRAG` results.
 * @returns {Object} 400 - Bad request if required inputs are missing or invalid.
 * @returns {Object} 500 - Internal server error if processing fails.
 */

router.post('/expand', async (req, res) => {
    try {
        console.log("Expand post received");
        console.log("Request body:", req.body.inputs);
        if (!req.body.inputs.input && (req.body.inputs.ansInput == undefined && req.body.inputs.ragInput == undefined)) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        // TODO: change name of input 
        const context = req.body.inputs.context || "";
        const input = req.body.inputs.input || [];
        const mode = req.body.inputs.mode || "no-RAG";

        console.log("context:", context);

        const noRAGInput = req.body.inputs.ansInput || [];
        const RAGInput = req.body.inputs.ragInput || [];

        console.log("Request is being processed...");
        const { expanded, expandedRAG } = await processExpand(input, noRAGInput, RAGInput, context, mode);
        console.log("Expanded:", expanded);

        console.log("Request processed successfully");
        return res.status(200).json({ expanded, expandedRAG })

    } catch (error) {
        console.error("Problem with generation", error);
        return res.status(500).json({ error: 'Error generating response' });

    }
});

/**
 * Handles a generation request using a provided processing function.
 *
 * @param {Object} req - The request object.
 * req.body.inputs should contain: { input?: string | string[], context?: string, mode?: string }
 * @param {Object} res - The response object.
 * @param {Function} processFunction - Async function that processes the input and returns { answer }.
 * @param {string} processType - Label for logging the type of processing being done.
 *
 * @returns {Object} 200 - Success response with `answer`.
 * @returns {Object} 400 - Bad request if `inputs` are missing.
 * @returns {Object} 500 - Internal server error if processing fails.
 */

async function handleGeneration(req, res, processFunction, processType) {
    try {
        console.log(`${processType} post received`);
        if (!req.body.inputs) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        const context = req.body.inputs.context || "";
        const input = req.body.inputs.input || [];
        const mode = req.body.inputs.mode || "no-RAG";

        console.log("Request is being processed...");
        const { answer } = await processFunction(input, context, mode);

        console.log("Request processed successfully");
        return res.status(200).json({ answer: answer });

    } catch (error) {
        console.error("Problem with generation", error);
        return res.status(500).json({ error: 'Error generating response' });

    }
}

/**
 * POST /offert
 * Delegates offert generation to the shared handleGeneration function.
 *
 * @param {Object} req - The request object.
 * req.body.inputs should contain: { input?: string | string[], context?: string, mode?: string }
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with `answer`.
 * @returns {Object} 400 - Bad request if required inputs are missing.
 * @returns {Object} 500 - Internal server error if generation fails.
 */

router.post('/offert', async (req, res) => {
    try {
        return handleGeneration(req, res, processOffert, "offert");

    } catch (error) {
        console.error("Problem with generating Offert", error);
        return res.status(500).json({ error: 'Error when generating Offert' });
    }
});


/**
 * POST /guide
 * Delegates guide generation to the shared handleGeneration function.
 *
 * @param {Object} req - The request object.
 * req.body.inputs should contain: { input?: string | string[], context?: string, mode?: string }
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with `answer`.
 * @returns {Object} 400 - Bad request if required inputs are missing.
 * @returns {Object} 500 - Internal server error if generation fails.
 */

router.post('/guide', async (req, res) => {
    try {
        return handleGeneration(req, res, processGuide, "guide");

    } catch (error) {
        console.error("Problem with generating Guide", error);
        return res.status(500).json({ error: 'Error when generating Guide' });
    }

});

/**
 * POST /saving
 * Delegates saving generation to the shared handleGeneration function.
 *
 * @param {Object} req - The request object.
 * req.body.inputs should contain: { input?: string | string[], context?: string, mode?: string }
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with `answer`.
 * @returns {Object} 400 - Bad request if required inputs are missing.
 * @returns {Object} 500 - Internal server error if generation fails.
 */

router.post('/saving', async (req, res) => {
    try {
        return handleGeneration(req, res, processSaving, "saving");

    } catch (error) {
        console.error("Problem with generating Saving", error);
        return res.status(500).json({ error: 'Error when generating Saving' });
    }
});

/**
 * POST /form
 * Handles user feedback form submission and processes satisfaction, relevance, and additional info.
 *
 * @param {Object} req - The request object.
 * req.body should contain: { satisfaction: number, relevance: number, additional_info?: string }
 * @param {Object} res - The response object.
 *
 * @returns {Object} 200 - Success response with a confirmation message.
 * @returns {Object} 500 - Internal server error if form processing fails.
 */

router.post('/form', async (req, res) => {
    try {
        console.log("Form post received");

        const { satisfaction, relevance, additional_info } = req.body;

        const inputs = {
            satisfaction,
            relevance,
            additional_info
        };

        await processForm(inputs);

        return res.status(200).json({ message: 'Form submitted successfully' });
    } catch (error) {
        console.error("Problem with form", error);
        return res.status(500).json({ error: 'Error with form' });
    }
});


module.exports = router;



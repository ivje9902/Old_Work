var express = require('express');
var router = express.Router();

/**
 * GET /style
 * 
 * Handles requests to render the style page.
 * 
 * @function
 * @name getStyle
 * @param {Object} req - The Express request object.
 * @param {Object} res - The Express response object.
 * @param {Function} next - The next middleware function.
 * @returns {void} Renders the style page.
 */
router.get('/', function (req, res, next) {
    res.render('style', { title: 'Style' });
});



module.exports = router;



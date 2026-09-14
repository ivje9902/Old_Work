var express = require('express');
var router = express.Router();

/**
 * GET /comparison
 * 
 * Handles requests to render the comparison page.
 * 
 * @function
 * @name getComparison
 * @param {Object} req - The Express request object.
 * @param {Object} res - The Express response object.
 * @param {Function} next - The next middleware function.
 * @returns {void}
 */
router.get('/', function(req, res, next) {
  res.render('comparison', { title: 'Express' });
});

module.exports = router;



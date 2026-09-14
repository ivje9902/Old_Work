var express = require('express');
var router = express.Router();

/**
 * GET /evaluation
 * Renders evaluation page.
 * @param {Object} req - The request object.
 * @param {Object} res - The response object.
 * @param {Function} next - The next middleware function.
 * @returns {void}
 */
router.get('/', function(req, res, next) {
  res.render('evaluation', { title: 'Express' });
});



module.exports = router;



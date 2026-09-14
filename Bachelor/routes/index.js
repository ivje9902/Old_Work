var express = require('express');
var router = express.Router();

/**
 * GET /index
 * Renders index page.
 * @param {Object} req - The request object.
 * @param {Object} res - The response object.
 * @param {Function} next - The next middleware function.
 * @returns {void}
 */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});



module.exports = router;




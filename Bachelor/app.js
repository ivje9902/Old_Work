/**
 * The main application file for the AI chatbot.
 * 
 * - Sets up the Express application.
 * - Configures middleware, routes, and error handling.
 * - Exports the configured Express app for use in the server.
 * 
 * @module app
 */

var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var responseRouter = require('./routes/response');
var uploadRouter = require('./routes/upload');
var evaluationRouter = require('./routes/evaluation');
var comparisonRouter = require('./routes/comparison');
var styleRouter = require('./routes/style');


var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(logger('dev'));

app.use(express.json({ limit: '15mb' }));
app.use(express.urlencoded({ limit: '15mb', extended: true }));

app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/response', responseRouter);
app.use('/upload', uploadRouter);
app.use('/evaluation', evaluationRouter);
app.use('/comparison', comparisonRouter);
app.use('/style', styleRouter);


// catch 404 and forward to error handler
app.use(function (req, res, next) {
  next(createError(404));
});

// error handler
app.use(function (err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;

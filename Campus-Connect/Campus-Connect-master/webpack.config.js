const path = require('path')

module.exports = {
    mode: 'development',
    entry: {
        bundle: ['./src/frontend.js', './src/backend.js', './src/quiz.js']
    },
    output: {
        path: path.resolve(__dirname, 'public'),
        filename: 'bundle.js'
    },
    watch: true
};
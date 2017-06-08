process.env.PATH = "C:\\local\\cntk_2\\cntk;" + process.env.PATH;

const path = require('path')
const cntk = require('../.');

modelPath = path.join(__dirname, 'models', 'mnist.cmf')
console.info('Trying to load model at:', modelPath)

cntk.loadModel(modelPath, (err, model) => {
    if (err) {
        consoel.info('Got error:', err);
        return;
    }
    console.info('Got model!', model)
});
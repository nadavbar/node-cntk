process.env.PATH = "C:\\local\\cntk_2\\cntk;" + process.env.PATH;

const path = require('path')
const cntk = require('../.');

modelPath = path.join(__dirname, 'model', 'mnist.cmf')

console.info("CNTK module:", cntk);

console.info("Try setting device to GPU");
try {
    cntk.setDefaultDeviceSync(cntk.CNTKDevices.GPU);
}
catch(ex) {
    console.info("Failed setting GPU device, setting CPU instead");
    cntk.setDefaultDeviceSync(CNTKDevices.CPU);
}

console.info('Trying to load model at:', modelPath)
//modelPath = "C:/code/node-cntk/test/model/mnist.cmf"

cntk.loadModel(modelPath, (err, model) => {
    if (err) {
        console.info('Got error:', err);
        return;
    }
    console.info('Got model!', model)
});
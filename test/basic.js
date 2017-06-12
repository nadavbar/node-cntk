process.env.PATH = "C:\\local\\cntk_2\\cntk;" + process.env.PATH;

const pixel = require('pixel');
const path = require('path');
const cntk = require('../.');

modelPath = path.join(__dirname, 'mnist', 'mnist_conv.cmf');
testImagePath = path.join(__dirname, 'mnist', 'sample_4.bmp');

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

    console.info('Loading img data for:', testImagePath)
   
    pixel.parse(testImagePath).then(function(images){
        img = images[0];
        // we only care about one channel since all three channels have the same values
        var inputDataArr = new Uint8ClampedArray(img.width * img.height);
        
        for (var i =0; i <  inputDataArr.length; i++) {
            inputDataArr[i] = img.data[i*4]
        }

        inputData = {
            'input' : [inputDataArr]
        }

        outputNodes = ['output']
        model.eval(inputData, outputNodes, (err, res)=>{
            if (err) {
                console.info(err);
                return;
            }
            console.info('Eval result:', res);
        })
    });

});
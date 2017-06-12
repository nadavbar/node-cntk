process.env.PATH = "C:\\local\\cntk_2\\cntk;" + process.env.PATH;

const pixel = require('pixel');
const path = require('path');
const cntk = require('../.');

modelPath = path.join(__dirname, 'mnist', 'mnist_conv.cmf');
testImagePath1 = path.join(__dirname, 'mnist', 'sample_4.bmp');
testImagePath2 = path.join(__dirname, 'mnist', 'sample_9.bmp');

console.info("CNTK module:", cntk);

console.info("Try setting device to GPU");
try {
    cntk.setDefaultDeviceSync(cntk.CNTKDevices.GPU);
}
catch(ex) {
    console.info("Failed setting GPU device, setting CPU instead");
    cntk.setDefaultDeviceSync(CNTKDevices.CPU);
}

function rgbToOneChannel(img) {
    var inputDataArr = new Uint8ClampedArray(img.width * img.height);
        
    for (var i =0; i <  inputDataArr.length; i++) {
        inputDataArr[i] = img.data[i*4]
    }

    return inputDataArr;
}

console.info('Trying to load model at:', modelPath)
//modelPath = "C:/code/node-cntk/test/model/mnist.cmf"

cntk.loadModel(modelPath, (err, model) => {
    if (err) {
        console.info('Got error:', err);
        return;
    }
    console.info('Got model!', model)

    console.info('Loading images data')
   
    pixel.parse(testImagePath1).then(function(images1){
        img1 = images1[0];
        pixel.parse(testImagePath2).then(function(images2) {
            img2 = images2[0];
            inputData = {
                'input' : [rgbToOneChannel(img1), rgbToOneChannel(img2) ]
            }

            outputNodes = ['output']
            console.info('Calling eval')
            model.eval(inputData, outputNodes, (err, res)=>{
                if (err) {
                    console.info(err);
                    return;
                }
                console.info('Eval result:', res);
            })
        });
    });

});
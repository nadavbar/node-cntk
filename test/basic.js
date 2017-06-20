const pixel = require('pixel');
const path = require('path');
const cntk = require('../.');

var modelPath = path.join(__dirname, 'mnist', 'mnist_conv.cmf');
var testImagePath1 = path.join(__dirname, 'mnist', 'sample_4.bmp');
var testImagePath2 = path.join(__dirname, 'mnist', 'sample_9.bmp');

console.info("CNTK module:", cntk);

console.info("Try setting device to GPU");
try {
    cntk.setDefaultDeviceSync(cntk.devices.gpu);
}
catch(ex) {
    console.info("Failed setting GPU device, setting CPU instead");
    cntk.setDefaultDeviceSync(cntk.devices.cpu);
}

function rgbToOneChannel(img) {
    var inputDataArr = new Uint8ClampedArray(img.width * img.height);
        
    for (var i =0; i <  inputDataArr.length; i++) {
        inputDataArr[i] = img.data[i*4]
    }

    return inputDataArr;
}

console.info('Trying to load model at:', modelPath)

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

            // inputs data can be an object with variable names
            // inputData = {
            //   'input' : [rgbToOneChannel(img1), rgbToOneChannel(img2) ]
            // }
            
            // this also works
            // inputData = [[rgbToOneChannel(img1), rgbToOneChannel(img2)]]
            
            // and this works as well:
            inputData = [rgbToOneChannel(img1), rgbToOneChannel(img2)]

            // you can optionally specify output nodes that you are interested in
            // outputNodes = ['output']
            console.info('Calling eval')
            model.eval(inputData, /*outputNodes,*/ (err, res)=>{
                if (err) {
                    console.info(err);
                    return;
                }
                console.info('Eval result:', res);

                // The result of the evaluation is a one-hot vector
                // so we use argmax to get the index of the highest value
                console.info('Classification result (maximum indices):', cntk.utils.argmax(res.output));
            })
        });
    });
});
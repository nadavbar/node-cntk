const bodyParser = require('body-parser');
const express = require('express');
const path = require('path');
const app = express();
const cntk = require('../../.');

const modelPath = path.join(__dirname, '../models/mnist_conv.cmf')
var model = null;

app.use(express.static('public'))

app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

// POST http://localhost:8080/api/users
// parameters sent with 
app.post('/recognize', function(req, res) {
    console.info('Got request, processing');
    // TODO: convert from base64?
    // For now just get an array from the client..
    var dataInput = [req.body.payload];
    model.eval(dataInput, (err, evalResult) =>{
        if (err) {
            console.info('Error during eval:', err);
            res.end(err.toString(), 400);
            return;
        }

        // so we use argmax to get the index of the highest value
        var digit = cntk.utils.argmax(evalResult.output);
        responseBody = JSON.stringify({
            'digit' : digit
        });

        console.info('Request processes, returning ')

        res.end(responseBody , 200);
    });
});

console.info("Try setting CNTK device to GPU");
try {
    cntk.setDefaultDeviceSync(cntk.devices.gpu);
}
catch(ex) {
    console.info("Failed setting CNTK GPU device, setting to CPU instead");
    cntk.setDefaultDeviceSync(cntk.devices.cpu);
}

console.info('Trying to load model at:', modelPath)

cntk.loadModel(modelPath, (err, loadedModel) => {
    if (err) {
        console.info('Got error while loading CNTK model:', err);
        return;
    }

    console.info('CNTK model was loaded succesfully');
    model = loadedModel;

    app.listen(3000, function () {
        console.log('node-cntk MNIST sample app listening on port 3000')
    });
});
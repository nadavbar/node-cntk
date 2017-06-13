## node-cntk : Node.js bindings for CNTK (work in progress)

This node.js module provides node.js native bindings for <a href="">CNTK</a>, Microsoft's deep learning library.

### What is it good for?

node-cntk is currently meant to only support <b>model evaluation</b>. (e.g. test time).

That is, you need to first train you model using CNTK's python (or Brain-Script) API, and then consume it in node.js using this module.

### How to use?

1. Require the module and set the default device:

```javascript
const cntk = require('node-cntk');

try {
    // try to set the default device to GPU with device Id 0
    cntk.setDefaultDeviceSync(cntk.CNTKDevices.GPU /*, deviceId (default is 0) */);
}
catch(ex) {
    // failed to set device to GPU, try to set to CPU instead
    cntk.setDefaultDeviceSync(CNTKDevices.CPU);
}
```

Note that for now you can set the device globally, in the future this module will support choosing the device per operation.

2. Load the model using the <b>loadModel</b> async method:

```javascript
// Load the model 
var modelPath = path.join(__dirname, 'mnist', 'mnist_conv.cmf');

cntk.loadModel(modelPath, (err, model) => {
    if (err) {
        console.info('Got error:', err);
        return;
    }
    console.info('Got model!')

    // do something with the model
    // ...
});
```

3. Evaluate a sample (or samples) using the loaded model:

```javascript
// get the data sample
var dataSample = [...];
// in our case, 'input' is the name of the input variable for the model. This can differ across models.
 inputData = {
    'input' : [ dataSample ]
}

// Set the name of the output nodes that we will like to get the results for
// For this model we're just gonna assume it called "output"
outputVariables = ['output']
    
model.eval(inputData, outputVariables, (err, res)=>{
    if (err) {
        console.info(err);
        return;
    }

    // Print the result object.
    // The result object will hold an array with the evaluation result for each output variable 
    
    console.info('Eval result:', res);
})
```

For a full, working sample of evaluating images of hand written digits using a Convolutional Neural Network model trained on the <a href="http://yann.lecun.com/exdb/mnist/">MNIST dataset</a>, please see <a href="https://github.com/nadavbar/node-cntk/blob/master/test/basic.js">this sample</a>.

### TODO

Current list of major work items (For full list, please refer to the issues tab):

- [x] Basic native bindings for setDefaultDevice, loadModel, and eval.
- [x] Basic sample with local model evaluation.
- [ ] Extend native Model class APIs to support model inspection in JavaScript code.
- [ ] MNIST model web server sample.
- [ ] Keras sample: train Keras model with CNTK backend and then load it in node.js.

## License

MIT. See the LICENSE file for more details.


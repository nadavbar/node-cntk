const path = require('path')
addCntkLibsToPath()


const cntk_bindings = require("./build/Release/node_cntk_bindings.node");

function addCntkLibsToPath() {
    if (process.platform == 'win32') {
        cntkDllsPath = path.join(__dirname, "cntk", "dll", "win")
        process.env.PATH = cntkDllsPath + ";" + process.env.PATH;
    }
    else {
        throw new Error('Unsupported platform. Currently this module only supports Windows');
    }
}


module.exports = cntk_bindings;
module.exports.utils = require('./lib/utils.js')

const cntk_bindings = require("./build/Release/node_cntk_bindings.node");

module.exports = cntk_bindings;
module.exports.utils = require('./lib/utils.js')
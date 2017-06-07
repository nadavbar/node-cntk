{
  "targets": [
  {
    "target_name": "node_cntk_bindings",
    "sources": [ 
      "src/node_cntk_bindings.cc",
      "src/LoadModelAsyncWorker.cc",
      "src/LoadModelAsyncWorker.h"
    ],
    "include_dirs": [ 
      "<!(node -e \"require('nan')\")",
      "cntk/include"
    ],
    "conditions": [
      [ 'OS=="win"', {
        "libraries" : [
          "-l../cntk/lib/$(Platform)/$(Configuration)/Cntk.Core-2.0.lib"
        ]
      }]
    ]
  }]
}
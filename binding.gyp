{
  "targets": [
    {
      "target_name": "node_cntk_bindings",
      "sources": [ "src/node_cntk_bindings.cc"],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "libraries": [ ]
    }
  ]
}
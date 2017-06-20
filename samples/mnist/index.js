const express = require('express')
const app = express()

app.use(express.static('public'))

app.listen(3000, function () {
  console.log('node-cntk MNIST sample app listening on port 3000')
})
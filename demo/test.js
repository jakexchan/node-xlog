const express = require('express')
const app = express()
const port = 3000

const { XLogModule } = require("bindings")("nodejs-xlog");

app.get('/', (req, res) => {
  res.send('Hello World!')
})

let xlog;
app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
  xlog = new XLogModule({
    namePrefix: 'Test',
    logDir: '/Users/chenjunkai/Projects/node-xlog/demo/logs'
  })
  console.log(xlog);
})
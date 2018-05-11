const exec = require('exec').exec,
    io = require('io')


io.stdout.write("Hello, World: ");

var out = io.stdin.read(10);
console.log(out)
//const out = exec('uglifyjs', __filename);

//console.log(new TextDecoder().decode(out))
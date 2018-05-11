const exec = require('exec'),
    io = require('io')


console.log(exec.which('node'));

io.stdout.write("Hello, World: ");

var out = io.stdin.read(10);
console.log(out)
//const out = exec('uglifyjs', __filename);

//console.log(new TextDecoder().decode(out))
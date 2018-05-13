const exec = require('exec'),
    io = require('io'),
    prompt = require('prompt');

console.log(process.argv, process.cwd());

prompt.list('Name:', ['choud', 'moud']);


//console.log(new TextDecoder().decode(out))
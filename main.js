const exec = require('exec'),
    io = require('io'),
    prompt = require('prompt'),
    util = require('util');

console.log(util.flags(process.argv.slice(1)), process.cwd());

prompt.list('Name:', ['choud', 'moud']);


//console.log(new TextDecoder().decode(out))
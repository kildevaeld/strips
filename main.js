const exec = require('exec'),
    io = require('io'),
    prompt = require('prompt'),
    util = require('util');

const flags = util.flags(process.argv.slice(1), {
    name: {
        type: "string",
        alias: 'n'
    }
});

console.log(flags, process.cwd());

prompt.list('Name:', ['choud', 'moud']);


//console.log(new TextDecoder().decode(out))
const prompt = require('prompt');

const output = {
    name: prompt.input({
        message: 'Name',
        defaults: 'Peter',
        col: 4
    }),
    create: prompt.confirm('Create'),
    select: prompt.list({
        message: 'Hello',
        choices: ['Hello', 'World', "Ya'll"],
        max: 1,
        clear: false,
        col: 2
    }),
    mselect: prompt.list({
        message: 'Hello',
        choices: ['Hello', 'World', "Ya'll"],
        max: 2,
        clear: false
    })
};

console.log(output);
/*
const out = prompt.input({
    message: 'test',
    defaults: 'Rasmus',
    clear: true
})
console.log(out);*/
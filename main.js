const curl = require('curl'),
    io = require('io'),
    inc = require('./include');
console.log(inc.test);
const decoder = new TextDecoder('utf8');

const resp = curl.get('http://localhost:3000');

console.log(decoder.decode(resp.body));


const file = new io.File(__filename);

var r = curl.post('http://localhost:3000/post', {
    data: file
});

console.log(decoder.decode(r.body));
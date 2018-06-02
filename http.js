const curl = require('curl'),
    io = require('io');

const re = new curl.Request({
    url: "https://google.com",
});



const resp = curl.req(re);

console.log('done');
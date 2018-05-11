const io = require('io'),
    curl = require('curl');



const req = new curl.Request({
    url: "http://google.com",
});

const res = curl.req(req);
const io = require('io'),
    curl = require('curl'),
    fs = require('filesystem');


/*
const req = new curl.Request({
    url: "http://google.com",
});

const res = curl.req(req);*/

fs.readdir('.', function (str) {
    console.log(str)
})
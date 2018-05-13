const io = require('io'),
    fs = require('filesystem'),
    path = require("path"),
    exec = require('exec').exec

const templatePath = path.join(__dirname, "../js");
const fileData = []
fs.readdir(templatePath, function (fileName) {
    const full = path.join(templatePath, fileName);

    const file = new io.File(full);
    len = file.seek(0, io.SEEK_END).tell()

    if (len == 0) return;

    const out = file.read(len);

    //const out = exec('uglifyjs', full);

    fileData.push({
        file: full,
        buffer: out
    });

});


//exec('uglifyjs')


var out = ['#pragma once\n'];

function format(buffer) {
    var out = []
    for (var i = 0; i < buffer.length; i++) {
        out.push(buffer[i])
    }
    return out.join(', ');
}

fileData.forEach(function (c) {
    var base = path.basename(c.file).replace('.js', '').replace(/\-/, '_');
    out.push("static const unsigned char " + base + "_js[] = {" + format(c.buffer) + '};\n');
    out.push("static int " + base + "_js_len = " + c.buffer.length + ';\n');
});;
console.log(out.join('\n'));
//io.stdout.write(out.join('\n'));
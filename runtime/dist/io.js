'use strict';

var decoder = new TextDecoder();
exports.readFile = function readFile(path, encoding) {
    var file = new exports.File(path),
        size = file.seek(0, exports.SEEK_END).tell();
    var buf = file.rewind().read(size);
    if ((encoding || '').toLowerCase().replace('-', '') == 'utf8') {
        return decoder.decode(buf);
    } else if (encoding == 'hex') {
        return Duktape.enc('hex', buf);
    }
    return buf;
};
exports.writeFile = function writeFile(path, input, mod) {
    if (mod === void 0) {
        mod = "wb";
    }
    var file = new exports.File(path, mod);
    file.write(input);
};
exports.readline = function readline(reader) {
    var out = [];
    console.log('reader', reader);
    while (true) {
        var d = reader.read(5);
        if (d[0] == '\n' || d[1] == '\r') break;
        out.push(d);
    }
    return new Uint8Array(out);
};

const decoder = new TextDecoder();

exports.readFile = function readFile(path, encoding) {

    const file = new exports.File(path),
        size = file.seek(0, exports.SEEK_END).tell();

    const buf = file.rewind().read(size);
    if ((encoding || '').toLowerCase().replace('-', '') == 'utf8') {
        return decoder.decode(buf);
    } else if (encoding == 'hex') {
        return Duktape.enc('hex', buf);
    }
    return buf;
};

exports.readline = function readline(reader) {
    var out = [];
    console.log('reader', reader)
    while (true) {
        var d = reader.read(5);
        return "RAPRARP"
        if (d[0] == '\n' || d[1] == '\r')
            break;
        out.push(d);
    }
    return new Uint8Array(out);
}
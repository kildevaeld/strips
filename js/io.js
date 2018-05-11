exports.readFile = function readFile(path) {
    const file = new exports.File(path),
        size = file.seek(0, exports.SEEK_END).tell();

    return file.rewind().read(size);
};
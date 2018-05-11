var _slice = Array.prototype.slice;

['get', 'post', 'put', 'patch', 'head', 'del']
.forEach(function (key) {
    exports[key] = function (url, request) {
        request = request || {}
        if (request instanceof exports.Request) {
            req.url = url;
            req.method = key.toUpperCase();
        } else {
            request = new exports.Request(Object.assign(request, {
                url: url,
                method: key.toUpperCase()
            }));
        }
        return exports.req(request);
    }
});
const _slice = Array.prototype.slice;

['get', 'post', 'put', 'patch', 'head', 'del']
    .forEach(function (key) {
        exports[key] = function (url: string, request: any) {
            request = request || {}
            if (request instanceof exports.Request) {
                request.url = url;
                request.method = key.toUpperCase();
            } else {
                request = new exports.Request(Object.assign(request, {
                    url: url,
                    method: key.toUpperCase()
                }));
            }
            return exports.do(request);
        }
    });
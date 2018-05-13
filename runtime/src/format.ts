

var _slice = Array.prototype.slice;
export const formatters = {
    s: function (data: any) {
        return String(data);
    },
    O: function (data: any) {
        return JSON.stringify(data);
    }
};
export function format() {
    var args = _slice.call(arguments);
    if ('string' !== typeof args[0]) {
        // anything else let's inspect with %O
        args.unshift('%O');
    }

    var index = 0,
        self = this;
    args[0] = args[0].replace(/%([a-zA-Z%])/g, function (match, format) {
        // if we encounter an escaped % then don't increase the array index
        if (match === '%%') return match;
        index++;
        var formatter = formatters[format];
        if ('function' === typeof formatter) {
            var val = args[index];
            match = formatter.call(this, val);

            // now we need to remove `args[index]` since it's inlined in the `format`
            args.splice(index, 1);
            index--;
        }
        return match;
    });

    return args.join(' ');
};
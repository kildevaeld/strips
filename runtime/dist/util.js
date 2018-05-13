'use strict';

Object.defineProperty(exports, '__esModule', { value: true });

var minimist = function minimist(args, opts) {
    if (!opts) opts = {};

    var flags = { bools: {}, strings: {}, unknownFn: null };

    if (typeof opts['unknown'] === 'function') {
        flags.unknownFn = opts['unknown'];
    }

    if (typeof opts['boolean'] === 'boolean' && opts['boolean']) {
        flags.allBools = true;
    } else {
        [].concat(opts['boolean']).filter(Boolean).forEach(function (key) {
            flags.bools[key] = true;
        });
    }

    var aliases = {};
    Object.keys(opts.alias || {}).forEach(function (key) {
        aliases[key] = [].concat(opts.alias[key]);
        aliases[key].forEach(function (x) {
            aliases[x] = [key].concat(aliases[key].filter(function (y) {
                return x !== y;
            }));
        });
    });

    [].concat(opts.string).filter(Boolean).forEach(function (key) {
        flags.strings[key] = true;
        if (aliases[key]) {
            flags.strings[aliases[key]] = true;
        }
    });

    var defaults = opts['default'] || {};

    var argv = { _: [] };
    Object.keys(flags.bools).forEach(function (key) {
        setArg(key, defaults[key] === undefined ? false : defaults[key]);
    });

    var notFlags = [];

    if (args.indexOf('--') !== -1) {
        notFlags = args.slice(args.indexOf('--') + 1);
        args = args.slice(0, args.indexOf('--'));
    }

    function argDefined(key, arg) {
        return flags.allBools && /^--[^=]+$/.test(arg) || flags.strings[key] || flags.bools[key] || aliases[key];
    }

    function setArg(key, val, arg) {
        if (arg && flags.unknownFn && !argDefined(key, arg)) {
            if (flags.unknownFn(arg) === false) return;
        }

        var value = !flags.strings[key] && isNumber(val) ? Number(val) : val;
        setKey(argv, key.split('.'), value);

        (aliases[key] || []).forEach(function (x) {
            setKey(argv, x.split('.'), value);
        });
    }

    function setKey(obj, keys, value) {
        var o = obj;
        keys.slice(0, -1).forEach(function (key) {
            if (o[key] === undefined) o[key] = {};
            o = o[key];
        });

        var key = keys[keys.length - 1];
        if (o[key] === undefined || flags.bools[key] || typeof o[key] === 'boolean') {
            o[key] = value;
        } else if (Array.isArray(o[key])) {
            o[key].push(value);
        } else {
            o[key] = [o[key], value];
        }
    }

    function aliasIsBoolean(key) {
        return aliases[key].some(function (x) {
            return flags.bools[x];
        });
    }

    for (var i = 0; i < args.length; i++) {
        var arg = args[i];

        if (/^--.+=/.test(arg)) {
            // Using [\s\S] instead of . because js doesn't support the
            // 'dotall' regex modifier. See:
            // http://stackoverflow.com/a/1068308/13216
            var m = arg.match(/^--([^=]+)=([\s\S]*)$/);
            var key = m[1];
            var value = m[2];
            if (flags.bools[key]) {
                value = value !== 'false';
            }
            setArg(key, value, arg);
        } else if (/^--no-.+/.test(arg)) {
            var key = arg.match(/^--no-(.+)/)[1];
            setArg(key, false, arg);
        } else if (/^--.+/.test(arg)) {
            var key = arg.match(/^--(.+)/)[1];
            var next = args[i + 1];
            if (next !== undefined && !/^-/.test(next) && !flags.bools[key] && !flags.allBools && (aliases[key] ? !aliasIsBoolean(key) : true)) {
                setArg(key, next, arg);
                i++;
            } else if (/^(true|false)$/.test(next)) {
                setArg(key, next === 'true', arg);
                i++;
            } else {
                setArg(key, flags.strings[key] ? '' : true, arg);
            }
        } else if (/^-[^-]+/.test(arg)) {
            var letters = arg.slice(1, -1).split('');

            var broken = false;
            for (var j = 0; j < letters.length; j++) {
                var next = arg.slice(j + 2);

                if (next === '-') {
                    setArg(letters[j], next, arg);
                    continue;
                }

                if (/[A-Za-z]/.test(letters[j]) && /=/.test(next)) {
                    setArg(letters[j], next.split('=')[1], arg);
                    broken = true;
                    break;
                }

                if (/[A-Za-z]/.test(letters[j]) && /-?\d+(\.\d*)?(e-?\d+)?$/.test(next)) {
                    setArg(letters[j], next, arg);
                    broken = true;
                    break;
                }

                if (letters[j + 1] && letters[j + 1].match(/\W/)) {
                    setArg(letters[j], arg.slice(j + 2), arg);
                    broken = true;
                    break;
                } else {
                    setArg(letters[j], flags.strings[letters[j]] ? '' : true, arg);
                }
            }

            var key = arg.slice(-1)[0];
            if (!broken && key !== '-') {
                if (args[i + 1] && !/^(-|--)[^-]/.test(args[i + 1]) && !flags.bools[key] && (aliases[key] ? !aliasIsBoolean(key) : true)) {
                    setArg(key, args[i + 1], arg);
                    i++;
                } else if (args[i + 1] && /true|false/.test(args[i + 1])) {
                    setArg(key, args[i + 1] === 'true', arg);
                    i++;
                } else {
                    setArg(key, flags.strings[key] ? '' : true, arg);
                }
            }
        } else {
            if (!flags.unknownFn || flags.unknownFn(arg) !== false) {
                argv._.push(flags.strings['_'] || !isNumber(arg) ? arg : Number(arg));
            }
            if (opts.stopEarly) {
                argv._.push.apply(argv._, args.slice(i + 1));
                break;
            }
        }
    }

    Object.keys(defaults).forEach(function (key) {
        if (!hasKey(argv, key.split('.'))) {
            setKey(argv, key.split('.'), defaults[key]);

            (aliases[key] || []).forEach(function (x) {
                setKey(argv, x.split('.'), defaults[key]);
            });
        }
    });

    if (opts['--']) {
        argv['--'] = new Array();
        notFlags.forEach(function (key) {
            argv['--'].push(key);
        });
    } else {
        notFlags.forEach(function (key) {
            argv._.push(key);
        });
    }

    return argv;
};

function hasKey(obj, keys) {
    var o = obj;
    keys.slice(0, -1).forEach(function (key) {
        o = o[key] || {};
    });

    var key = keys[keys.length - 1];
    return key in o;
}

function isNumber(x) {
    if (typeof x === 'number') return true;
    if (/^0x[0-9a-f]+$/i.test(x)) return true;
    return (/^[-+]?(?:\d+(?:\.\d*)?|\.\d+)(e[-+]?\d+)?$/.test(x)
    );
}

var toString = Object.prototype.toString;

var isPlainObj = function isPlainObj(x) {
	var prototype;
	return toString.call(x) === '[object Object]' && (prototype = Object.getPrototypeOf(x), prototype === null || prototype === Object.getPrototypeOf({}));
};

var arrify = function arrify(val) {
	if (val === null || val === undefined) {
		return [];
	}

	return Array.isArray(val) ? val : [val];
};

var push = function push(obj, prop, value) {
	if (!obj[prop]) {
		obj[prop] = [];
	}

	obj[prop].push(value);
};

var insert = function insert(obj, prop, key, value) {
	if (!obj[prop]) {
		obj[prop] = {};
	}

	obj[prop][key] = value;
};

var passthroughOptions = ['stopEarly', 'unknown', '--'];

var minimistOptions = function minimistOptions(options) {
	options = options || {};

	var result = {};

	passthroughOptions.forEach(function (key) {
		if (options[key]) {
			result[key] = options[key];
		}
	});

	Object.keys(options).forEach(function (key) {
		var value = options[key];

		if (key === 'arguments') {
			key = '_';
		}

		// If short form is used
		// convert it to long form
		// e.g. { 'name': 'string' }
		if (typeof value === 'string') {
			value = { type: value };
		}

		if (isPlainObj(value)) {
			var props = value;

			if (props.type) {
				var type = props.type;

				if (type === 'string') {
					push(result, 'string', key);
				}

				if (type === 'boolean') {
					push(result, 'boolean', key);
				}
			}

			var aliases = arrify(props.alias);

			aliases.forEach(function (alias) {
				insert(result, 'alias', alias, key);
			});

			if ({}.hasOwnProperty.call(props, 'default')) {
				insert(result, 'default', key, props.default);
			}
		}
	});

	return result;
};

var _slice = Array.prototype.slice;
var formatters = {
    s: function s(data) {
        return String(data);
    },
    O: function O(data) {
        return JSON.stringify(data);
    }
};
function format() {
    var args = _slice.call(arguments);
    if ('string' !== typeof args[0]) {
        // anything else let's inspect with %O
        args.unshift('%O');
    }
    var index = 0;
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
}

function flags(argv, options) {
    options = options || {};
    return minimist(argv, minimistOptions(options));
}

exports.flags = flags;
exports.formatters = formatters;
exports.format = format;

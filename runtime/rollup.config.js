const resolve = require('rollup-plugin-node-resolve'),
    commonjs = require('rollup-plugin-commonjs'),
    babel = require('rollup-plugin-babel'),
    typescript = require('rollup-plugin-typescript');

const pkg = require('./package.json');

const plugins = [
    typescript({
        typescript: require('typescript')
    }),
    resolve(), // so Rollup can find `ms`
    commonjs(), // so Rollup can convert `ms` to an ES module
    babel({
        //presets: ['env'],
        //exclude: ['node_modules/**']
    })
];

function config(name) {
    return {
        input: `./src/${name}.ts`,
        output: {
            file: `dist/${name}.js`,
            format: 'cjs',
        },
        plugins: plugins
    }
}


module.exports = [
    // browser-friendly UMD build
    config('util'),
    config('io'),
    config('curl'),
    config('polyfills')
];
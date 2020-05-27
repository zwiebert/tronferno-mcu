// rollup.config.js
import json from '@rollup/plugin-json';
import { terser } from 'rollup-plugin-terser';
import strip from '@rollup/plugin-strip';

const isProduction = process.env.buildTarget === "PROD";

export default {
    input: 'src/main.js',
    output: [{
        file: 'build/bundle.js',
        format: 'iife',
        name: 'tfmcu',
        plugins: [
        ]
    }, {
        file: 'build/bundle.min.js',
        format: 'iife',
        name: 'tfmcu',
        plugins: [
            terser()
        ]
    }
    ],
    plugins: [
        json(),
        ...isProduction ? [
            strip({
                functions: ['testing.*', 'dbLog', 'console.*', 'assert.*'],
                sourceMap: true
            })] : []
    ]
};

// rollup.config.js
import json from '@rollup/plugin-json';
import { terser } from 'rollup-plugin-terser';
import strip from '@rollup/plugin-strip';
import { eslint } from "rollup-plugin-eslint";
import svelte from 'rollup-plugin-svelte';
import resolve from '@rollup/plugin-node-resolve';
import sveltePreprocess from 'svelte-preprocess';

export const isProduction = process.env.buildTarget === "PROD";

export default {
  onwarn(warning, rollupWarn) {
    if (warning.code !== 'CIRCULAR_DEPENDENCY') {
      rollupWarn(warning);
    }
  },
  input: 'src/main.js',
  output: [...!isProduction ? [ {
    file: 'build_dev/wapp.js',
    sourcemap: true,
    format: 'iife',
    name: 'wapp',
    // sourcemapPathTransform: relativePath => {      return relativePath.substr(2);},
    plugins: [
    ]
  }] : [ {
    file: 'build/wapp.js',
    format: 'iife',
    name: 'wapp',
    sourcemap: true,
    sourcemapPathTransform: relativePath => {
      // will transform e.g. "src/main.js" -> "main.js"
      return relativePath.substr(2);
    },
    plugins: [
      terser()
    ]
  }]],
  plugins: [
    json(),
    ...isProduction ? [
      strip({
        functions: ['testing.*', 'testing_*', 'appDebug.*', 'console.*', 'assert.*'],
        labels: ['testing'],
        sourceMap: true
      })] : [],
    eslint(),
    svelte({
      dev: !isProduction,
      //emitCss: true,
      css: css => { css.write(isProduction ? 'build/wapp.css' : 'build_dev/wapp.css'); },
      onwarn: (warning, handler) => {
        // e.g. don't warn on <marquee> elements, cos they're cool
        if (warning.code === 'a11y-no-onchange') return;

        // let Rollup handle all other warnings normally
        handler(warning);
      },
      preprocess: sveltePreprocess({postcss: true}),

    }),
    // If you have external dependencies installed from
    // npm, you'll most likely need these plugins. In
    // some cases you'll need additional configuration -
    // consult the documentation for details:
    // https://github.com/rollup/plugins/tree/master/packages/commonjs
    resolve({
      browser: true,
      dedupe: ['svelte']
    })
  ]
};

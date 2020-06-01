// rollup.config.js
import json from '@rollup/plugin-json';
import { terser } from 'rollup-plugin-terser';
import strip from '@rollup/plugin-strip';
import { eslint } from "rollup-plugin-eslint";
import svelte from 'rollup-plugin-svelte';
import resolve from '@rollup/plugin-node-resolve';

export const isProduction = process.env.buildTarget === "PROD";




export default {
  onwarn(warning, rollupWarn) {
    if (warning.code !== 'CIRCULAR_DEPENDENCY') {
      rollupWarn(warning);
    }
  },
  input: 'src/main.js',
  output: [{
    file: 'build/bundle.js',
    sourcemap: true,
    format: 'iife',
    name: 'tfmcu',
   // sourcemapPathTransform: relativePath => {      return relativePath.substr(2);},
    plugins: [
    ]
  }, {
    file: 'build/bundle.min.js',
    format: 'iife',
    name: 'tfmcu',
    sourcemap: true,
    sourcemapPathTransform: relativePath => {
      // will transform e.g. "src/main.js" -> "main.js"
      return relativePath.substr(2);
    },
    plugins: [
      terser()
    ]
  }
  ],
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
      dev: !isProduction //
      , css: css => {	 css.write('build/bundle.css');  }
      , onwarn: (warning, handler) => {
    // e.g. don't warn on <marquee> elements, cos they're cool
    if (warning.code === 'a11y-no-onchange') return;

    // let Rollup handle all other warnings normally
    handler(warning);
  }
      // By default, all .svelte and .html files are compiled
      //extensions: ['.my-custom-extension'],

      // You can restrict which files are compiled
      // using `include` and `exclude`
      //include: 'src/*.svelte',

      // By default, the client-side compiler is used. You
      // can also use the server-side rendering compiler
      //generate: 'ssr',

      // ensure that extra attributes are added to head
      // elements for hydration (used with ssr: true)
      //hydratable: true,

      // Optionally, preprocess components with svelte.preprocess:
      // https://svelte.dev/docs#svelte_preprocess
      /*
  preprocess: {
    style: ({ content }) => {
      return transformStyles(content);
    }

  },

  // Emit CSS as "files" for other plugins to process
  emitCss: false,

  // Extract CSS into a separate file (recommended).
  // See note below
  css: function (css) {
    console.log(css.code); // the concatenated CSS
    console.log(css.map); // a sourcemap

    // creates `main.css` and `main.css.map` — pass `false`
    // as the second argument if you don't want the sourcemap
    css.write('public/main.css');
  },

  // Warnings are normally passed straight to Rollup. You can
  // optionally handle them here, for example to squelch
  // warnings with a particular code
  onwarn: (warning, handler) => {
    // e.g. don't warn on <marquee> elements, cos they're cool
    if (warning.code === 'a11y-distracting-elements') return;

    // let Rollup handle all other warnings normally
    handler(warning);
  }
  */
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

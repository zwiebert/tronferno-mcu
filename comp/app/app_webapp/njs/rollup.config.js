// rollup.config.js
import replace from "@rollup/plugin-replace";
import json from "@rollup/plugin-json";
import strip from "@rollup/plugin-strip";
import resolve from "@rollup/plugin-node-resolve";
import commonjs from "@rollup/plugin-commonjs";
import alias from "@rollup/plugin-alias";
import svelte from "rollup-plugin-svelte";
import sveltePreprocess from "svelte-preprocess";
import { terser } from "@wwa/rollup-plugin-terser";
import css from "rollup-plugin-css-only";


export const isProduction = process.env.NODE_ENV === "production";
export const isDistro = process.env.DISTRO === "1";
const build_directory = process.env.BUILD_DIR || "/tmp/tronferno-mcu/njs/build";

console.log("isProduction:", isProduction, "isDistro:", isDistro);

let wdir = __dirname + "/";

const aliases = alias({
  resolve: [".svelte", ".js"], //optional, by default this will just look for .js files or folders
  entries: [
    { find: "components", replacement: wdir + "src/components" },
    { find: "services", replacement: wdir + "src/services" },
    { find: "stores", replacement: wdir + "src/store" },
    { find: "panes", replacement: wdir + "src/panes" },
    { find: "app", replacement: wdir + "src/app" },
    { find: "main", replacement: wdir + "src" },
  ],
});

export default {
  onwarn(warning, rollupWarn) {
    if (warning.code !== "CIRCULAR_DEPENDENCY") {
      rollupWarn(warning);
    }
  },
  input: "src/main.js",
  output: [
    ...(!isProduction
      ? [
          {
            file: build_directory + "/wapp.js",
            sourcemap: true,
            format: "iife",
            name: "wapp",
            // sourcemapPathTransform: relativePath => {      return relativePath.substr(2);},
            plugins: [],
          },
        ]
      : [
          {
            file: build_directory + "/wapp.js",
            format: "iife",
            name: "wapp",
            sourcemap: true,
            sourcemapPathTransform: (relativePath) => {
              // will transform e.g. "src/main.js" -> "main.js"
              return relativePath.substr(2);
            },
            plugins: [terser()],
          },
        ]),
  ],
  moduleContext: (id) => {
    // In order to match native module behaviour, Rollup sets `this`
    // as `undefined` at the top level of modules. Rollup also outputs
    // a warning if a module tries to access `this` at the top level.
    // The following modules use `this` at the top level and expect it
    // to be the global `window` object, so we tell Rollup to set
    // `this = window` for these modules.
    const thisAsWindowForModules = [
      "node_modules/intl-messageformat/lib/core.js",
      "node_modules/intl-messageformat/lib/compiler.js",
      "node_modules/intl-messageformat/lib/formatters.js",
      "node_modules/intl-format-cache/lib/index.js",
      "node_modules/intl-messageformat-parser/lib/normalize.js",
      "node_modules/intl-messageformat-parser/lib/parser.js",
      "node_modules/intl-messageformat-parser/lib/skeleton.js",
    ];

    if (thisAsWindowForModules.some((id_) => id.trimRight().endsWith(id_))) {
      return "window";
    }
  },
  plugins: [
    replace({
      preventAssignment:true,
      // workaround the way sveltejs-tippy imports tippy: https://github.com/mdauner/sveltejs-tippy/issues/117
      "process.env.NODE_ENV": JSON.stringify(
        isProduction ? "production" : "development"
      ),
    }),
    aliases,
    json(),
    ...(isProduction
      ? [
          strip({
           //include: "src/**/*.(js)",
           functions: ["testing.*", "testing_*", "appDebug.*", "console.*", "assert.*"],
           labels: ["testing"],
           sourceMap: true,
          }),
        ]
      : []),
    svelte({
      preprocess: sveltePreprocess({
        postcss: true,
        replace: [          
          ["misc.NODE_ENV_DEV", isProduction ? "false" : "true"],
          ["misc.PROD", isProduction ? "true" : "false"],
          ["misc.DISTRO", isDistro ? "true" : "false"],
          ["//NODE_ENV_DEV", isProduction ? "if(false)" : "if(true)"],
          ...(isProduction ? [
          ] : []),
        ],
      }),
      compilerOptions: {
        dev: !isProduction,
      },
      onwarn: (warning, handler) => {
        if (warning.code === "a11y-no-onchange") return;
        if (warning.code === "css-unused-selector") return;
        if (warning.code.startsWith("a11y-")) return;
        handler(warning);
      },
    }),
    css({ output: "wapp.css" }),
    // If you have external dependencies installed from
    // npm, you'll most likely need these plugins. In
    // some cases you'll need additional configuration -
    // consult the documentation for details:
    // https://github.com/rollup/plugins/tree/master/packages/commonjs
    resolve({
      browser: true,
      dedupe: ["svelte"],
    }),
    commonjs(),
  ],
};

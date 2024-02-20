module.exports = {
  parserOptions: {
    ecmaVersion: 2018,
    sourceType: "module",
  },
  env: {
    es6: true,
    browser: true,
  },
  plugins: [],
  overrides: [
    // {      files: ['*.svelte'],   processor: 'svelte/svelte'   }
  ],
  rules: {
    // override/add rules settings here, such as:
    // 'svelte/rule-name': 'error'
    "css-unused-selector": "off",
    "a11y-no-onchange": "off",
    "no-var": "error",
    semi: "error",
    "max-params": ["error", 4],
    eqeqeq: "error",
  },
  settings: {
    // ...
  },
  extends: [
    // add more generic rule sets here, such as:
    // 'eslint:recommended',
    "plugin:svelte/recommended",
  ],
};

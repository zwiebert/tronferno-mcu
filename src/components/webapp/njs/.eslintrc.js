module.exports = {
  parserOptions: {
    ecmaVersion: 2015,
    sourceType: 'module'
  },
  env: {
    es6: true,
    browser: true
  },
  plugins: [
    'svelte3'
  ],
  overrides: [
    {
      files: ['*.svelte'],
      processor: 'svelte3/svelte3'
    }
  ],
  rules: {
    "a11y-no-onchange":"off",
    "no-var": "error",
    "semi": "error",
    "max-params": [
      "error",
      4
    ],
    "eqeqeq": "error"
  },
  settings: {
    // ...
  },
   "extends": "eslint:recommended" 
};

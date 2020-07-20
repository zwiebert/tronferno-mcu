const isProduction = process.env.buildTarget === "PROD";

module.exports = {
  plugins: [
    require('tailwindcss'),
    isProduction && require('@fullhuman/postcss-purgecss')({
      content: [
        './**/**/*.html',
        './**/**/*.svelte'
      ],
    
      whitelistPatterns: [/svelte-/],
    
      defaultExtractor: content => content.match(/[\w-/:]+(?<!:)/g) || []
    }),
  ]
};

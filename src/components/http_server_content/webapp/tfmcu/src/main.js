'use strict';
// src/main.js

//import * as testing from './testing.svelte';
import * as appState from './app_state.js';
import * as httpFetch from './fetch.js';

export default function() {
 // testing.testing_init();
  generate_html();
  appState.init();
  httpFetch.http_postDocRequest('cliparm_config');
}


import App from './tfmcu_html.svelte';

function generate_html() {
    const app = new App({
    target: document.body,
    props: {
   
    }
  });
}

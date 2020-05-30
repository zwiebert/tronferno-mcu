'use strict';
// src/main.js

//import * as testing from './testing.svelte';
import * as appState from './app_state.svelte';
import * as httpFetch from './fetch.js';
import * as connWs from './net/conn_ws.js';

export default function() {
 // testing.testing_init();
  generate_html();
  appState.init();
  setTimeout(() => { connWs.websocket(); }, 1000);
  httpFetch.http_postDocRequest('cliparm_config');
}


import App from './tfmcu_html.svelte';

function generate_html() {
    // eslint-disable-next-line no-unused-vars
    const app = new App({
    target: document.body,
    props: {
   
    }
  });
}

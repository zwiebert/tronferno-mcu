'use strict';
// src/main.js

import App from './tfmcu_html.svelte';
import * as appState from './app_state.svelte';
import * as connWs from './net/conn_ws.js';

export default function () {
  // testing.testing_init();
  generate_html();
  appState.init();
  setTimeout(() => { connWs.websocket(); }, 1000);
}

function generate_html() {
  let isProduction = true;
  // eslint-disable-next-line no-unused-labels 
  testing: { isProduction = false; }

  // eslint-disable-next-line no-unused-vars    
  const app = new App({
    target: document.body,
    props: {
      isProduction: isProduction
    }
  });
}

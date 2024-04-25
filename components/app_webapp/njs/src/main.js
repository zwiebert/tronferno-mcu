'use strict';
// src/main.js

import App from "./app/main.svelte";
import * as appState from "./app/app_state.svelte";
import * as connWs from "./net/conn_ws.js";

export default function () {
  // testing.testing_init();
  generate_html();
  appState.init();
  setTimeout(() => { connWs.websocket(); }, 1000);
}

function generate_html() {
  // eslint-disable-next-line no-unused-vars    
  const app = new App({
    target: document.body,
    props: {
    }
  });
}

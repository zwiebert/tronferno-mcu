<script context="module">
'use strict';
import * as httpFetch from './fetch.js';

const load_fetch = httpFetch.FETCH_GMU;

export class AppState {

  constructor() {
    this.docs = {};
    this.mEsp32BootCount = 0;
    this.mWebSocket = 0;
  }

  load() {
    httpFetch.http_fetchByMask(load_fetch);
  }
}

export var ast;
export function init() {
  ast = new AppState();
  ast.load();
}
</script>


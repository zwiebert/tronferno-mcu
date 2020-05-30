<script>
</script>
<script context="module">
'use strict';
import * as appDebug from './app_debug.js';
import * as shutterAlias from './shutter_alias.svelte';
import * as httpFetch from './fetch.js';
import * as connWs from './net/conn_ws.js';
import {GM, Auto,Pct,Name} from './store/curr_shutter.js';
import {GmcFetch} from './store/app_state.js';

const load_fetch = httpFetch.FETCH_GMU;

export class AppState {

  constructor() {
    this.mAuto = {  auto: {} };
    this.mPair = { all: {} };
    this.mShutterPrefs = {};
    this.docs = {};
    GmcFetch.subscribe(value => this.gmc_fetch = value);
    GM.subscribe(value => this.gm = value);
    this.mEsp32BootCount = 0;
    this.mWebSocket = 0;
  }

  load() {
    httpFetch.http_fetchByMask(load_fetch);
    setTimeout(() => { ast.mWebSocket = connWs.websocket(); }, 1000);
  }

   getAutoName() { return "auto" + this.gm; }
  getAutoObj() { let key = this.getAutoName(); return (key in this.mAuto.auto) ? this.mAuto.auto[key] : {}; }
  linkAutoObj() { Auto.set(this.getAutoObj());}

  gmChanged() {
    this.linkAutoObj();
    appDebug.dbLog(JSON.stringify(this));
    if (this.gmc_fetch)
      httpFetch.http_fetchByMask(this.gmc_fetch);
    shutterAlias.aliasPaired_updHtml();
  }

  set auto(obj) {
    this.mAuto.auto[this.getAutoName()] = obj;
    this.linkAutoObj();
    //shutterAuto.automaticOptions_updHtml();
  }

  aliases_add(key, val) {
    this.mPair.all[key] = val;
    shutterAlias.aliasControllers_updHtml();
  }

  set aliases(value) {
    this.mPair.all = value;
    shutterAlias.aliasControllers_updHtml();
  }

  get aliases() {
    return this.mPair.all;
  }


}

export var ast;
export function init() {
  ast = new AppState();
  ast.load();
}
</script>


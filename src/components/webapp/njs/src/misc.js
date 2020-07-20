'use strict';

import { ReloadProgress } from './store/app_state.js';

import * as httpFetch from './fetch.js';

const reload_Progress = {
  ivId: 0,
  ms: 1000,
  count: 12,
  counter: 0,
};

function req_reloadTick() {
  const rpr = reload_Progress;

  if (++rpr.counter > rpr.count) {
    location.reload();
    clearInterval(rpr.ivId); // may be useless after reload...
  } else {
    ReloadProgress.set(reload_Progress.counter * 100 / reload_Progress.count);
  }
}

function req_reloadStart() {
  const rpr = reload_Progress;

  rpr.ivId = setInterval(req_reloadTick, rpr.ms);
}

export function req_mcuRestart() {
  let json = { to: "tfmcu", config: { restart: "1" } };
  let url = '/cmd.json';
  httpFetch.http_postRequest(url, json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}


'use strict';
import * as appState from './app_state.js';
import * as httpFetch from './fetch.js';


export function shn_fromHtml_toMcu() {
  const val = document.getElementById("smn").value;

  let tfmcu = { "to":"tfmcu", "shpref":{"g":appState.ast.g, "m":appState.ast.m, "tag.NAME":val }};

  let url = '/cmd.json';
    httpFetch.http_postRequest(url, tfmcu);
  }

  export function shn_updHtml() {
    const shsgm_key = "shs" + appState.ast.g.toString() + appState.ast.m.toString();
    if (shsgm_key in appState.ast.shutterPrefs) {
      const shsgm = appState.ast.shutterPrefs[shsgm_key];
      if ('tag.NAME' in shsgm) {
        document.getElementById("smn").value = shsgm['tag.NAME'];
        return;
      }
    }
    document.getElementById("smn").value = "";
  }
'use strict';
import * as as from './app_state.js';
import * as a_http from './fetch.js';


export function shn_fromHtml_toMcu(){
  const val = document.getElementById("smn").value;

  let tfmcu = {"to":"tfmcu", "shpref":{"g":as.ast.g, "m":as.ast.m, "tag.NAME":val }};

  var url = '/cmd.json';
  a_http.http_postRequest(url, tfmcu);
}

export function shn_updHtml() {
  const shsgm_key = "shs" + as.ast.g.toString() + as.ast.m.toString();
  if (shsgm_key in as.ast.shutterPrefs) {
    const shsgm = as.ast.shutterPrefs[shsgm_key];
    if ('tag.NAME' in shsgm) {
      document.getElementById("smn").value = shsgm['tag.NAME'];
      return;
    }
  }
  document.getElementById("smn").value = "";
}
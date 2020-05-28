'use strict';
import * as aps from './app_state.js';
import * as a_http from './fetch.js';


export function shn_fromHtml_toMcu() {
  const val = document.getElementById("smn").value;

  let tfmcu = { "to":"tfmcu", "shpref":{"g":aps.ast.g, "m":aps.ast.m, "tag.NAME":val }};

  var url = '/cmd.json';
    a_http.http_postRequest(url, tfmcu);
  }

  export function shn_updHtml() {
    const shsgm_key = "shs" + aps.ast.g.toString() + aps.ast.m.toString();
    if (shsgm_key in aps.ast.shutterPrefs) {
      const shsgm = aps.ast.shutterPrefs[shsgm_key];
      if ('tag.NAME' in shsgm) {
        document.getElementById("smn").value = shsgm['tag.NAME'];
        return;
      }
    }
    document.getElementById("smn").value = "";
  }
'use strict';

import { ReloadProgress } from "../store/app_state.js";
//import { set } from "svelte/store";
import { GuiAcc } from "../store/app_state";
import * as httpFetch from "../app/fetch.js";

export const NODE_ENV_DEV = true;
export const PROD = false;
export const DISTRO = false;

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
  httpFetch.http_postCommand(json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}



export function set_gui_access_level(al) {
  const developer = -1;
  const expert = 0;
  const admin = 10;
  const user = 20;
  const kid = 30;
  GuiAcc.set( {
    cfg: al < user,
    mcu_cfg: al < user,
    shutter_sep: al < admin,
    shutter_auto: al < kid,
    shutter_time_iso: al < admin,
    shutter_sunpos: al < kid,
    edit_transmitter_names: al < user,
    edit_shutter_names: al < user,
    edit_shutter_durations: al < user,
    edit_rf_module: al < admin,
    restore_shutter_names:al < admin,
    set_func_by_transmitter_id: al < admin,
    register_by_transmitter_id: al < admin,
    program_shutter_rotation_direction: al < admin,
    radio_code: al < user,
    set_mode: al < user,
    debug: al < expert,
    ota: al < user,
  });
}

  // return a promise
  function copyToClipboard(textToCopy) {
    // navigator clipboard api needs a secure context (https)
    if (navigator.clipboard && window.isSecureContext) {
        // navigator clipboard api method'
        return navigator.clipboard.writeText(textToCopy);
    } else {
        // text area method
        let textArea = document.createElement("textarea");
        textArea.value = textToCopy;
        // make the textarea out of viewport
        textArea.style.position = "fixed";
        textArea.style.left = "-999999px";
        textArea.style.top = "-999999px";
        document.body.appendChild(textArea);
        textArea.focus();
        textArea.select();
        return new Promise((res, rej) => {
            // here the magic happens
            document.execCommand('copy') ? res() : rej();
            textArea.remove();
        });
    }
}

export function textToClipboard(txt) {
  copyToClipboard(txt)
    .then(() => console.log('text copied !'))
    .catch(() => console.log('error'));
}


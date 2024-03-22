"use strict";
import * as appDebug from "../app/app_debug.js";
import * as httpResp from "../app/http_resp.js";
import { G, M0, Address } from "../store/curr_shutter.js";
import { Gmu } from "../store/mcu_config.js";
import { get } from "svelte/store";
import { ws_isOpen, ws_sendObject } from "../net/conn_ws";
import { RadioCodeEnabled } from "../store/curr_shutter";

let b = 0;
export const FETCH_CONFIG = 1 << b++;
export const FETCH_CONFIG_P0 = 1 << b++;
export const FETCH_CONFIG_P1 = 1 << b++;
export const FETCH_CONFIG_P2 = 1 << b++;
export const FETCH_CONFIG_P3 = 1 << b++;
export const FETCH_AUTO = 1 << b++;
export const FETCH_POS = 1 << b++;
export const FETCH_VERSION = 1 << b++;
export const FETCH_ALIASES = 1 << b++;
export const FETCH_REPEATER_IDS = 1 << b++;
export const FETCH_ALIASES_START_PAIRING = 1 << b++;
export const FETCH_ALIASES_START_UNPAIRING = 1 << b++;
export const FETCH_SHUTTER_PREFS = 1 << b++;
export const FETCH_GMU = 1 << b++;
export const FETCH_GIT_TAGS = 1 << b++;
export const FETCH_SHUTTER_NAME = 1 << b++;
export const FETCH_SHUTTER_NAMES = 1 << b++;
export const FETCH_ALL_POS = 1 << b++;
export const FETCH_BOOT_COUNT = 1 << b++;
export const FETCH_CONFIG_GPIO_STRING = 1 << b++;
export const FETCH_ERROR_MASK = 1 << b++;
export const FETCH_CC1101_CONFIG = 1 << b++;
export const FETCH_SETTINGS_ALL = 1 << b++;

const FETCHES_REPLY_BY_WS = 0;

const MAX_RETRY_COUNT = 3;

export const CMD_URL = "/cmd.json";

let g;
G.subscribe((value) => (g = value));
let m;
M0.subscribe((value) => (m = value));


export function http_postCommand(data, state = { retry_count: 0 }) {
  http_postRequest(CMD_URL, data, state);
}

export function http_postRequest(url = "", data = {}, state = { retry_count: 0 }) {
  if (url === CMD_URL) {
   if (ws_sendObject(data))
   return;
  }
  http_postRequest_no_websocket(url, data, state);
}

export function http_postRequest_no_websocket(url = "", data = {}, state = { retry_count: 0 }) {
  appDebug.dbLog("post-json: " + JSON.stringify(data));

  const fetch_data = {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-Type": "application/json",
    },
    referrer: "no-referrer",
    body: JSON.stringify(data),
  };

  return fetch(url, fetch_data)
    .then((response) => {
      if (!response.ok) {
        console.log("error");
        if (state.retry_count++ < MAX_RETRY_COUNT) {
          return http_postRequest(url, data, state);
        }
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => httpResp.http_handleResponses(json))

    .catch((error) => {
      console.log("error: http_postRequest(): ", error);
    });
}

export function getFile(url) {
  const fetch_data = {
    method: "GET",
    cache: "no-cache",
    headers: {},
    referrer: "no-referrer",
  };

  return fetch(url, fetch_data).then((response) => {
    if (response.ok) {
      response.text().then((text) => {
        httpResp.http_handleDocResponses(url, text);
      });
    }
  });
}


export function getJson(url) {
  const fetch_data = {
    method: "GET",
    cache: "no-cache",
    headers: {},
    referrer: "no-referrer",
  };

  return fetch(url, fetch_data).then((response) => {
    if (response.ok) {
      response.json()
      .then((json) => httpResp.http_handleResponses(json))
      .catch((error) => {
        console.log("error: http_postRequest(): ", error);
      });
    }
  });
}


export function http_postShutterCommand(c = document.getElementById("send-c").value) {
  let tfmcu = { to: "tfmcu" };

  let send = {
    ...get(Address),
    c: c,
  };
  tfmcu.send = send;
  appDebug.dbLog(JSON.stringify(tfmcu));
  let url = "/cmd.json";
  appDebug.dbLog("url: " + url);
  http_postRequest(url, tfmcu);
}

let fetchMask = 0;
function async_fetchByMask() {
  let mask = fetchMask;
  fetchMask = 0;
  if (mask & FETCHES_REPLY_BY_WS && !ws_isOpen()) {
    fetchMask = mask & FETCHES_REPLY_BY_WS;
    mask &= ~FETCHES_REPLY_BY_WS;
    setTimeout(async_fetchByMask, 125);
  }
  http_fetchByMask(mask, true);
}

export function http_fetchByMask(mask, synchron) {
  if (!mask) {
    return;
  }

  if (!synchron) {
    if (!fetchMask) setTimeout(async_fetchByMask, 125);
    fetchMask |= mask;
    return;
  }

  let tfmcu = { to: "tfmcu" };

  // XXX: avoid websocket for large data, because fragmentation does not work atm
  if (mask &  FETCH_SETTINGS_ALL) {
    http_postRequest_no_websocket("/cmd.json", {"mcu":{"test-rj":"?"}});
  }

  if (mask & FETCH_CONFIG) {
    add_kv(tfmcu, "config", "all", "?");
  }
  if (mask & FETCH_CONFIG_P0) {
    add_kv(tfmcu, "config", "all", "p0s2?");
  }
  if (mask & FETCH_CONFIG_P1) {
    add_kv(tfmcu, "config", "all", "p1s2?");
  }
  if (mask & FETCH_CONFIG_P2) {
    add_kv(tfmcu, "config", "all", "p2s2?");
  }
  if (mask & FETCH_CONFIG_P3) {
    add_kv(tfmcu, "config", "all", "p3s2?");
  }

  if (mask & FETCH_CONFIG_GPIO_STRING) {
    add_kv(tfmcu, "config", "gpio", "$");
  }

  if (mask & FETCH_GMU) {
    add_kv(tfmcu, "config", "gm-used", "?");
  }

  if (mask & FETCH_BOOT_COUNT) {
    add_kv(tfmcu, "mcu", "boot-count", "?");
  }

  if (mask & FETCH_ERROR_MASK) {
    add_kv(tfmcu, "mcu", "error-mask", "?");
  }

  if (mask & FETCH_CC1101_CONFIG) {
    add_kv(tfmcu, "config", "cc1101-config", "?");
    add_kv(tfmcu, "mcu", "cc1101-config", "?");
  }

  if (mask & FETCH_VERSION) {
    add_kv(tfmcu, "mcu", "version", "?");
  }

  if (mask & FETCH_AUTO)
    tfmcu.auto = {
      g: g,
      m: m,
      f: "ukI",
    };

  if (mask & FETCH_POS)
    tfmcu.cmd = {
      g: g,
      m: m,
      p: "?",
    };

  if (mask & FETCH_ALL_POS)
    tfmcu.cmd = {
      p: "?",
    };

  if (mask & FETCH_ALIASES) add_kv(tfmcu, "pair", "c", "read_all");

  if (mask & (FETCH_ALIASES_START_UNPAIRING | FETCH_ALIASES_START_PAIRING)) {
    add_kv(tfmcu, "pair", "g", g);
    add_kv(tfmcu, "pair", "m", m);
    add_kv(tfmcu, "pair", "a", "?");
    add_kv(tfmcu, "pair", "c", mask & FETCH_ALIASES_START_PAIRING ? "pair" : "unpair");
  }

  if (mask & FETCH_REPEATER_IDS) add_kv(tfmcu, "config", "rf-repeater", "?");

  if (mask & FETCH_SHUTTER_PREFS) {
    add_kv(tfmcu, "shpref", "g", g);
    add_kv(tfmcu, "shpref", "m", m);
    add_kv(tfmcu, "shpref", "mvut", "?");
    add_kv(tfmcu, "shpref", "mvdt", "?");
    add_kv(tfmcu, "shpref", "mvspdt", "?");
    add_kv(tfmcu, "shpref", "tag.NAME", "?");
  }

  if (mask & FETCH_SHUTTER_NAME) {
    add_kv(tfmcu, "shpref", "g", g);
    add_kv(tfmcu, "shpref", "m", m);
    add_kv(tfmcu, "shpref", "tag.NAME", "?");
  }

  if (mask & FETCH_SHUTTER_NAMES) {
    let n = 0;
    for (let g = 0; g <= 7; ++g) {
      if (get(Gmu)[g] === 0) continue;
      for (let m = 0; m <= get(Gmu)[g]; ++m) {
        let c = { shpref: { g: g, m: m, "tag.NAME": "?" } };
        setTimeout(() => {
          http_postRequest(CMD_URL, c);
        }, 1000 * ++n);
      }
    }
  }


  http_postRequest(CMD_URL, tfmcu);
  

  if (mask & FETCH_GIT_TAGS) gitTags_fetch();
}

function add_kv(root, cmd, key, val) {
  if (!(cmd in root)) root[cmd] = {};

  root[cmd][key] = val;
}

export function fetchWithTimeout(url, data, timeout) {
  return new Promise((resolve, reject) => {
    // Set timeout timer
    let timer = setTimeout(() => reject(new Error("Request timed out")), timeout);

    fetch(url, data)
      .then(
        (response) => resolve(response),
        (err) => reject(err)
      )
      .finally(() => clearTimeout(timer));
  });
}

function gitTags_fetch() {
  const tag_url = "https://api.github.com/repos/zwiebert/tronferno-mcu-bin/tags";
  const fetch_data = {
    method: "GET",
    cache: "no-cache",
    headers: {},
    referrer: "no-referrer",
    //body: JSON.stringify(data),
  };
  return fetch(tag_url, fetch_data)
    .then((response) => {
      if (!response.ok) {
        console.log("error");
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => httpResp.gitTags_handleResponse(json))

    .catch((error) => {
      console.log("error: httpFetch.http_postRequest(): ", error);
    });
}

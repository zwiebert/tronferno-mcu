'use strict';
import * as appDebug from './app_debug.js';
import * as httpResp from './http_resp.js';
import { G, M } from './store/curr_shutter.js';

export const FETCH_CONFIG = 1;
export const FETCH_AUTO = 2;
export const FETCH_POS = 4;
export const FETCH_VERSION = 8;
export const FETCH_ALIASES = 16;
export const FETCH_ALIASES_START_PAIRING = 32;
export const FETCH_ALIASES_START_UNPAIRING = 64;
export const FETCH_SHUTTER_PREFS = 128;
export const FETCH_GMU = 256;
export const FETCH_GIT_TAGS = 512;
export const FETCH_SHUTTER_NAME = 1024;
export const FETCH_ALL_POS = 2048;


const MAX_RETRY_COUNT = 3;

export function http_postRequest(url = '', data = {}, state = { retry_count:0 }) {
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

    .then(response => {
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

export function http_postDocRequest(name) {
  let url = '/doc';
  // Default options are marked with *
  return fetch(url, {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-Type": "text/plain",
    },
    referrer: "no-referrer",
    body: name,
  })
    .then(response => {
      if (response.ok) {
        response.text().then(text => {
          httpResp.http_handleDocResponses(name, text);
        });
      }
    });
}

export function http_postShutterCommand(c = document.getElementById('send-c').value) {
  let tfmcu = { to: "tfmcu" };
  let g = G.get().toString();
  let m = M.get().toString();

  let send = {
    g: g,
    m: m,
    c: c,
  };
  tfmcu.send = send;
  appDebug.dbLog(JSON.stringify(tfmcu));
  let url = '/cmd.json';
  appDebug.dbLog("url: " + url);
  http_postRequest(url, tfmcu);
}

export function http_fetchByMask(mask) {
  let tfmcu = { to: "tfmcu" };
  let g = G.get();
  let m = M.get();

  if (mask & FETCH_CONFIG)
    tfmcu.config = { all: "?" };

  if (mask & FETCH_GMU)
    tfmcu.config = { 'gm-used': "?" };

  if (mask & FETCH_VERSION)
    tfmcu.mcu = { version: "?", 'boot-count': '?' };


  if (mask & FETCH_AUTO)
    tfmcu.auto = {
      g: g,
      m: m,
      f: "uki",
    };

  if (mask & FETCH_POS)
    tfmcu.send = {
      g: g,
      m: m,
      p: "?",
    };

    if (mask & FETCH_ALL_POS)
    tfmcu.cmd = {
      g: g,
      m: m,
      p: "?",
    };

  if (mask & FETCH_ALIASES)
    tfmcu.pair = {
      c: "read_all"
    };

  if (mask & FETCH_ALIASES_START_PAIRING)
    tfmcu.pair = {
      a: "?",
      g: g,
      m: m,
      c: "pair"
    };
  if (mask & FETCH_ALIASES_START_UNPAIRING)
    tfmcu.pair = {
      a: "?",
      g: g,
      m: m,
      c: "unpair"
    };

  if (mask & FETCH_SHUTTER_PREFS) {
    if (!('shpref' in tfmcu))
      tfmcu.shpref = {};
    Object.assign(tfmcu.shpref, {
      g: g,
      m: m,
      mvut: '?', mvdt: '?', mvspdt: '?', 'tag.NAME': '?',
    });
  }

  if (mask & FETCH_SHUTTER_NAME) {
    if (!('shpref' in tfmcu))
      tfmcu.shpref = {};
    Object.assign(tfmcu.shpref, {
      g: g,
      m: m,
      'tag.NAME': '?',
    });
  }

  let url = '/cmd.json';
  http_postRequest(url, tfmcu);

  if (mask & FETCH_GIT_TAGS)
    gitTags_fetch();

}



export function fetchWithTimeout(url, data, timeout) {
  return new Promise((resolve, reject) => {
    // Set timeout timer
    let timer = setTimeout(
      () => reject(new Error('Request timed out')),
      timeout
    );

    fetch(url, data).then(
      response => resolve(response),
      err => reject(err)
    ).finally(() => clearTimeout(timer));
  });
}

function gitTags_fetch() {
  const tag_url =
    "https://api.github.com/repos/zwiebert/tronferno-mcu-bin/tags";
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

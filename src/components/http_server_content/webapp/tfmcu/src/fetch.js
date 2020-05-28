'use strict';
import {dbLog} from './misc.js';
import * as as from './app_state.js';
import * as ota from './netota.js';

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


export function http_postRequest(url = '', data = {}) {
  dbLog("post-json: "+JSON.stringify(data));

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
      if(!response.ok) {
        console.log("error");
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => as.ast.http_handleResponses(json))

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
      if(response.ok) {
        response.text().then(text => {
          as.ast.http_handleDocResponses(name, text);
        });
      }
    });
}

export function http_postShutterCommand(c=document.getElementById('send-c').value) {
  var tfmcu = {to:"tfmcu"};
  let g = as.ast.g.toString();
  let m = as.ast.m.toString();

  var send = {
    g: g,
    m: m,
    c: c,
  };
  tfmcu.send = send;
  dbLog(JSON.stringify(tfmcu));
  var url = '/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, tfmcu);
}

 export function http_fetchByMask(mask) {
    let tfmcu = {to:"tfmcu"};

    if (mask & FETCH_CONFIG)
      tfmcu.config = { all: "?" };

    if (mask & FETCH_GMU)
      tfmcu.config = { 'gm-used': "?" };

    if (mask & FETCH_VERSION)
      tfmcu.mcu = { version:"?", 'boot-count':'?' };


    if (mask & FETCH_AUTO)
      tfmcu.auto = {
        g: as.ast.g,
        m: as.ast.m,
        f: "uki",
      };

    if (mask & FETCH_POS)
      tfmcu.send = {
        g: as.ast.g,
        m: as.ast.m,
        p: "?",
      };

    if (mask & FETCH_ALIASES)
      tfmcu.pair = {
        c: "read_all"
      };

    if (mask & FETCH_ALIASES_START_PAIRING)
      tfmcu.pair = {
        a: "?",
        g: as.ast.g,
        m: as.ast.m,
        c: "pair"
      };
    if (mask & FETCH_ALIASES_START_UNPAIRING)
      tfmcu.pair = {
        a: "?",
        g: as.ast.g,
        m: as.ast.m,
        c: "unpair"
      };

    if (mask & FETCH_SHUTTER_PREFS) {
      if (!('shpref' in tfmcu))
        tfmcu.shpref = {};
      Object.assign(tfmcu.shpref, {
        g: as.ast.g,
        m: as.ast.m,
        mvut: '?', mvdt: '?', mvspdt: '?', 'tag.NAME':'?',
      });
    }

    if (mask & FETCH_SHUTTER_NAME) {
      if (!('shpref' in tfmcu))
        tfmcu.shpref = {};
      Object.assign(tfmcu.shpref, {
        g: as.ast.g,
        m: as.ast.m,
        'tag.NAME':'?',
      });
    }

    let url = '/cmd.json';
    http_postRequest(url, tfmcu);

    if (mask & FETCH_GIT_TAGS)
      ota.gitTags_fetch();
  
}



export function fetchWithTimeout( url, data, timeout ) {
  return new Promise( (resolve, reject) => {
    // Set timeout timer
    let timer = setTimeout(
      () => reject( new Error('Request timed out') ),
      timeout
    );

    fetch(url, data).then(
      response => resolve( response ),
      err => reject( err )
    ).finally( () => clearTimeout(timer) );
  });
}



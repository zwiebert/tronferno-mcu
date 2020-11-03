"use strict";
import * as appDebug from "app/app_debug.js";
import * as httpResp from "app/http_resp.js";
import { McuWebsocket } from "stores/app_state.js";

let isOpen = false;

export function websocket() {
  let ws = new WebSocket("ws://" + window.location.host + "/ws");
  // eslint-disable-next-line no-unused-vars
  ws.onopen = (evt) => {
    ws.send(JSON.stringify({ to: "tfmcu", cmd: { p: "?" } }));
    isOpen = true;
  };
  ws.onmessage = (evt) => {
    let json = evt.data;
    let obj = JSON.parse(json);
    httpResp.http_handleResponses(obj);
  };
  ws.onclose = (evt) => {
    appDebug.dbLog(evt.reason);
    setTimeout(function () {
      websocket();
    }, 1000);
    isOpen = false;
  };
  ws.onerror = (err) => {
    appDebug.dbLog(err.msg);
    ws.close();
  };

  McuWebsocket.set(ws);
  return ws;
}

export function ws_isOpen() { return isOpen; }
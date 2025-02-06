"use strict";
import * as appDebug from "../app/app_debug.js";
import * as httpResp from "../app/http_resp.js";
import { McuWebsocket } from "../store/app_state.js";

let isOpen = false;
let ws = null;

export function websocket() {
  ws = new WebSocket("ws://" + window.location.host + "/ws");
  // eslint-disable-next-line no-unused-vars
  ws.onopen = (evt) => {
    // send something to open the websocket connection on the other end
    console.log("ws.onopen");
    ws.send(JSON.stringify({from:"wapp",to:"tfmcu",open_websocket:{}}));
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
export function ws_sendObject(obj) {
  if (!isOpen || !ws) {
    return false;
  }

  const json = JSON.stringify(obj);
  console.log("ws.send: ", json);
  ws.send(json);
  return true;
}
export function ws_keepAlive() {
  if (!isOpen || !ws) {
    return false;
  }
  ws.send("");
  return true;
}
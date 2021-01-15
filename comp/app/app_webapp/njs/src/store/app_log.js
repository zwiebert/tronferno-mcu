"use strict";
import { writable, derived } from "svelte/store";
import { PersistentOptionStore, PersistentValStore } from "./custom_stores.js";

export const AppLog = writable([]);

export const AppLogTxt = derived(AppLog, (appLog) => {
  let r = "";
  for (let msg of appLog) {
    if ("log" in msg) {
      r += msg.log.tag + ": " + msg.log.txt + "\n";
    } else if ("rc" in msg) {
        r += JSON.stringify(msg) + "\n";
    }
  }
  return r;
});

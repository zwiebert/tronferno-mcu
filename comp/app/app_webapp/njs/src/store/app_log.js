'use strict';
import { writable, derived } from "svelte/store";
import { PersistentOptionStore, PersistentValStore } from "./custom_stores.js";

export const AppLog = writable([]);


export const AppLogTxt = derived(AppLog, (appLog) => {
    let r = "";
    for(let msg of appLog) {
       r += msg.tag + ": " + msg.txt + "\n";
    }
    return r;
});


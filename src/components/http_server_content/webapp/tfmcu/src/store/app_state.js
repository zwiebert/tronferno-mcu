'use strict';
import { writable } from 'svelte/store';

export const TabIdx = writable(Number.parseInt((localStorage.getItem("tab_idx") || "1"), 10));
TabIdx.subscribe(value => localStorage.setItem("tab_idx", value ? value.toString() : "1"));


export const GmcFetch = writable(0);

export const McuWebsocket = writable(0);
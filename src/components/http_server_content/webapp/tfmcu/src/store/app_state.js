'use strict';
import { writable } from 'svelte/store';

export const TabIdx = writable(Number.parseInt((localStorage.getItem("tab_idx") || "0"), 10));
TabIdx.subscribe(value => localStorage.setItem("tab_idx", value ? value.toString() : "0"));

export const McuWebsocket = writable(0);
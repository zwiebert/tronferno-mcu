'use strict';
import { writable, derived } from 'svelte/store';
import { PersistentOptionStore, PersistentValStore } from './custom_stores.js';

export const TabIdx = PersistentOptionStore("TabIdx");

export const McuWebsocket = writable(0);

export const ReloadProgress = writable(0);

export const Verbose = PersistentValStore("AppVerbose", 1);

export const ShowHelp = derived(TabIdx, opts => opts.showHelp ? "help-text-v1" : "help-text-v0");
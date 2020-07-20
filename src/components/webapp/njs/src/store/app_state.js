'use strict';
import { writable } from 'svelte/store';
import { PersistentOptionStore, PersistentValStore } from './custom_stores.js';

export const TabIdx = PersistentOptionStore("TabIdx");

export const McuWebsocket = writable(0);

export const ReloadProgress = writable(0);

export const Verbose = PersistentValStore("AppVerbose", 1);
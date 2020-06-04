'use strict';
import { writable } from 'svelte/store';
import { PersistentIntStore } from './custom_stores.js';

export const TabIdx = PersistentIntStore("tab_idx");

export const McuWebsocket = writable(0);
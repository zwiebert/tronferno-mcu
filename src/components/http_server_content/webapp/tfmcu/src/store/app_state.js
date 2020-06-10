'use strict';
import { writable } from 'svelte/store';
import { PersistentOptionStore } from './custom_stores.js';

export const TabIdx = PersistentOptionStore("TabIdx");

export const McuWebsocket = writable(0);
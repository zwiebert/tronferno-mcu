'use strict';
import { writable, derived } from 'svelte/store';
import { OptionStore, PersistentValStore} from './custom_stores.js';


export const Pcts = OptionStore();
export const Gmu = PersistentValStore("gmu", [0, 7, 7, 7, 7, 7, 7, 7] );
export const McuConfig = OptionStore();

export const McuConfigKeys = derived(McuConfig, mcuConfig => Object.keys(mcuConfig) || []); // XXX: ordering of keys?


export const GmuMaxM = derived(Gmu, gmu => { let result = 0; gmu.forEach(val => { if (val > result) {result = val;}});  return result; });
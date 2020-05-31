import { writable, derived } from 'svelte/store';
'use strict';
import * as appState from '../app_state.svelte';
import {Gmu} from './mcu_config.js';
import {Pcts,Prefs,Autos} from './shutters.js';

let g = 0;
let m = 0;
let gmu = [];


export const G = (() => {
	const { subscribe, set } = writable(Number.parseInt((localStorage.getItem("group") || "0"), 10));

	return {
		subscribe,
		increment: () => {
			if (!appState.ast)
				return;
			for (let i = g + 1; i <= 7; ++i) {
				if (gmu[i] > 0) {
					set(i);
					return;
				}
			}
			set(0);
		},
		set: (value) => {
			set(value);
			if (value !== 0 && m > gmu[value]) {
				M.set(gmu[value]);
			}
		},
		get: () => g,
		reset: () => set(0)
	};
})();

export const M = (() => {
	const { subscribe, set } = writable(Number.parseInt((localStorage.getItem("member") || "0"), 10));

	return {
		subscribe,
		increment: () => {
			if (g && appState.ast)
				set((m >= gmu[g]) ? 0 : m + 1);
		},
		set: (val) => { if (g) set(val); return !g; },
		get: () => g ? m : 0,
		reset: () => set(0)
	};
})();

export const GM = derived([G, M], ([g, m]) =>  g.toString() + m.toString());
export const Auto = derived([GM, Autos], ([gm, autos]) =>  autos["auto"+gm] || {});
export const Pct = derived([GM, Pcts], ([gm, pcts]) =>  pcts[gm]);
export const Pref = derived([GM, Prefs], ([gm, prefs]) =>  prefs["shs"+gm] || {});
export const Name = derived(Pref, pref =>  pref['tag.NAME'] || '');
export const PrefMvut = derived(Pref, pref =>  pref.mvut || 0);
export const PrefMvdt = derived(Pref, pref =>  pref.mvdt || 0);
export const PrefMvspdt = derived(Pref, pref =>  pref.mvspdt || 0);


G.subscribe(value => { localStorage.setItem("group", value ? value.toString() : "0"); g = value;});
M.subscribe(value => { localStorage.setItem("member", value ? value.toString() : "0"); m = value;});
Gmu.subscribe(value => gmu = value);

//const store = writable(localStorage.getItem("store") || "");
//store.subscribe(val => localStorage.setItem("store", val));


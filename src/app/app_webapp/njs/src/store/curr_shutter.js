'use strict';
import { derived } from 'svelte/store';
import { Gmu } from './mcu_config.js';
import { Autos, Pcts, Prefs, Names } from './shutters.js';
import { PersistentIntStore } from './custom_stores.js';

export const G = PersistentIntStore("group");
export const M = PersistentIntStore("member");
export const M0 = derived([G, M, Gmu], ([g, m, gmu]) => g === 0 ? 0 : Math.min(gmu[g], m));
export const GM = derived([G, M0], ([g, m]) => g.toString() + m.toString());
export const Auto = derived([GM, Autos], ([gm, autos]) => autos["auto" + gm] || {});
export const Pct = derived([GM, Pcts], ([gm, pcts]) => pcts[gm]);
export const Pref = derived([GM, Prefs], ([gm, prefs]) => prefs["shs" + gm] || {});
export const Name = derived([GM, Names], ([gm, names]) => names[gm] || '');
export const PrefMvut = derived(Pref, pref => pref.mvut || 0);
export const PrefMvdt = derived(Pref, pref => pref.mvdt || 0);
export const PrefMvspdt = derived(Pref, pref => pref.mvspdt || 0);

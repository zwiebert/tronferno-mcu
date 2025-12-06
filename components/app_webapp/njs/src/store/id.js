"use strict";
import { writable,derived } from "svelte/store";
import { PersistentOptionStore} from "./custom_stores.js";

export const SelectedId = writable("");

export const SelectedIdIsValid = derived([SelectedId], ([id]) =>  /^[12][0-9A-Fa-f]{5}$/.test(id));
export const SelectedIdIsValidPlain = derived([SelectedId], ([id]) =>  /1[0-9A-Fa-f]{5}$/.test(id));
export const SelectedIdIsValidSunSensor = derived([SelectedId], ([id]) =>  /2[0-9A-Fa-f]{5}$/.test(id));

export const TxNames = PersistentOptionStore("txNames");
export const TxName = derived([SelectedId, TxNames], ([id, names]) => names[id] || "");

export function is_valid_fernotron_id(id) {
 return /^[12][0-9A-Fa-f]{5}$/.test(id);
}

export function is_valid_fernotron_sunsensor_id(id) {
 return /2[0-9A-Fa-f]{5}$/.test(id);
}
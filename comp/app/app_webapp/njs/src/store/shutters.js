//import { writable } from 'svelte/store';
import { OptionStore, PersistentOptionStore} from "./custom_stores.js";


export const Pcts = OptionStore();
export const Prefs = OptionStore();
export const Aliases = OptionStore();
export const Autos = OptionStore();
export const Names = PersistentOptionStore("shutterNames");
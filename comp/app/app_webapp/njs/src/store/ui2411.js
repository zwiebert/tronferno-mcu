import { writable, readable } from 'svelte/store';
//import { OptionStore, PersistentOptionStore} from "./custom_stores.js";


export const MenuCount = writable(0);
export const MenuCountMax = readable(1);

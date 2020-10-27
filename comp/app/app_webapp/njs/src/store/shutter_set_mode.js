import { writable } from 'svelte/store';
//import { OptionStore, PersistentOptionStore} from './custom_stores.js';


export const SetModeSrcAddress = writable("101234");
export const SetModeSrcMotorCode = writable("01234");
export const SetModeSrcRadio = writable(0);

export const SetMode_isInSetMode = writable(0);

export const SetModeDstAddress = writable("101234");
export const SetModeDstG = writable(1);
export const SetModeDstM = writable(1);
export const SetModeDstRadio = writable(0);

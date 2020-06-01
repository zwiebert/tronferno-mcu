'use strict';
import { writable } from 'svelte/store';
import { OptionStore} from './custom_stores.js';


export const Pcts = OptionStore();
export const Gmu = writable( [0, 1, 2, 3, 4, 5, 6, 7] );
export const McuConfig = OptionStore();


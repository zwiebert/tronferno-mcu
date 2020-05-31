'use strict';
import { writable } from 'svelte/store';

export function OptionStore() {
	const { subscribe, set, update } = writable({});

	return {
		subscribe,
		update: obj => update(old => {
           return Object.assign(old, obj);
        }),
        set: (value) => set(value),
		reset: () => set({})
	};
}
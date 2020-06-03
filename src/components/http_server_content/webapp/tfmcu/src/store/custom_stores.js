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


function read_storage(name) {
	return JSON.parse(localStorage.getItem(name) || '{}');
}
function save_storage(name, obj) {
	localStorage.setItem(name, JSON.stringify(obj));
}

export function PersistentOptionStore(name) {

	const { subscribe, set, update } = writable(read_storage(name));

	function my_set(value) {
		save_storage(name, value);
		set(value);
	}

	let result = {
		subscribe,
		update: obj => update(old => {
			let new_obj = Object.assign(old, obj);
			save_storage(name, new_obj);
			return new_obj;
		}),
		set: my_set,
		reset: () => my_set({})
	};

	return result;
}


function read_storage_int(name) {
	return Number.parseInt((localStorage.getItem(name) || "0"), 10);
}
function save_storage_int(name, value) {
	localStorage.setItem(name, value ? value.toString() : "0");
}

export function PersistentIntStore(name) {

	const { subscribe, set } = writable(read_storage_int(name));

	function my_set(value) {
		save_storage_int(name, value);
		set(value);
	}

	let result = {
		subscribe,
		set: my_set,
		reset: () => my_set(0)
	};

	return result;
}
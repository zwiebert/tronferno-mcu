"use strict";
import { writable, get } from "svelte/store";

export function OptionStore() {
  const { subscribe, set, update } = writable({});

  return {
    subscribe,
    update: (obj) =>
      update((old) => {
        return Object.assign(old, obj);
      }),
    set: (value) => set(value),
    reset: () => set({}),
    remove: (el) =>
      update((old) => {
        delete old[el];
        return old;
      }),
  };
}

function read_storage_obj(name) {
  let obj = JSON.parse(localStorage.getItem(name) || "{}");
  if (typeof obj === "object" && !Array.isArray(obj)) {
    return obj;
  }
  return {};
}
function save_storage_obj(name, obj) {
  localStorage.setItem(name, JSON.stringify(obj));
}

export function PersistentOptionStore(name) {
  const { subscribe, set, update } = writable(read_storage_obj(name));

  function my_set(value) {
    save_storage_obj(name, value);
    set(value);
  }

  let result = {
    subscribe,
    update: (obj) =>
      update((old) => {
        let new_obj = Object.assign(old, obj);
        save_storage_obj(name, new_obj);
        return new_obj;
      }),
    set: my_set,
    reset: () => my_set({}),
    remove: (el) =>
      update((old) => {
        delete old[el];
        save_storage_obj(name, old);
        return old;
      }),
  };

  return result;
}

function read_storage_int(name) {
  return Number.parseInt(localStorage.getItem(name) || "0", 10);
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
    reset: () => my_set(0),
  };

  return result;
}

function read_storage_string(name) {
  return localStorage.getItem(name) || "";
}
function save_storage_string(name, value) {
  localStorage.setItem(name, value ? value.toString() : "0");
}

export function PersistentStringStore(name) {
  const { subscribe, set } = writable(read_storage_string(name));

  function my_set(value) {
    save_storage_string(name, value);
    set(value);
  }

  let result = {
    subscribe,
    set: my_set,
    reset: () => my_set(0),
  };

  return result;
}

function read_storage_val(name, def_value) {
  try {
    let obj = JSON.parse(localStorage.getItem(name));
    if (obj !== null) return obj.value;
    return def_value;
  } catch (e) {
    return def_value;
  }
}
function save_storage_val(name, value) {
  localStorage.setItem(name, JSON.stringify({ value: value }));
}

export function PersistentValStore(name, def_value) {
  let ws = writable(read_storage_val(name, def_value));

  function my_set(value) {
    save_storage_val(name, value);
    ws.set(value);
  }
  function my_update(f) {
    ws.update(f);
    save_storage_val(name, get(ws));
  }

  let result = {
	delete: () => localStorage.removeItem(name),
    subscribe: ws.subscribe,
    set: my_set,
    update: my_update,
  };

  return result;
}

export function PersistentSetStore(name, def_value) {
  let pvs = PersistentValStore(name, [...def_value]);
  if (!Array.isArray(get(pvs))) {
    pvs.delete(name);
    pvs = PersistentValStore(name, [...def_value]);
  }
  let ws = writable(new Set(get(pvs)));

  return {
    delete: pvs.delete,
    subscribe: ws.subscribe,
    set: (value) => {
      pvs.set([...value]);
      ws.set(value);
    },
    update: (value) => {
      ws.update(value);
      pvs.set([...get(ws)]);
    },
  };
}

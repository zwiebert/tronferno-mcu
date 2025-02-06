"use strict";
import { writable } from "svelte/store";
import { PersistentSetStore } from "./custom_stores.js";

export const Pras = writable(null);
export const ReceivedAddresses = PersistentSetStore("ReceivedAddresses", new Set([]));

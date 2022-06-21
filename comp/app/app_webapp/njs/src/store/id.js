"use strict";
import { writable,derived } from "svelte/store";

export const SelectedId = writable("");

export const SelectedIdIsValid = derived([SelectedId], ([id]) =>  /^[12]0[0-9A-Fa-f]{4}$/.test(id));


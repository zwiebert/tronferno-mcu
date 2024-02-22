"use strict";
import { writable, derived } from "svelte/store";
import { PersistentOptionStore, PersistentValStore, PersistentIntStore } from "./custom_stores.js";

export const TabIdx = PersistentOptionStore("TabIdx");

export const McuWebsocket = writable(0);

export const ReloadProgress = writable(0);

export const Verbose = PersistentValStore("AppVerbose", 1);

export const ShowHelp = derived(TabIdx, (opts) => (opts.showHelp ? "help-text-v1" : "help-text-v0"));

export const GuiAcc = writable();
export const GuiUserLevel = PersistentIntStore("GuiUserLevel", 10);

export const ShowAutomatic = PersistentIntStore("ShowAutomatic", true); // show auto in up/down pane


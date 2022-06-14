"use strict";
import { OptionStore, PersistentOptionStore } from "./custom_stores.js";

export const Sep = OptionStore();
export const SepMode = PersistentOptionStore("SepMode", {enabled: false, auth_key:0,});
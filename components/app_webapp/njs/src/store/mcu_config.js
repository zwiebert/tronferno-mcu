"use strict";
import { _ } from "../services/i18n";
import { writable, derived } from "svelte/store";
import { OptionStore, PersistentValStore } from "./custom_stores.js";

export const Pcts = OptionStore();
export const Gmu = PersistentValStore("gmu", [0, 7, 7, 7, 7, 7, 7, 7]);
export const McuConfig = OptionStore();
export const Backup = OptionStore();
export const McuSettings = OptionStore();
export const Cuas = OptionStore();

export const McuConfigKeys = derived(McuConfig, (mcuConfig) => Object.keys(mcuConfig).filter((key) => key !== "gpio") || []); // XXX: ordering of keys?

export const GmuMaxM = derived(Gmu, (gmu) => {
  let result = 0;
  gmu.forEach((val) => {
    if (val > result) {
      result = val;
    }
  });
  return result;
});

export const McuGpiosFree = derived(McuConfig, (cfg) => {
  const gpios = cfg.gpio || "";
  let result = [];

  for (let i = 0; i < gpios.length; ++i) {
    if (gpios.charAt(i) !== "d") continue;
    result.push(i);
  }
  return result;
});


export const McuConfigNames = derived(McuConfigKeys, (keys) => {
  let result = {};
  for (let key of keys) {
     result[key] = _("mcuConfigNames."+key) || key;
  }
  return result;
});



export const Cc1101Config = writable("");
export const Cc1101Status = writable("");


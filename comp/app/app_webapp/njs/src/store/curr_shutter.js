"use strict";
import { writable, derived, get } from "svelte/store";
import { Gmu } from "./mcu_config.js";
import { Autos, Pcts, Prefs, Names } from "./shutters.js";
import { PersistentIntStore, PersistentStringStore } from "./custom_stores.js";

export const G = PersistentIntStore("group");
export const M = PersistentIntStore("member");
export const M0 = derived([G, M, Gmu], ([g, m, gmu]) => (g === 0 ? 0 : Math.min(gmu[g], m)));
export const GM = derived([G, M0], ([g, m]) => g.toString() + m.toString());
export const GMH = derived([G, M0], ([g, m]) => (g ? g.toString() + (m ? m.toString() : "A") : "A"));
export const Auto = derived([GM, Autos], ([gm, autos]) => autos["auto" + gm] || {});
export const AutoSunEnabled = derived([Auto], ([auto]) =>   "f" in auto && auto.f.indexOf("S") >= 0);
export const Pct = derived([GM, Pcts], ([gm, pcts]) => pcts[gm]);
export const Pref = derived([GM, Prefs], ([gm, prefs]) => prefs["shs" + gm] || {});
export const Name = derived([GM, Names], ([gm, names]) => names[gm] || "");
export const PrefMvut = derived(Pref, (pref) => pref.mvut || 0);
export const PrefMvdt = derived(Pref, (pref) => pref.mvdt || 0);
export const PrefMvspdt = derived(Pref, (pref) => pref.mvspdt || 0);
export const RadioCode = PersistentStringStore("radio_code", "");
export const RadioCodeEnabled = writable(false);
export const Address = derived([G, M, RadioCodeEnabled, RadioCode], ([g, m, rce, rc]) => (!rce ? { g: g, m: m } : { a: rc }));

export const AutoData = derived([GM, Autos], ([gm, autos]) => {
  let dst = {
    hasDaily: false,
    hasWeekly: false,
    hasAstro: false,
    isRandom: false,
    isSun: false,
    isManual: false,
    astroToday: "",
    dailyUp: "",
    dailyDown: "",
    weekly: "",
    astro: "0",
  };
  const src = autos["auto" + gm];
  if (!src) return dst;

  if ("astro" in src) dst.astro = src.astro;

  let f = "f" in src ? src.f : "";
  dst.hasDaily = f.indexOf("D") >= 0;
  dst.hasWeekly = f.indexOf("W") >= 0;
  dst.hasAstro = f.indexOf("A") >= 0;
  dst.isRandom = f.indexOf("R") >= 0;
  dst.isSun = f.indexOf("S") >= 0;
  dst.isManual = f.indexOf("M") >= 0;

  if ("daily" in src) {
    let d = src.daily;
    let l = src.daily.length;
    dst.dailyUp = d.startsWith("-") ? "" : d.substring(0, 2) + ":" + d.substring(2, 4);
    dst.dailyDown = d.endsWith("-") ? "" : d.substring(l - 4, l - 2) + ":" + d.substring(l - 2);
  }

  dst.weekly = src.weekly || "";
  dst.astroToday =
    "asmin" in src
      ? Math.floor(src.asmin / 60)
          .toString()
          .padStart(2, "0") +
        ":" +
        (src.asmin % 60).toString().padStart(2, "0")
      : "";

  return dst;
});

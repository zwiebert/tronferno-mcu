'use strict';
import { writable, derived } from "svelte/store";

export const McuBootCount = writable(0);
export const McuGitTagNames = writable([]);
export const McuFirmwareVersion = writable("");
export const McuChipId = writable("");
export const McuFirmwareBuildDate = writable("");

export const McuFirmwareUpdState = writable(0);
export const McuFirmwareUpdProgress = writable(0);
export const McuFirmwareUpdChip = writable('');


const reBeta = /\d+\.\d+\.\d+\.\d+/;

export const McuGitTagNamesMaster = derived(McuGitTagNames, (allNames) => {
   return allNames.filter((item) => {
      return !item.match(reBeta);
   });
});

export const McuGitTagNamesBeta = derived(McuGitTagNames, (allNames) => {
    return allNames.filter((item) => {
       return item.match(reBeta);
    });
 });

 export const McuGitTagNameLatestMaster = derived(McuGitTagNamesMaster, (masterNames) => {
    return (masterNames.length) ? masterNames[0] : "";
 });

 export const McuGitTagNameLatestBeta = derived(McuGitTagNamesBeta, (betaNames) => {
    return (betaNames.length) ? betaNames[0] : "";
 });
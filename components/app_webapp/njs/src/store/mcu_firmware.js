'use strict';
import { writable, derived } from "svelte/store";

export const McuBootCount = writable(0);
export const McuErrorBits = writable(0);
export const McuError_cc1101_gdo2_nc = derived(McuErrorBits, (errbits) => { return (errbits & 0x01) !== 0; }) 
export const McuError_cc1101_init = derived(McuErrorBits, (errbits) => { return (errbits & 0x02) !== 0; }) 
export const McuGitTagNames = writable([]);
export const McuFirmwareVersion = writable("");
export const McuFirmwareVersionNumber = derived(McuFirmwareVersion, (versionString) => {
   if (!versionString) {
     return "";
   }
   return versionString.substring(14);
});
export const McuFirmwareReleaseStatus = derived(McuFirmwareVersionNumber, (versionNumber) => {
   if (versionNumber.match(/-/g)) { return "none"; }
   if (versionNumber.match(/\d+\.\d+\.\d+\.\d+/)) { return "beta"; }
   if (versionNumber.match(/\d+\.\d+\.\d+/)) { return "release"; }
   return "none";
});
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
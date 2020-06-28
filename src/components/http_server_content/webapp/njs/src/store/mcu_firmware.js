'use strict';
import { writable } from 'svelte/store';

export const McuBootCount = writable(0);
export const McuGitTagNames = writable([]);
export const McuFirmwareVersion = writable("");
export const McuChipId = writable("");
export const McuFirmwareBuildDate = writable("");
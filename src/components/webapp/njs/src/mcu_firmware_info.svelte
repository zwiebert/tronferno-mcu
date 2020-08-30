<script>
  "use strict";
  import {
    McuBootCount,
    McuFirmwareBuildDate,
    McuChipId,
    McuFirmwareVersion,
  } from "./store/mcu_firmware.js";
  import * as httpFetch from "./fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION|httpFetch.FETCH_BOOT_COUNT);
  });

</script>

<ul>
  <li>MCU type: {$McuChipId}</li>
  <li>Version: {$McuFirmwareVersion}</li>
  <li>Build Date: {$McuFirmwareBuildDate}</li>
  <li>Boot Count: {$McuBootCount}</li>
</ul>

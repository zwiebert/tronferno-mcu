<script>
  "use strict";
  import { _, json } from "../services/i18n";
  import {
    McuBootCount,
    McuErrorBits,
    McuError_cc1101_gdo2_nc,
    McuError_cc1101_init,
    McuFirmwareBuildDate,
    McuChipId,
    McuFirmwareVersion,
  } from "../store/mcu_firmware.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION | httpFetch.FETCH_BOOT_COUNT | httpFetch.FETCH_ERROR_MASK);
  });

  let nu = $derived($json("notifyUser"));
</script>

<ul>
  <li>MCU type: {$McuChipId}</li>
  <li>Version: {$McuFirmwareVersion}</li>
  <li>Build Date: {$McuFirmwareBuildDate}</li>
  <li>Boot Count: {$McuBootCount}</li>
  <li>Error-Bits: {$McuErrorBits}</li>
  {#if $McuErrorBits}
    <ul>
      {#if $McuError_cc1101_gdo2_nc}
        <li><span class="bg-red-400">CC1101 GDO2 not connected.</span> Is it connected to the configured GPIO?</li>
      {/if}
      {#if $McuError_cc1101_init}
        <li><span class="bg-red-400">CC1101 init failed.</span> Is the CC1101 SPI interface connected to the configured GPIOS?</li>
      {/if}
    </ul>
  {/if}
</ul>

{#if $json("notifyUser").messages.length > 0}
  <table>
    <caption class="bg-red-400">{$json("notifyUser").caption}</caption>
    <tbody>
      {#each $json("notifyUser").messages as nu}
        <tr>
          <td>{nu.date}</td>
          <td>{nu.text}</td>
        </tr>
      {/each}
    </tbody>
  </table>
{/if}

<hr />
<a href="https://github.com/zwiebert/tronferno-mcu/commits/develop">develop-branch commit log on GitHub </a><br />
<a href="https://github.com/zwiebert/tronferno-mcu/commits/master">master-branch commit log on GitHub</a>

<style lang="scss">
  @use "../styles/app.scss" as *;
  table,
  th,
  td {
    border-color: $color_border_main_area;
    border-style: solid;
    border-width: 1px;
    border-radius: 0.75rem;
    overflow: hidden;
    border-collapse: collapse;
    margin: 0rem;
    padding: 0rem 0.25rem;
    border-spacing: 0;
  }
</style>

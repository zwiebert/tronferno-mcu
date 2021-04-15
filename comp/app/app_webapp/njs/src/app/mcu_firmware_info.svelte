<script>
  "use strict";
  import { _ } from "services/i18n";
  import { McuBootCount, McuErrorMask, McuFirmwareBuildDate, McuChipId, McuFirmwareVersion } from "stores/mcu_firmware.js";
  import * as httpFetch from "app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION | httpFetch.FETCH_BOOT_COUNT | httpFetch.FETCH_ERROR_MASK);
  });

  $: nu = $_("notifyUser");
</script>

<style type="text/scss">
  @import "../styles/app.scss";
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
    border-gap: 0;
  }
</style>


<ul>
  <li>MCU type: {$McuChipId}</li>
  <li>Version: {$McuFirmwareVersion}</li>
  <li>Build Date: {$McuFirmwareBuildDate}</li>
  <li>Boot Count: {$McuBootCount}</li>
  <li>Error Mask: {$McuErrorMask}</li>
</ul>

{#if $_("notifyUser").messages.length > 0}
<table>
  <caption class="bg-red-400">{$_("notifyUser").caption}</caption>
  {#each $_("notifyUser").messages as nu}
    <tr>
      <td>{nu.date}</td>
      <td>{nu.text}</td>
    </tr>
  {/each}
</table>
{/if}

<hr>
<a href="https://github.com/zwiebert/tronferno-mcu/commits/develop">develop-branch commit log on GitHub </a><br>
<a href="https://github.com/zwiebert/tronferno-mcu/commits/master">master-branch commit log on GitHub</a>
<script>
  import { AppLog } from "stores/app_log.js";

  export let rxonly = false;

  $: showMqtt = !rxonly;
  $: showRc = true;
  $: showSc = !rxonly;

  $: {
    $AppLog;
    let elem = document.getElementById("app_log_div");
    if (elem) elem.scrollTop = elem.scrollHeight;
  }
</script>

<div class="flex-row">
  Filters:
  <label class="bg-red-300">MQTT
    <input type="checkbox" bind:checked={showMqtt} /></label>
  <label class="bg-green-300">RF received
    <input type="checkbox" bind:checked={showRc} /></label>
  <label class="bg-blue-300">RF sent
    <input type="checkbox" bind:checked={showSc} /></label>
</div>

<div
  id="app_log_div"
  class="w-11/12 h-full border-black border-2 bg-gray-800 overflow-auto text-xs">
  <!-- {@html htmlEntities($AppLogTxt)} -->

  {#each $AppLog as msg}
    {#if showMqtt && 'log' in msg && 'mqtt_client' === msg.log.tag}
      <span class="text-red-300">{msg.date.toLocaleTimeString() + ': ' + msg.log.tag + ': ' + msg.log.txt}<br /></span>
    {:else if showRc && 'rc' in msg}
      <span
        class="text-green-300">{msg.date.toLocaleTimeString() + ': ' + 'rc: ' + msg.rc.type + ': ' + msg.rc.a + (msg.rc.type === 'central' ? '-' + msg.rc.g + msg.rc.m : '') + '-' + msg.rc.c + (msg.rc.rssi ? (':rssi=' + msg.rc.rssi) : '')}<br /></span>
    {:else if showSc && 'sc' in msg}
      <span
        class="text-blue-300">{msg.date.toLocaleTimeString() + ': ' + 'sc: ' + msg.sc.type + ': ' + msg.sc.a + (msg.sc.type === 'central' ? '-' + msg.sc.g + msg.sc.m : '') + '-' + msg.sc.c}<br /></span>
    {/if}
  {/each}
</div>

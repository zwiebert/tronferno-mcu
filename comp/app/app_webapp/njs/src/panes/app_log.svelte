<script>
  import { AppLog } from "stores/app_log.js";

  export let rxonly = false;

  $: showMqttRx = !rxonly;
  $: showMqttTx = !rxonly;
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
  <label class="bg-red-300">MQTT-RX
    <input type="checkbox" bind:checked={showMqttRx} /></label>
  <label class="bg-yellow-400">MQTT-TX
    <input type="checkbox" bind:checked={showMqttTx} /></label>
  <label class="bg-green-300">RF-RX
    <input type="checkbox" bind:checked={showRc} /></label>
  <label class="bg-blue-300">RF-TX
    <input type="checkbox" bind:checked={showSc} /></label>
</div>

<div
  id="app_log_div"
  class="w-11/12 h-full border-black border-2 bg-gray-800 overflow-auto text-xs">
  <!-- {@html htmlEntities($AppLogTxt)} -->

  {#each $AppLog as msg}
    {#if 'log' in msg}
      {#if 'mqtt_client' === msg.log.tag && msg.log.txt.startsWith("receive")}
        {#if showMqttRx}
          <span class="text-red-300">{msg.date.toLocaleTimeString() + ': ' + msg.log.tag + ': ' + msg.log.txt}<br /></span>
        {/if}
      {:else if 'mqtt_client' === msg.log.tag && msg.log.txt.startsWith("publish")}
        {#if showMqttTx}
          <span class="text-yellow-400">{msg.date.toLocaleTimeString() + ': ' + msg.log.tag + ': ' + msg.log.txt}<br /></span>
        {/if}
      {:else}
          <span class="text-gray-200">{msg.date.toLocaleTimeString() + ': ' + msg.log.tag + ': ' + msg.log.txt}<br /></span>
      {/if}

    {:else if showRc && 'rc' in msg}
      <span
        class="text-green-300">{msg.date.toLocaleTimeString() + ': ' + 'rx: ' + msg.rc.type + ': ' + msg.rc.a
         + (msg.rc.type === 'central' ? '-' + msg.rc.g + msg.rc.m : '') + ' ' + msg.rc.c
         + (msg.rc.c === 'rtc' ? ' (' + msg.rc.t + ')' : '')
         + (msg.rc.c === 'auto' ? ' (' + msg.rc.t + ',' + msg.rc.f + ')' : '')
         + (msg.rc.rssi ? (':rssi=' + msg.rc.rssi) : '')}<br /></span>
    {:else if showSc && 'sc' in msg}
      <span
        class="text-blue-300">{msg.date.toLocaleTimeString() + ': ' + 'tx: ' + msg.sc.type + ': ' + msg.sc.a 
        + (msg.sc.type === 'central' ? '-' + msg.sc.g + msg.sc.m : '') + ' ' + msg.sc.c
        + (msg.sc.c === 'rtc' ? ' (' + msg.sc.t + ')' : '')
        + (msg.sc.c === 'auto' ? ' (' + msg.sc.t + ',' + msg.sc.f + ')' : '')
        }<br /></span>
    {/if}
  {/each}
</div>

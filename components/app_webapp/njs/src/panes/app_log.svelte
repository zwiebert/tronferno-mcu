<script>
  import { AppLog } from "../store/app_log.js";

  /**
   * @typedef {Object} Props
   * @property {boolean} [rxonly]
   */

  /** @type {Props} */
  let { rxonly = false } = $props();

  let showMqttRx = $state(!rxonly);
  let showMqttTx = $state(!rxonly);
  let showRc = $state(true);
  let showSc = $state(!rxonly);

  function scroll_top(appLog) {
    let elem = document.getElementById("app_log_div");
    if (elem) elem.scrollTop = elem.scrollHeight;
  }

  $effect(() => {
    scroll_top($AppLog);
  });
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

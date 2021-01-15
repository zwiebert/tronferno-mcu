<script>
  import { AppLogTxt, AppLog } from "stores/app_log.js";


  $: showMqtt = true;
  $: showRc = true;

  function htmlEntities(str) {
    let safeString = String(str)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;");
    return String(safeString).replace(/\n/g, "<br>\n");
  }

  function dummy_fun(x) {}
  $: {
    dummy_fun($AppLog);
    let elem = document.getElementById("app_log_div");
    if (elem) elem.scrollTop = elem.scrollHeight;
  }
</script>

<div class="flex-row">
  Filters:
<label class="bg-red-300">MQTT <input type="checkbox" bind:checked={showMqtt} /></label>
<label class="bg-green-300">Receiver <input type="checkbox" bind:checked={showRc} /></label>
</div>

<div
  id="app_log_div"
  class="w-11/12 h-full border-black border-2 bg-gray-800 overflow-auto">
  <!-- {@html htmlEntities($AppLogTxt)} -->


  {#each $AppLog as msg}
    {#if showMqtt && 'log' in msg && 'mqtt_client' === msg.log.tag}
      <span class="text-red-300">{msg.log.tag + ": " + msg.log.txt}<br /></span>
    {:else if showRc && 'rc' in msg}
       <span class="text-green-300">{"rc: " + msg.rc.type + ": " + msg.rc.a + (msg.rc.type === "central" ? "-" + msg.rc.g + msg.rc.m : "") + "-" + msg.rc.c}<br /></span>
    {/if}
  {/each}
</div>

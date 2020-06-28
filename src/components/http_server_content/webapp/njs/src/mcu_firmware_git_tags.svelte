<script>
  "use strict";
 import { McuGitTagNames } from "./store/mcu_firmware.js";
 import { onMount, onDestroy } from 'svelte';
 import * as mcuFirmware from "./mcu_firmware.svelte";


 let on_destroy = [];
onMount(() => {
    on_destroy.push(McuGitTagNames.subscribe((names) => gitTags_genHtml(names)));
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});

  function gitTags_netota() {
    const git_tag = document.getElementById("gitTags_select").value;
    mcuFirmware.netFirmwareOTA("tag:" + git_tag);
  }

  function gitTags_genHtml(names) {
    let html =
      '<button id="gitTag_netota" type="button">Do flash selected firmware version: </button>' +
      '<select id="gitTags_select">';
    names.forEach((name) => {
      html += '<option value="' + name + '">' + name + "</option>";
    });

    html += "</select>";
    document.getElementById("gitTags_div").innerHTML = html;
    document.getElementById("gitTag_netota").onclick = () => gitTags_netota();
  }

</script>


<div id="gitTags_div" />
<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { McuFirmwareVersion } from "../store/mcu_firmware.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION);
  });

  $: version_number = $McuFirmwareVersion.match(/[0-9.]+/) || "develop";
  $: selected = 0;
  $: tag_or_branches = [version_number, "develop", "master"];
  $: tag_or_branch = tag_or_branches[selected];
  $: repo_url = "https://github.com/zwiebert/tronferno-mcu/blob/" + tag_or_branch + "/";
</script>



<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.help_doc.tt.header") }}>{$_("panes.help_doc.header")}</h4>

  Document version:

  <select bind:value={selected} >
    <option value={0}>{version_number}</option>
    <option value={1}>Latest Beta</option>
    <option value={2}>Latest Master</option>
  </select>

  <div class="area">
    <ul>
      <li><a href={repo_url + $_("panes.help_doc.starter.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.starter.txt")}</a></li>
      <li><a href={repo_url + $_("panes.help_doc.starter_faq.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.starter_faq.txt")}</a></li>
      <li><a href={repo_url + $_("panes.help_doc.mqtt.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.mqtt.txt")}</a></li>
 
    </ul>
  </div>
</div>

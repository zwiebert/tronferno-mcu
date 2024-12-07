<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { McuFirmwareVersion } from "../store/mcu_firmware.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION);
  });

  let version_number = $derived($McuFirmwareVersion.match(/[0-9.]+/) || "develop");
  let selected = $state(0);
  
  let tag_or_branches = $derived([version_number, "develop", "master"]);
  let tag_or_branch = $derived(tag_or_branches[selected]);
  let repo_url = $derived("https://github.com/zwiebert/tronferno-mcu/blob/" + tag_or_branch + "/");
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.help_doc.tt.header") }}>{$_("panes.help_doc.header")}</h4>

  <div class="area">
    <a href={$_("panes.help_doc.home_page.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.home_page.txt")}</a>

  </div>

  <div class="area">
    Document version:

    <select bind:value={selected}>
      <option value={0}>{version_number}</option>
      <option value={1}>Latest Beta</option>
      <option value={2}>Latest Master</option>
    </select>

    <ul>
      <li><a href={repo_url + $_("panes.help_doc.starter.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.starter.txt")}</a></li>
      <li><a href={repo_url + $_("panes.help_doc.starter_faq.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.starter_faq.txt")}</a></li>
      <li><a href={repo_url + $_("panes.help_doc.mqtt.url")} target="_blank" rel="noreferrer">{$_("panes.help_doc.mqtt.txt")}</a></li>
    </ul>
  </div>
</div>

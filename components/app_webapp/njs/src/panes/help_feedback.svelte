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
  <h4 class="text-center" use:tippy={{ content: $_("panes.help_feedback.tt.header") }}>{$_("panes.help_feedback.header")}</h4>

  <div class="area">
    <dl>
      <dt>
        {$_("panes.help_feedback.issues.link_src.info")}
      </dt>
      <dd>
        <a href={$_("panes.help_feedback.issues.link_src.url")} target="_blank" rel="noreferrer">{$_("panes.help_feedback.issues.link_src.txt")}</a>
      </dd>
      <dt>
        {$_("panes.help_feedback.issues.link_bin.info")}
      </dt>
      <dd>
        <a href={$_("panes.help_feedback.issues.link_bin.url")} target="_blank" rel="noreferrer">{$_("panes.help_feedback.issues.link_bin.txt")}</a>
      </dd>
      <dt>
        {$_("panes.help_feedback.issues.link_mail.info")}
      </dt>
      <dd>
        <a href={$_("panes.help_feedback.issues.link_mail.url")} target="_blank" rel="noreferrer">{$_("panes.help_feedback.issues.link_mail.txt")}</a>
      </dd>
    </dl>
  </div>
</div>

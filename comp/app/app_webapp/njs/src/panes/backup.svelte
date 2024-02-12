<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import * as misc from "../app/misc.js";
  import { Backup } from "../store/mcu_config.js";
  import ShutterPosTableRow from "../app/shutter_pos_table_row.svelte";
  import { onMount } from "svelte";

  onMount(() => {
    // httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_ALL_POS);
    httpFetch.getJson("/f/backup/settings.json");
  });

  $: backup_json = stringify_object_to_json($Backup);

  let text = backup_json;

  let pr_config_isChecked;
  let pr_auto_isChecked;
  let pr_shpref_isChecked;
  let pr_pair_isChecked;

  function parse_json_to_object(text) {
    const backup = JSON.parse(text);
    return backup;
  }

  function stringify_object_to_json(backup) {
    return JSON.stringify({ backup: backup }, undefined, 2);
  }

  function post_config(obj) {
    Object.entries(obj).forEach((el) => {
      let cmd = { config: {} };
      cmd.config[el[0]] = el[1];
      httpFetch.http_postCommand(cmd);
    });
  }

  function post_pair(obj) {
    Object.entries(obj).forEach((el) => {
      let cmd = { pair: { c: "store" } };
      cmd.pair.a = el[0];
      cmd.pair.mm = el[1];

      httpFetch.http_postCommand(cmd);
    });
  }

  function post_auto(arr) {
    [{ f: "mrsadw" }, ...arr].forEach((el) => {
      el.f += "u";
      httpFetch.http_postCommand({ auto: el });
    });
  }

  function post_shpref(arr) {
    [...arr].forEach((el) => {
      el.c = "store";
      httpFetch.http_postCommand({ shpref: el });
    });
  }
</script>

<div class="main-area">
  <h5 class="text-center">WORK IN PROGRESS / EXPERIMENTAL CODE</h5>
  <h4 class="text-center" use:tippy={{ content: $_("panes.backup.tt.header") }}>{$_("panes.backup.header")}</h4>

  <div class="area">
    <button
      use:tippy={{ content: $_("panes.backup.tt.create") }}
      on:click={() => {
        let tfmcu = { mcu: { "test-sj": 0 } };
        httpFetch.http_postCommand(tfmcu);
      }}>{$_("panes.backup.create")}</button
    > <a href="/f/backup/settings.json">/f/backup/settings.json</a><br />
  </div>

  <div class="area">
    <h5 class="text-center" use:tippy={{ content: $_("panes.backup.tt.restore") }}>JSON</h5>
    <textarea bind:value={text} wrap="hard" style="font-size:6pt" cols={56} rows={16} disabled={false} />
    <br />

    <button
      use:tippy={{ content: $_("panes.backup.tt.load") }}
      on:click={() => {
        text = backup_json;
        httpFetch.getJson("/f/backup/settings.json");
      }}>{$_("panes.backup.load")}</button
    >

    <button
      use:tippy={{ content: $_("panes.backup.tt.toClipboard") }}
      on:click={() => {
        misc.textToClipboard(text);
      }}>{$_("panes.backup.toClipboard")}</button
    >

    <div class="area">
      <h5 class="text-center" use:tippy={{ content: $_("panes.backup.tt.restore") }}>Restore</h5>

      <dl>
        <dt><input type="checkbox" bind:checked={pr_config_isChecked} /> .config</dt>
        <dd>MCU configuration (network, gpios, ...)</dd>
        <dt><input type="checkbox" bind:checked={pr_auto_isChecked} /> .auto</dt>
        <dd>Receiver built-in automatic (timer, sun, ...)</dd>
        <dt><input type="checkbox" bind:checked={pr_shpref_isChecked} /> .shpref</dt>
        <dd>Receiver related config (names, run-times)</dd>
        <dt><input type="checkbox" bind:checked={pr_pair_isChecked} /> .pair</dt>
        <dd>Transmitter (rf-code) pairings to members of groups (bitmask)</dd>
      </dl>

      <button
        disabled={!(pr_config_isChecked || pr_auto_isChecked || pr_shpref_isChecked || pr_pair_isChecked)}
        class="text-sm"
        use:tippy={{ content: $_("panes.backup.tt.restore") }}
        on:click={() => {
          const obj = parse_json_to_object(text);
          if (pr_config_isChecked) {
            post_config(obj.backup.settings.config);
          }
          if (pr_auto_isChecked) {
            post_auto(obj.backup.settings.auto);
          }
          if (pr_shpref_isChecked) {
            post_shpref(obj.backup.settings.shpref);
          }
          if (pr_pair_isChecked) {
            post_pair(obj.backup.settings.pair);
          }
        }}>Restore Now!</button
      >
    </div>
  </div>
</div>

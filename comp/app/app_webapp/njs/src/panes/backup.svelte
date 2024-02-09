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

  function parse_json_to_object(text) {
    const backup = JSON.parse(text);
    return backup;
  }

  function stringify_object_to_json(backup) {
    return JSON.stringify({ backup: backup }, undefined, 2);
  }

  function post_config(obj) {
    Object.entries(obj).forEach((el) => {
      let cmd = { "config" : {} };
      cmd.config[el[0]] = el[1]; 
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

    <button
      use:tippy={{ content: $_("panes.backup.tt.restore") }}
      on:click={() => {
        const obj = parse_json_to_object(text);
        post_auto(obj.backup.settings.auto);
        post_shpref(obj.backup.settings.shpref);
      }}>{$_("panes.backup.restore")}</button
    >

    <div class="area">
      <h6 class="text-center">Partial restore</h6>

      <button
      class="text-sm"
      use:tippy={{ content: $_("panes.backup.tt.restore") }}
      on:click={() => {
        const obj = parse_json_to_object(text);
        post_config(obj.backup.settings.config);
      }}>.config</button
    >

      <button
        class="text-sm"
        use:tippy={{ content: $_("panes.backup.tt.restore") }}
        on:click={() => {
          const obj = parse_json_to_object(text);
          post_auto(obj.backup.settings.auto);
        }}>.auto</button
      >

      <button
        use:tippy={{ content: $_("panes.backup.tt.restore") }}
        on:click={() => {
          const obj = parse_json_to_object(text);
          post_shpref(obj.backup.settings.shpref);
        }}>.shpref</button
      >
    </div>
  </div>
</div>

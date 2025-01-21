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


  let text = $state(stringify_object_to_json($Backup));


  let pr_config_isChecked = $state();
  let pr_config_netconn_isChecked = $state();
  let pr_auto_isChecked = $state();
  let pr_shpref_isChecked = $state();
  let pr_pair_isChecked = $state();
  let pr_kvs_isChecked = $state();

  function parse_json_to_object(text) {
    const backup = JSON.parse(text);
    return backup;
  }

  function stringify_object_to_json(backup) {
    return JSON.stringify({ backup: backup }, undefined, 2);
  }

  function post_config(obj, restore_netconn) {
    Object.entries(obj).forEach((el) => {
      if (!restore_netconn) {
        const key = el[0];
        if (key.startsWith("wlan-"))
          return;
        if (key.startsWith("lan-"))
          return;
        if (key === "network")
          return;
      }
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

  function post_kvs(obj) {
    Object.entries(obj).forEach((el) => {
      let cmd = { kvs: {} };
      cmd.kvs[el[0]] = el[1];
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
    <h5 class="text-center" use:tippy={{ content: $_("panes.backup.tt.create") }}>Create a Backup</h5>

    <button
      use:tippy={{ content: $_("panes.backup.tt.create") }}
      onclick={() => {
        let tfmcu = { mcu: { "test-sj": 0 } };
        httpFetch.http_postCommand(tfmcu);
      }}>{$_("panes.backup.create")}</button
    > <a href="/f/backup/settings.json">View Snapshot</a><br />
  </div>

  <div class="area">
    <h5 class="text-center" use:tippy={{ content: $_("panes.backup.tt.restore") }}>View/Edit Backup (JSON)</h5>

    <div class="mr-2">
      <textarea class="w-full" bind:value={text} wrap="hard" style="font-size:6pt" cols={56} rows={16} disabled={false}></textarea>
    </div>
    <div class="flex flex-row">
      <label class="file_input whitespace-nowrap" use:tippy={{ content: $_("panes.backup.tt.open") }}>
        {$_("panes.backup.open")}
        <input
          class="hidden"
          type="file"
          accept=".json"
          onchange={(e) => {
            e.target.files[0].text().then((txt) => {
              text = stringify_object_to_json(parse_json_to_object(txt));
            });
          }}
        />
      </label>

      <div class="w-full flex flex-row-reverse">
        <button
          class=""
          use:tippy={{ content: $_("panes.backup.tt.toClipboard") }}
          onclick={() => {
            misc.textToClipboard(text);
          }}>{$_("panes.backup.toClipboard")}</button
        >
        <button
          class=""
          use:tippy={{ content: $_("panes.backup.tt.load") }}
          onclick={() => {
            text = stringify_object_to_json($Backup);
            httpFetch.getJson("/f/backup/settings.json");
          }}>{$_("panes.backup.load")}</button
        >
      </div>
    </div>

    <div class="area">
      <h5 class="text-center" use:tippy={{ content: $_("panes.backup.tt.restore") }}>Select Parts to Restore</h5>

      <dl>
        <dt>
          <label><input type="checkbox" bind:checked={pr_config_isChecked} /> .config</label>
          <label><input type="checkbox" bind:checked={pr_config_netconn_isChecked} disabled={!pr_config_isChecked}/>..network_connection</label>
        </dt>
        <dd>MCU configuration (network, gpios, ...)</dd>
        <dt><label><input type="checkbox" bind:checked={pr_auto_isChecked} /> .auto</label></dt>
        <dd>Receiver built-in automatic (timer, sun, ...)</dd>
        <dt><label><input type="checkbox" bind:checked={pr_shpref_isChecked} /> .shpref</label></dt>
        <dd>Receiver related config (names, run-times)</dd>
        <dt><label><input type="checkbox" bind:checked={pr_pair_isChecked} /> .pair</label></dt>
        <dd>Transmitter (rf-code) pairings to members of groups</dd>
        <dt><label><input type="checkbox" bind:checked={pr_kvs_isChecked} /> .kvs</label></dt>
        <dd>generic storage (transmitter names)</dd>
      </dl>

      <button
        disabled={!(pr_config_isChecked || pr_auto_isChecked || pr_shpref_isChecked || pr_pair_isChecked || pr_kvs_isChecked)}
        class="text-sm"
        use:tippy={{ content: $_("panes.backup.tt.restore") }}
        onclick={() => {
          const obj = parse_json_to_object(text);
          if (pr_config_isChecked) {
            post_config(obj.backup.settings.config, pr_config_netconn_isChecked);
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
          if (pr_kvs_isChecked) {
            post_kvs(obj.backup.settings.kvs);
          }
        }}>{$_("panes.backup.restore")}</button
      >
    </div>
  </div>
</div>

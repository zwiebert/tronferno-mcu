<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { Names } from "../store/shutters.js";
  import { McuConfig, Gmu } from "../store/mcu_config.js";
  import { onMount } from "svelte";
  import * as misc from "../app/misc.js";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_CONFIG_P2);
  });

  $: gmu = $Gmu;
  $: root_topic = ($McuConfig["mqtt-root-topic"] || "tfmcu") + "/";
  $: mqttText = "";
  $: moduleText = "";

  $: {
    $Gmu;
    $Names;
    root_topic;
    mqttText = genText_mqtt($Gmu);
  }

  function get_name(g, m) {
    const gm = g.toString() + m.toString();
    return g.toString() + (m ? m.toString() : "A") + ($Names[gm] ? " " + $Names[gm] : "");
  }

  function genText_mqtt(gmu) {
    let txt =
      "# Auto generated configuration for mqtt/cover\n" +
      "# Copy and paste it into configuration file:\n" +
      "#  Linux: /home/homeassistant/.homeassistant/configuration.yaml\n" +
      "#  HassOS: /config/configuration.yaml\n" +
      "mqtt:\n" +
      "  cover:\n";

    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          txt +=
            `    - name: "${get_name(g, m)}"\n` +
            `      command_topic: "${root_topic + g.toString() + m.toString()}/cmd"\n` +
            `      position_topic: "${root_topic + g.toString() + m.toString()}/pct_out"\n` +
            `      set_position_topic: "${root_topic + g.toString() + m.toString()}/pct"\n` +
            `      qos: 1\n` +
            `      payload_open: "up"\n` +
            `      payload_close: "down"\n` +
            `      payload_stop: "stop"\n`;
        }
    }
    return txt;
  }
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.hs_config.home_assistant.tt.header") }}>{$_("panes.hs_config.home_assistant.header")}</h4>

  <div class="area">
    <button
      on:click={() => {
        misc.textToClipboard(mqttText);
      }}>Copy configuration to clipboard</button
    ><br />
    <textarea value={mqttText} style="font-size:6pt" cols={56} rows={16} disabled={true} />
  </div>
</div>

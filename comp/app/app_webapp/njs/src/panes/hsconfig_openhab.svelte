<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { Names } from "../store/shutters.js";
  import { McuConfig, Gmu } from "../store/mcu_config.js";
  import ShutterGM from "../app/shutter_gm.svelte";
  import { AutoData, AutoSunEnabled, G, M, M0, GM, GMH } from "../store/curr_shutter.js";
  import { onMount } from "svelte";
  import * as misc from "../app/misc.js";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU);
  });

  $: gmu = $Gmu;
  $: root_topic = ($McuConfig["mqtt-root-topic"] || "tfmcu") + "/";
  $: mqttText = "";
  $: mqttTextGm = "";

  $: {
    $Gmu;
    $Names;
    root_topic;
    mqttText = genText_mqtt($Gmu);
  }

  $: {
    $G;
    $M0;
    $Names;
    root_topic;
    mqttTextGm = genText_mqtt_gm($G, $M0);
  }

  function get_name(g, m) {
    const gm = g.toString() + m.toString();
    return g.toString() + (m ? m.toString() : "A") + ($Names[gm] ? " " + $Names[gm] : "");
  }

  function genText_mqtt(gmu) {
    let txt =
      "# Auto generated configuration for OpenHAB-3\n" + //
      "# Copy and paste it into code tab of a MQTT-Generic-Thing\n";

    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const gm = g.toString() + m.toString();
          txt += genText_mqtt_gm(g, m, $Names[gm]);
          txt += "############################################################\n\n";
        }
    }
    return txt;
  }

  function genText_mqtt_gm(g, m, location) {
    let txt = "";
    const gm = g.toString() + m.toString();
    const base_topic = root_topic + gm;
    if (location) txt += `location: ${location}\n`;

    txt +=
      `channels:\n` +
      `  - id: roll${gm}_ch\n` +
      `    channelTypeUID: mqtt:rollershutter\n` +
      `    label: ${get_name(g, m)}\n` +
      `    description: null\n` +
      `    configuration:\n` +
      `      commandTopic: ${base_topic}/cmd\n` +
      `      stop: stop\n` +
      `      stateTopic: ${base_topic}/pct_out\n` +
      `      off: down\n` +
      `      on: up\n` +
      `  - id: proll${gm}_ch\n` +
      `    channelTypeUID: mqtt:dimmer\n` +
      `    label: ${get_name(g, m)} Position\n` +
      `    description: null\n` +
      `    configuration:\n` +
      `      commandTopic: ${base_topic}/pct\n` +
      `      stateTopic: ${base_topic}/pct_out\n` +
      `      off: "0"\n` +
      `      on: "100"\n`;

    return txt;
  }
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.hs_config.openhab.tt.header") }}>{$_("panes.hs_config.openhab.header")}</h4>

  <div class="area">
    <div class="mt-4 p-2">
      <ShutterGM radio={false} />
    </div>

    <button
      on:click={() => {
        misc.textToClipboard(mqttTextGm);
      }}>Copy configuration of {$GM} to clipboard</button
    ><br />
    <textarea value={mqttTextGm} style="font-size:6pt" cols={56} rows={16} disabled={true} />
  </div>

  <div class="area">
    <button
      on:click={() => {
        misc.textToClipboard(mqttText);
      }}>Copy all configurations to clipboard</button
    ><br />
    <textarea value={mqttText} style="font-size:6pt" cols={56} rows={16} disabled={true} />
  </div>
</div>

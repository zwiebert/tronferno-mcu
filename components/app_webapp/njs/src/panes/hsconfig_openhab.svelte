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
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_CONFIG);
  });

  $: gmu = $Gmu;
  $: root_topic = ($McuConfig["mqtt-root-topic"] || "tfmcu") + "/";
  $: oh_bridgeUID = "c7e9dfce43";

  $: mqttTextGm = "";
  $: mqttTextThings = "";
  $: mqttTextItems = "";

  $: {
    $Gmu;
    $Names;
    root_topic;
    mqttTextThings = genText_mqttThings($Gmu);
  }

  $: {
    $Gmu;
    $Names;
    root_topic;
    mqttTextItems = genText_mqttItems($Gmu);
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

  function genText_mqttThings(gmu) {
    let txt =
      "// Auto generated configuration for OpenHAB-3\n" + //
      "// Copy and paste it into new file:\n" +
      "// $OPENHAB_CONF/things/tronferno.things\n\n";

    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const gm = g.toString() + m.toString();
          txt += genText_mqttThings_gm(g, m, $Names[gm]);
          txt += "\n\n";
        }
    }
    return txt;
  }

  function genText_mqttThings_gm(g, m, location) {
    let txt = "";
    const gm = g.toString() + m.toString();
    const base_topic = root_topic + gm;

    txt += //
      `Thing mqtt:topic:${oh_bridgeUID}:tfmcu${gm} "Rollo${gm}"\n` +
      `     (mqtt:broker:${oh_bridgeUID}) @ "${location}" {\n` +
      `  Channels:\n` +
      `   Type rollershutter : cmd [commandTopic="${base_topic}/cmd",\n` +
      `                             stateTopic="${base_topic}/ipct_out",\n` +
      `                             off="down", on="up", stop="stop"]\n` +
      `   Type dimmer : pct [commandTopic="${base_topic}/ipct",\n` +
      `                      stateTopic="${base_topic}/ipct_out",\n` +
      `                      off="100", on="0"]\n` +
      `  }\n`;

    return txt;
  }

  function genText_mqttItems(gmu) {
    let txt =
      "// Auto generated configuration for OpenHAB-3\n" + //
      "// Copy and paste it into new file:\n" +
      "// $OPENHAB_CONF/items/tronferno.items\n\n";

    txt += `Group gTronfernoA "Fernotron" <blinds> ["Blinds"]\n\n`;

    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const gm = g.toString() + m.toString();

          txt += genText_mqttItems_gm(g, m, $Names[gm]);
        }
    }
    return txt;
  }

  function genText_mqttItems_gm(g, m, location) {
    let txt = "";
    const gm = g.toString() + m.toString();
    const base_topic = root_topic + gm;

    txt += //
      `Rollershutter pTronferno${gm}_rollershutter "Fernotron${gm}_cmd" <blinds> (gTronferno${gm}) ["Point"]\n` +
      `{channel="mqtt:topic:${oh_bridgeUID}:tfmcu${gm}:cmd"}\n` +
      `Dimmer pTronferno${gm}_dimmer "Fernotron${gm}_pct" <blinds> (gTronferno${gm}) ["Point"]\n` +
      `   { channel="mqtt:topic:${oh_bridgeUID}:tfmcu${gm}:pct"}\n` +
      `Group gTronferno${gm} "Fernotron_${gm}" <blinds> (gTronfernoA) ["Blinds"]\n` +
      `\n`;

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
    <h5>UI Based Config (work in progress)</h5>
    <div class="mt-4 p-2">
      <ShutterGM radio={false} />
    </div>

    <button
      on:click={() => {
        misc.textToClipboard(mqttTextGm);
      }}>Copy configuration of {$GM} to clipboard</button
    ><br />
    <textarea class="hscfg" value={mqttTextGm} cols={56} rows={16} disabled={true} />
  </div>

  <div class="area">
    <h5>Configuration files (work in progress)</h5>
    <table>
      <tbody>
        <tr><td>MQTT broker ID</td><td><input type="text" bind:value={oh_bridgeUID} /></td></tr>
      </tbody>
    </table>

    <button
      on:click={() => {
        misc.textToClipboard(mqttTextThings);
      }}>Copy all configurations to clipboard</button
    ><br />
    <textarea class="hscfg" value={mqttTextThings} cols={56} rows={16} disabled={true} />
    <hr />
    <button
      on:click={() => {
        misc.textToClipboard(mqttTextItems);
      }}>Copy all configurations to clipboard</button
    ><br />
    <textarea class="hscfg" value={mqttTextItems} cols={56} rows={16} disabled={true} />
  </div>
</div>

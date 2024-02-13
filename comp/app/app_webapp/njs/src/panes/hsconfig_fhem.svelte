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
  $: room = "Rollo";
  $: name_prefix = "Rollo_";
  $: alexa_name_prefix = "Rollo";
  $: clientId = ""; // ($McuConfig["mqtt-client-id"] || "tfmcu")
  $: defmodText = "";
  $: deleteText = "";
  $: radio_value = 0;

  $: {
    $Gmu;
    $Names;
    room;
    name_prefix;
    alexa_name_prefix;
    root_topic;
    deleteText = genText_delete($Gmu);
    defmodText = radio_value ? genText_module($Gmu) : genText_mqtt($Gmu);
  }

  $: {
    name_prefix = name_prefix.replace(/[^a-zA-Z0-9_:]/g, ""); //enforce FHEM device name rules
  }

  function get_topic_pct(g, m) {
    return g.toString() + m.toString() + "/pct";
  }
  function get_topic_pct_out(g, m) {
    return g.toString() + m.toString() + "/pct_out";
  }
  function get_topic_cmd(g, m) {
    return g.toString() + m.toString() + "/cmd";
  }
  function get_devName(g, m) {
    return name_prefix + g.toString() + m.toString();
  }
  function get_alias(g, m) {
    const gm = g.toString() + m.toString();
    return g.toString() + (m ? m.toString() : "A") + ($Names[gm] ? " " + $Names[gm] : "");
  }

  function get_alexaName(g, m) {
    const gm = g.toString() + m.toString();
    return alexa_name_prefix + " " + ($Names[gm] ? $Names[gm] : gm);
  }

  function genText_mqtt(gmu) {
    let txt =
      "# Configuration for mqtt server\n" + //
      "# This does not need any modules from tronferno-fhem\n" + //
      "# Paste it into multiline command input [+] in FHEM-Web\n" +
      `# and press button "Execute"\n\n`;

    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const devName = get_devName(g, m);
          txt +=
            `defmod ${devName} MQTT2_DEVICE ${clientId}\n` +
            `attr ${devName} alias ${get_alias(g, m)}\n` +
            `attr ${devName} alexaName ${get_alexaName(g, m)}\n` +
            `attr ${devName} room ${room}\n` +
            `attr ${devName} IODev mqtts\n` +
            `attr ${devName} genericDeviceType blind\n` +
            `attr ${devName} autocreate 0\n` +
            `attr ${devName} readingList ${root_topic}status:.* status\\\n` +
            `${root_topic + get_topic_pct_out(g, m)}:.* state\\\n` +
            `${root_topic + get_topic_pct_out(g, m)}:.* pct\n` +
            `attr ${devName} setList stop:noArg ${root_topic + get_topic_cmd(g, m)} stop\\\n` +
            `up:noArg ${root_topic + get_topic_cmd(g, m)} up\\\n` +
            `down:noArg ${root_topic + get_topic_cmd(g, m)} down\\\n` +
            `sun-down:noArg ${root_topic + get_topic_cmd(g, m)} sun-down\\\n` +
            `pct:slider,0,5,100  ${root_topic + get_topic_pct(g, m)} $EVTPART1\\\n` +
            `sun-auto:1,0 ${root_topic}cli auto g=${g} m=${m} f=k  sun-auto=$EVTPART1\\\n` +
            `get_pct:noArg  ${root_topic + get_topic_pct(g, m)} ?\n` +
            `attr ${devName} webCmd up:stop:down:pct\n` +
            "\n";
        }
    }
    return txt;
  }

  function genText_module(gmu) {
    let txt =
      "# Configuration for tronferno-fhem project\n" + //
      "# Needs the modules Tronferno and TronfernoMCU\n" + //
      "# Paste it into multiline command input [+] in FHEM-Web\n" +
      `# and press button "Execute"\n\n`;
    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const devName = get_devName(g, m);
          txt +=
            `defmod ${devName} Tronferno g=${g} m=${m}\n` +
            `attr ${devName} alias ${get_alias(g, m)}\n` +
            `attr ${devName} alexaName ${get_alexaName(g, m)}\n` +
            `attr ${devName} room ${room}\n` +
            `attr ${devName} IODev tfmcu\n` +
            `attr ${devName} genericDeviceType blind\n` +
            `attr ${devName} webCmd up:stop:down:pct\n` +
            "\n";
        }
    }
    return txt;
  }

  function genText_delete(gmu) {
    let txt =
      "# Delete previously defined devices\n" + //
      "# Paste it into multiline command input [+] in FHEM-Web\n" +
      `# and press button "Execute"\n\n`;
    for (let g = 0; g < gmu.length; ++g) {
      if (g === 0 || gmu[g])
        for (let m = 0; m <= gmu[g]; ++m) {
          const devName = get_devName(g, m);
          txt += `delete ${devName}\n`;
        }
    }
    return txt;
  }
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.hs_config.fhem.tt.header") }}>{$_("panes.hs_config.fhem.header")}</h4>

  <div class="area">
    <label
      ><input
        checked={radio_value === 0}
        on:change={() => {
          radio_value = 0;
        }}
        type="radio"
        name="def_type"
        value={0}
        disabled={false}
      />MQTT</label
    >

    <label
      ><input
        checked={radio_value === 1}
        on:change={() => {
          radio_value = 1;
        }}
        type="radio"
        name="def_type"
        value={1}
        disabled={false}
      />tronferno-fhem</label
    >

    <table>
      <tr><td>Device-Name Prefix</td><td><input type="text" bind:value={name_prefix} /></td></tr>
      <tr><td>Alexa-Name Prefix</td><input type="text" bind:value={alexa_name_prefix} /><td></td></tr>
      <tr><td>Room(s)</td><input type="text" bind:value={room} /><td></td></tr>
    </table>

    <h5 class="text-left">Generated Device Definitions</h5>
    <button
      class="block"
      on:click={() => {
        misc.textToClipboard(defmodText);
      }}>Copy definitions to clipboard</button
    >

    <textarea class="block" value={defmodText} style="font-size:6pt" cols={56} rows={16} disabled={true} />

    <h5 class="text-left">Generated Device Deletions</h5>
    <button
      class="block"
      on:click={() => {
        misc.textToClipboard(deleteText);
      }}>Copy deletions to clipboard</button
    >

    <textarea class="block" value={deleteText} style="font-size:6pt" cols={56} rows={16} disabled={true} />
  </div>
</div>

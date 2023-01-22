<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { Names } from "../store/shutters.js";
  import { McuConfig, Gmu } from "../store/mcu_config.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GMU | httpFetch.FETCH_CONFIG_P2);
  });

  $: gmu = $Gmu;
  $: root_topic = ($McuConfig["mqtt-root-topic"] || "tfmcu") + "/";
  $: room = "Rollo";
  $: clientId = ""; // ($McuConfig["mqtt-client-id"] || "tfmcu")
  $: mqttText = "";
  $: moduleText = "";

  $: {
    $Gmu;
    mqttText = genText_mqtt($Gmu);
    moduleText = genText_module($Gmu);
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
    return "Rollo_" + g.toString() + m.toString();
  }
  function get_alias(g, m) {
    const gm = g.toString() + m.toString();
    return g.toString() + (m ? m.toString() : "A") + ($Names[gm] ? " " + $Names[gm] : "");
  }

  function get_alexaName(g, m) {
    const gm = g.toString() + m.toString();
    return "Rollo " + ($Names[gm] ? $Names[gm] : gm);
  }

  function genText_mqtt(gmu) {
    let txt = "# Configuration for mqtt server\n" + "# Copy and paste it into a raw definition window in FHEM-Web and execute it\n\n";

    for (let g = 1; g < gmu.length; ++g) {
      for (let m = 0; m <= gmu[g]; ++m) {
        const devName = get_devName(g, m);
        txt +=
          `defmod ${get_devName(g, m)} MQTT2_DEVICE ${clientId}\n` +
          `attr ${get_devName(g, m)} alias ${get_alias(g, m)}\n` +
          `attr ${get_devName(g, m)} alexaName ${get_alexaName(g, m)}\n` +
          `attr ${get_devName(g, m)} IODev mqtts\n` +
          `attr ${get_devName(g, m)} genericDeviceType blind\n` +
          `attr ${get_devName(g, m)} autocreate 0\n` +
          `attr ${get_devName(g, m)} readingList ${root_topic}status:.* status\\\n` +
          `${root_topic + get_topic_pct_out(g, m)}:.* state\\\n` +
          `${root_topic + get_topic_pct_out(g,m)}:.* pct\n` +
          `attr ${get_devName(g, m)} room ${room}\n` +
          `attr ${get_devName(g, m)} setList stop:noArg ${root_topic + get_topic_cmd(g, m)} stop\\\n` +
          `up:noArg ${root_topic + get_topic_cmd(g, m)} up\\\n` +
          `down:noArg ${root_topic + get_topic_cmd(g, m)} down\\\n` +
          `sun-down:noArg ${root_topic + get_topic_cmd(g, m)} sun-down\\\n` +
          `pct:slider,0,5,100  ${root_topic + get_topic_pct(g, m)} $EVTPART1\\\n` +
          `sun-auto:1,0 ${root_topic}cli auto g=${g} m=${m} f=k  sun-auto=$EVTPART1\\\n` +
          `get_pct:noArg  ${root_topic + get_topic_pct(g, m)} ?\n` +
          `attr ${get_devName(g, m)} webCmd up:stop:down:pct\n` +
          "\n";
      }
    }
    return txt;
  }

  function genText_module(gmu) {
    let txt = "# Configuration for tronferno-mcu module\n" + "# Copy and paste it into a raw definition window in FHEM-Web and execute it\n\n";

    for (let g = 1; g < gmu.length; ++g) {
      for (let m = 0; m <= gmu[g]; ++m) {
        const devName = get_devName(g, m);
        txt +=
          `defmod ${get_devName(g, m)} Tronferno g=${g} m=${m}\n` +
          `attr ${get_devName(g, m)} alias ${get_alias(g, m)}\n` +
          `attr ${get_devName(g, m)} alexaName ${get_alexaName(g, m)}\n` +
          `attr ${get_devName(g, m)} IODev tfmcu\n` +
          `attr ${get_devName(g, m)} genericDeviceType blind\n` +
          `attr ${get_devName(g, m)} room ${room}\n` +
          `attr ${get_devName(g, m)} webCmd up:stop:down:pct\n` +
          "\n";
      }
    }
    return txt;
  }
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.hs_config.fhem.tt.header") }}>{$_("panes.hs_config.fhem.header")}</h4>

  <div>
    <button
      on:click={() => {
        navigator.clipboard.writeText(mqttText);
      }}>Copy MQTT definitions to clipboard</button
    ><br />
    <textarea value={mqttText} cols={80} rows={25} disabled={true} />
  </div>

  <div>
    <button
      on:click={() => {
        navigator.clipboard.writeText(moduleText);
      }}>Copy FHEM module definitions to clipboard</button
    ><br />
    <textarea value={moduleText} cols={80} rows={25}  disabled={true} />
  </div>
</div>

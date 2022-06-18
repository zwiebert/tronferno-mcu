<script>
  "use strict";
  import { G, M, GM, GMH, RadioCode, RadioCodeEnabled } from "stores/curr_shutter.js";
  import { Names } from "stores/shutters.js";
  import { Gmu } from "stores/mcu_config.js";
  import { GuiAcc } from "stores/app_state";
  import * as httpFetch from "app/fetch.js";

  let gmInit = $RadioCodeEnabled ? "RadioCode" : $GM;
  $: gm = gmInit;
  $: names = Object.values($Names);
  $: a = "A " + ($Names["00"] || "");
  $: showRadioCode = gm === "RadioCode";

  $: use_motorCode = showRadioCode;
  let motorCode = $RadioCode;
  $: motorCode_isValid = false;
  $: target = use_motorCode ? motorCode : $GMH;

  function get_motorCode(mc) {
    const re = /^9?0[a-fA-F0-9]{4}$/;
    if (!re.test(mc)) {
      return "invalid";
    }
    /*
    if (mc.startsWith("0")) {
      return "9" + mc;
    }
    */

    return mc;
  }

  $: {
    const mc = get_motorCode(motorCode);
    const rce = gm === "RadioCode";
    $RadioCodeEnabled = rce;
    motorCode_isValid = mc !== "invalid";

    if (rce && motorCode_isValid) $RadioCode = mc;
  }

  $: {
    if (!showRadioCode) {
      $G = Number.parseInt(gm.substr(0, 1));
      $M = Number.parseInt(gm.substr(1, 1));
      httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAME);
    }
  }
</script>

<select bind:value={gm}>
  <option value="00">{a}</option>
  {#each [1, 2, 3, 4, 5, 6, 7] as g}
    {#if $Gmu[g]}
      {#each { length: $Gmu[g] + 1 } as _, m}
        <option value={g.toString() + m.toString()}>
          {g}{m || "A"}
          {$Names[g.toString() + m.toString()] || ""}
        </option>
      {/each}
    {/if}
  {/each}
  {#if $GuiAcc.radio_code}
    <option value={"RadioCode"}> RadioCode </option>
  {/if}
</select>

{#if showRadioCode}
  <input class="w-24 {motorCode_isValid ? 'text-green-600' : 'text-red-600'}" type="text" maxlength="6" bind:value={motorCode} disabled={!use_motorCode} />
{/if}

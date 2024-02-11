<script>
  "use strict";
  import { _ } from "../services/i18n";
  import { GMR, G, M, GM, GMH, RadioCode, RadioCodeEnabled } from "../store/curr_shutter.js";
  import { Names } from "../store/shutters.js";
  import { Gmu } from "../store/mcu_config.js";
  import { GuiAcc } from "../store/app_state";
  import * as httpFetch from "../app/fetch.js";

  export let radio = true;
  export let groups = true;

  let gmInit = $RadioCodeEnabled && $GuiAcc.radio_code && radio ? "RadioCode" : $GM;
  $: {
    $GMR = gmInit;
  }
  $: names = Object.values($Names);
  $: a = "A " + ($Names["00"] || "");
  $: showRadioCode = $GMR === "RadioCode" && $GuiAcc.radio_code && radio;

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
    if (!$GuiAcc.radio_code || !radio) showRadioCode = false;
  }

  $: {
    const mc = get_motorCode(motorCode);
    const rce = $GMR === "RadioCode";
    $RadioCodeEnabled = rce;
    motorCode_isValid = mc !== "invalid";

    if (rce && motorCode_isValid) $RadioCode = mc;
  }

  $: {
    if (!showRadioCode) {
      $G = Number.parseInt($GMR.substr(0, 1));
      $M = Number.parseInt($GMR.substr(1, 1));
      httpFetch.http_fetchByMask(httpFetch.FETCH_SHUTTER_NAME);
    }
  }

</script>

<select bind:value={$GMR}>
  {#if groups}
    <option value="00">{a}</option>
  {/if}
  {#each [1, 2, 3, 4, 5, 6, 7] as g}
    {#if $Gmu[g]}
      {#each { length: $Gmu[g] + 1 } as _, m}
        {#if m || groups}
          <option value={g.toString() + m.toString()}>
            {g}{m || "A"}
            {$Names[g.toString() + m.toString()] || ""}
          </option>
        {/if}
      {/each}
    {/if}
  {/each}
  {#if $GuiAcc.radio_code && radio}
    <option value={"RadioCode"}> {$_("fernotron.RadioCode")} </option>
  {/if}
</select>

{#if showRadioCode && $GuiAcc.radio_code && radio}
  <input class="w-24 {motorCode_isValid ? 'text-green-600' : 'text-red-600'}" type="text" maxlength="6" bind:value={motorCode} disabled={!use_motorCode} />
{/if}

<script>
  "use strict";
  import { M0, RadioCodeEnabled } from "../store/curr_shutter.js";
  import * as httpFetch from "../app/fetch.js";

  import { GuiAcc } from "../store/app_state";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";

  /**
   * @typedef {Object} Props
   * @property {boolean} [disabled]
   */

  /** @type {Props} */
  let { disabled = false } = $props();

  function hClick_Sun() {
    httpFetch.http_postShutterCommand("sun-down");
  }

  function hClick_SunPos() {
    httpFetch.http_postShutterCommand("sun-inst");
  }
</script>

<div class="text-center">
  <button
    id="sspb"
    class="sb text-lg rounded-l-full rounded-r-full"
    type="button"
    {disabled}
    onclick={hClick_Sun}
    use:tippy={{ content: $_("app.sun.tt.move_sun_down") }}
  >
    {$_("app.sun.move_sun_down")}
  </button>

  {#if false && $GuiAcc.shutter_sunpos && ($M0 || $RadioCodeEnabled)}
    <button
      id="sspb"
      class="sb text-lg rounded-r-full"
      type="button"
      onclick={hClick_SunPos}
      use:tippy={{ content: $_("app.sun.tt.set_sun_pos") }}
    >
      {$_("app.sun.set_sun_pos")}
    </button>

    xxx
  {/if}
</div>

<style lang="scss">
</style>

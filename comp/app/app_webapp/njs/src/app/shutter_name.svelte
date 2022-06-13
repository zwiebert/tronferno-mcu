<script>
  "use strict";
  import { _ } from "services/i18n";
  import { G, M0, Name } from "stores/curr_shutter.js";
  import { GuiAcc } from "stores/app_state";
  import * as httpFetch from "app/fetch.js";
  import { Names } from "stores/shutters.js";
  import tippy from "sveltejs-tippy";
  
  $: GMName = $Name || "";

  function storeName_toMcu(g, m, name) {
    let tfmcu = { to: "tfmcu", shpref: { g: g, m: m, "tag.NAME": name } };

    httpFetch.http_postCommand(tfmcu);
  }

  function hChange_Name() {
    storeName_toMcu($G, $M0, GMName);
  }


  function names_fromBrowserStorage_toMcu() {
    console.log($Names);
     for (let g=0; g < 8; ++g) {
       for (let m=0; m < 8; ++m) {
         const name = $Names[`${g}${m}`];
         console.log(`${g}${m} ${name}`);
         if (name !== undefined) {
          storeName_toMcu(g, m, name);
         }
       }
     }

  }
</script>

<div class="text-center">
  <input type="text" name="name" bind:value={GMName} on:change={hChange_Name} />
  <br />
  {#if $GuiAcc.restore_shutter_names}
  <button type="button" use:tippy={{ content: $_("app.names.tt.restoreNames") }} on:click={names_fromBrowserStorage_toMcu}>{$_("app.names.restoreNames")}</button>
  {/if}
</div>

<style lang="scss">
</style>

<script>
  "use strict";
  import Layout from "app/layout.svelte";
  import Wapp from "app/wapp.svelte";
  import { setupI18n } from "services/i18n";
  import {SepModeEnabled } from "stores/app_state";
  import SepMode from "app/sep_control.svelte";

  setupI18n();

</script>

<Layout />

{#if !$SepModeEnabled}
 <Wapp />
 {:else}
  <SepMode />
{/if}



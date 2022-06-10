<script>
  "use strict";
  import Layout from "app/layout.svelte";
  import Wapp from "app/wapp.svelte";
  import { setupI18n } from "services/i18n";
  import {SepMode } from "stores/sep";
  import SepControl from "app/sep_control.svelte";

  setupI18n();

</script>

<Layout />

{#if !$SepMode.enabled}
 <Wapp />
 {:else}
  <SepControl />
{/if}



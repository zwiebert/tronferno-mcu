<script>
  "use strict";
  import Layout from "../app/layout.svelte";
  import Wapp from "../app/wapp.svelte";
  import { setupI18n } from "../services/i18n";
  import { SepMode } from "../store/sep";
  import { GuiUserLevel } from "../store/app_state";
  import * as misc from "../app/misc.js";
  import SepControl from "../app/sep_control.svelte";

  setupI18n();
  misc.set_gui_access_level($GuiUserLevel);


  $effect(() => {
    misc.set_gui_access_level($GuiUserLevel);
  });

</script>

<Layout />

{#if !$SepMode.enabled}
  <Wapp />
{:else}
  <SepControl />
{/if}

<script>
  "use strict";
  import { _ } from "services/i18n";
  import IdSelector from "app/id_selector.svelte";
  import TransmitterNames from "app/transmitter_names.svelte";
  import { GuiAcc } from "stores/app_state";
  import tippy from "sveltejs-tippy";
</script>

<div class="main-area text-center">
  <div>
    <IdSelector />
  </div>

  {#if $GuiAcc.edit_transmitter_names}
    <div class="main-area">
      <h5>{$_("app.navTab.sender.names.header")}</h5>
      <TransmitterNames />
    </div>
  {/if}
</div>

<script>
  "use strict";
  import { _ } from "services/i18n";
  import IdSelector from "app/id_selector.svelte";
  import ShutterAlias from "app/shutter_alias.svelte";
  import ShutterAliasRf from "app/shutter_alias_rf.svelte";
  import tippy from "sveltejs-tippy";
</script>

<div class="main-area text-center">
  <h4 class="text-center" use:tippy={{ content: $_("panes.register.tt.header") }}>{$_("panes.register.header")}</h4>

  <div>
    <ShutterAliasRf />
  </div>
  <hr />

  <div>
    <IdSelector />
  </div>

  <div>
    <ShutterAlias />
  </div>
</div>

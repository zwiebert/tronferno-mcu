<script>
  "use strict";
  import { _ } from "services/i18n";
  import IdSelector from "app/id_selector.svelte";
  import tippy from "sveltejs-tippy";
  import Repeater from "app/repeater.svelte";
</script>

<div class="main-area">
  <h4 class="text-center" use:tippy={{ content: $_("panes.repeater.tt.header") }}>{$_("panes.repeater.header") }</h4>

  <div>
    <IdSelector />
  </div>

  <div>
    <Repeater />
  </div>
</div>

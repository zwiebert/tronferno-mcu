<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { SepMode } from "../store/sep";

  function create_auth_key() {
    return Math.floor(Math.random() * 10000) + 1;
  }
</script>

<p class="w-80 text-left">{@html $_("app.sep.warning_html")}</p>

<button
  type="button"
  onclick={() => {
    $SepMode.auth_key = create_auth_key();
    $SepMode.enabled = true;
  }}
  use:tippy={{ content: $_("app.sep.tt.enter") }}
  >{$_("app.sep.enter")}
</button>

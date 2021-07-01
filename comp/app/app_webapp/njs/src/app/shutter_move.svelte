<script>
  "use strict";
  import * as httpFetch from "app/fetch.js";

  function hClick_Stop() {
    httpFetch.http_postShutterCommand("stop");
  }

  function hClick_Up() {
    httpFetch.http_postShutterCommand("up");
  }

  function hClick_Down() {
    httpFetch.http_postShutterCommand("down");
  }

</script>

<style type="text/scss">
@import "../styles/app.scss";

</style>

<div id="sdi" class="inline-block">
  <div class="flex flex-row items-center">

    <button class="w-24 h-16 text-lg" type="button" on:click={hClick_Down}>
      &#x25bc;
    </button>

    <button class="m-2 w-16 h-16 text-lg" type="button" on:click={hClick_Stop}>
      STOP
    </button>

    <button class="w-24 h-16 text-lg" type="button" on:click={hClick_Up}>
      &#x25b2;
    </button>

  </div>
</div>

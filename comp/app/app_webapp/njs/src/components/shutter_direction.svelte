<script>
  "use strict";
  import * as httpFetch from "app/fetch.js";
  import tippy from "sveltejs-tippy";

  function hClick_RotDir() {
    httpFetch.http_postShutterCommand("rot-dir");
  }

  function hClick_Up() {
    httpFetch.http_postShutterCommand("up");
    setTimeout(() => {
      httpFetch.http_postShutterCommand("stop");
    }, 1000);
  }

  function hClick_Down() {
    httpFetch.http_postShutterCommand("down");
    setTimeout(() => {
      httpFetch.http_postShutterCommand("stop");
    }, 1000);
  }

</script>

<style type="text/scss">
@import "../styles/app.scss";

</style>

<div class="inline-block">
  <div class="flex flex-row items-center">

    <button class="w-16 h-16 text-lg" type="button" on:click={hClick_Down}>
      Test &#x25bc;
    </button>

    <button class="m-2 w-32 h-16 text-lg" type="button" on:click={hClick_RotDir}>
      Toggle Direction
    </button>

    <button class="w-16 h-16 text-lg" type="button" on:click={hClick_Up}>
      Test &#x25b2;
    </button>

  </div>
</div>

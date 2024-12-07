<script>
  "use strict";
  import * as httpFetch from "../app/fetch.js";

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

<style lang="scss">
@use "../styles/app.scss" as *;

</style>

<div id="sdi" class="inline-block">
  <div class="flex flex-row items-center">

    <button class="w-24 h-16 text-lg rounded-l-full" type="button" onclick={hClick_Down}>
      &#x25bc;
    </button>

    <button class="m-2 w-16 h-16 text-lg rounded-full" type="button" onclick={hClick_Stop}>
      STOP
    </button>

    <button class="w-24 h-16 text-lg rounded-r-full" type="button" onclick={hClick_Up}>
      &#x25b2;
    </button>

  </div>
</div>

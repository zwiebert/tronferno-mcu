<script>
  "use strict";
  import * as httpFetch from "app/fetch.js";
  import { G, M0, GMH } from "stores/curr_shutter.js";
  import ShutterGM from "app/shutter_gm.svelte";
  import { SepModeEnabled } from "stores/app_state";
  import SepMode from "app/sep_control.svelte";

  $: upDown_enabled = false;
  let Button_Interval = null;
  $: disable_button_envents = false;
  let sep_auth_key = 4711;

  function hClick_Stop() {
    disable_button_envents = true;
    setTimeout(() => {
      disable_button_envents = false;
    }, 1000);
    clearInterval(Button_Interval);
    httpFetch.http_postCommand({ sep: { c: "stop", "auth-key": sep_auth_key  } });
  }

  function post_Up() {
    httpFetch.http_postCommand({ sep: { c: "up", "auth-key": sep_auth_key  } });
  }

  function post_Down() {
    httpFetch.http_postCommand({ sep: { c: "down", "auth-key": sep_auth_key  } });
  }

  function post_Test() {
    httpFetch.http_postCommand({ sep: { c: "test", "auth-key": sep_auth_key  } });
  }

  function post_KeepMoving() {
    httpFetch.http_postCommand({ sep: { c: "keep-moving", "auth-key": sep_auth_key  } });
  }

  function set_button_interval() {
    clearInterval(Button_Interval);
    Button_Interval = setInterval(post_KeepMoving, 1000);
  }

  function hClick_Test() {
    post_Test();
  }

  function hClick_Up() {
    post_Up();
    set_button_interval();
  }

  function hClick_Down() {
    post_Down();
    set_button_interval();
  }

  function sep_exit() {
    httpFetch.http_postCommand({ sep: { enable: "off" } });
    $SepModeEnabled = false;
  }

  function sep_enable() {
    httpFetch.http_postCommand({ sep: { g: $G, m: $M0, enable: "on", "request-auth": "", "auth-key": sep_auth_key } });

    upDown_enabled = true;
  }

  function sep_disable() {
    httpFetch.http_postCommand({ sep: { enable: "off" } });

    upDown_enabled = false;
  }
</script>

<div class="main-area flex flex-col text-center items-center px-1 border-none">
  <div id="sdi" class="inline-block">
    <ShutterGM />
    <button type="button" on:click={sep_enable}>Enable</button>
    <button type="button" on:click={sep_disable}>Disable</button>
    {#if upDown_enabled}
      <div class="flex flex-row items-center">
        <button class="w-24 h-16 text-lg" type="button" disabled={disable_button_envents} on:mousedown={hClick_Down} on:mouseup={hClick_Stop}>
          &#x25bc;
        </button>

        <button class="m-2 w-16 h-16 text-lg" type="button" disabled={true} on:click={hClick_Test}> Test </button>

        <button class="w-24 h-16 text-lg" type="button" disabled={disable_button_envents} on:mousedown={hClick_Up} on:mouseup={hClick_Stop}> &#x25b2; </button>
      </div>
    {/if}
    <br />
    <button type="button" on:click={sep_exit}>Exit</button>
  </div>
</div>

<style lang="scss">
  @import "../styles/app.scss";
</style>

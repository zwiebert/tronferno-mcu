<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { G, M0, Address, RadioCode, RadioCodeEnabled } from "../store/curr_shutter.js";
  import { onMount, onDestroy } from "svelte";
  import ShutterGM from "../app/shutter_gm.svelte";
  import ShutterMove from "../app/shutter_move.svelte";
  import { Sep, SepMode } from "../store/sep";

  $: upDown_enabled = false;
  let Button_Interval = null;
  $: disable_button_envents = false;
  const sep_auth_key = $SepMode.auth_key;

  $: isValidAddress = $M0 !== 0;
  $: sep_auth_button_was_pressed = false;

  $: isAuthenticated = sep_auth_button_was_pressed;

  let progress_interval = null;
  $: progress_value = 0;
  $: progress_max = 0;

  $: {
    if ($Sep["auth-success"]) {
      auth_button_done(true);
    }
  }

  $: {
    if ($Sep["auth-terminated"]) {
      auth_button_done(false);
    }
  }

  $: {
    if ($Sep["auth-button-error"]) {
      auth_button_done(false);
    }
  }

  $: {
    if ($Sep["ui-timeout"]) {
      upDown_enabled = false;
      delete $Sep["ui-timeout"];
    }
  }

  onMount(() => {
    sep_disable();
  });
  onDestroy(() => {
    sep_disable();
  });

  function auth_button_done(success) {
    isAuthenticated = success;
    sep_auth_button_was_pressed = success;
    clearInterval(progress_interval);
    progress_interval = null;
    progress_value = 0;
  }
  function hClick_Stop() {
    disable_button_envents = true;
    setTimeout(() => {
      disable_button_envents = false;
    }, 1000);
    clearInterval(Button_Interval);
    httpFetch.http_postCommand({ sep: { c: "stop", "auth-key": sep_auth_key } });
  }

  function post_Up() {
    httpFetch.http_postCommand({ sep: { c: "up", "auth-key": sep_auth_key } });
  }

  function post_Down() {
    httpFetch.http_postCommand({ sep: { c: "down", "auth-key": sep_auth_key } });
  }

  function post_Test() {
    httpFetch.http_postCommand({ sep: { c: "test", "auth-key": sep_auth_key } });
  }

  function post_KeepMoving() {
    httpFetch.http_postCommand({ sep: { c: "keep-moving", "auth-key": sep_auth_key } });
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
    $SepMode.enabled = false;
    sep_unauth();
  }

  function sep_auth() {
    $Sep = {};
    httpFetch.http_postCommand({ sep: { "request-auth": "button", "auth-key": sep_auth_key } });
    progress_value = 60;
    progress_max = 60;
    progress_interval = setInterval(() => {
      if (--progress_value < 0) {
        auth_button_done(false);
      }
    }, 1000);
  }

  function sep_unauth() {
    $Sep = {};
    httpFetch.http_postCommand({ sep: { "request-auth": "off", "auth-key": sep_auth_key } });
  }

  function sep_enable() {
    httpFetch.http_postCommand({ sep: { ...$Address, enable: "on", "auth-key": sep_auth_key } });

    upDown_enabled = true;
  }

  function sep_disable() {
    httpFetch.http_postCommand({ sep: { enable: "off" } });

    upDown_enabled = false;
  }
</script>

<div class="w-fit mr-auto ml-auto flex flex-col text-center px-1 border-none">
  <div class="main-area">
    <h5 use:tippy={{ content: $_("app.sep.tt.header") }}>{$_("app.sep.header")}</h5>

    <div class={isAuthenticated ? "bg-green-500" : "bg-red-500"}>
      <p>
        <button
          type="button"
          on:click={sep_auth}
          disabled={!isValidAddress || upDown_enabled || isAuthenticated}
          use:tippy={{ content: $_("app.sep.tt.authenticate") }}>{$_("app.sep.authenticate")}</button
        >
      </p>
      {#if 55 < progress_value}
        <progress value={progress_value - 55} max={5} />
        <p>{$_("app.sep.auth_prog_wait")}</p>
      {:else if 0 < progress_value}
        <progress value={progress_value} max={progress_max} />
        <p>{$_("app.sep.auth_prog_press")}</p>
      {/if}
    </div>

    {#if upDown_enabled}
      <div class="flex flex-row items-center bg-yellow-500">
        <button
          class="w-24 h-16 text-lg"
          type="button"
          disabled={disable_button_envents}
          on:touchstart={hClick_Down}
          on:mousedown={hClick_Down}
          on:touchend={hClick_Stop}
          on:mouseup={hClick_Stop}
        >
          &#x25bc;
        </button>

        <button
          class="w-24 h-16 text-lg"
          type="button"
          disabled={disable_button_envents}
          on:touchstart={hClick_Up}
          on:mousedown={hClick_Up}
          on:mouseup={hClick_Stop}
          on:touchend={hClick_Stop}
        >
          &#x25b2;
        </button>
      </div>
    {:else}
      <div>
        <div class="area">
          <ShutterGM />
          <ShutterMove />
        </div>
      </div>
    {/if}

    <button
      type="button"
      on:click={sep_enable}
      disabled={!isValidAddress || upDown_enabled || !isAuthenticated}
      use:tippy={{ content: $_("app.sep.tt.enable") }}>{$_("app.sep.enable")}</button
    >
    <button type="button" on:click={sep_disable} disabled={!upDown_enabled} use:tippy={{ content: $_("app.sep.tt.disable") }}>{$_("app.sep.disable")}</button>

    <br />
    <button type="button" on:click={sep_exit} use:tippy={{ content: $_("app.sep.tt.exit") }}>{$_("app.sep.exit")}</button>
  </div>
</div>

<style lang="scss">
  @import "../styles/app.scss";
</style>

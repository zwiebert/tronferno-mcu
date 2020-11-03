<script>
  "use strict";
  import * as httpFetch from "../fetch.js";
  import { G, M } from "stores/curr_shutter.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {});
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  const test_cmds = ["up", "down", "stop"];
  function get_randomCmd() {
    const idx = Math.floor(Math.random() * (test_cmds.length - 0.01));
    return test_cmds[idx];
  }

  function test_randomCmd() {
    const cmd = get_randomCmd();
    console.log(cmd);
    httpFetch.http_postShutterCommand(cmd);
  }

  let test_randomCmd_interval = 0;
  function testPressed(enable) {
    if (test_randomCmd_interval) {
      clearInterval(test_randomCmd_interval);
      test_randomCmd_interval = 0;
    }

    if (enable) {
      G.set(7);
      M.set(1);
      test_randomCmd_interval = setInterval(test_randomCmd, 1000);
    }
  }
</script>

Random Periodic Movement Commands:
<button id="test_rpm_start" type="button" on:click={() => testPressed(true)}>
  Test Start
</button>
<button id="test_rpm_stop" type="button" on:click={() => testPressed(false)}>
  Test Stop
</button>

<ul>
  <li>ScreenWidth: {window.screen.width}</li>
  <li>ScreenHeight: {window.screen.height}</li>
  <li>PixelRatio: {window.devicePixelRatio}</li>
</ul>

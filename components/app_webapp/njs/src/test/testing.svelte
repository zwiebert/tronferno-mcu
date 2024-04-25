<script>
  "use strict";
  import * as httpFetch from "../app/fetch.js";
  import { G, M } from "../store/curr_shutter.js";
  import { onMount, onDestroy } from "svelte";
  import * as httpResp from "../app/http_resp.js";
 
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


  export function http_postRequest(url = "", data = {}, state = { retry_count: 0 }) {
  console.log("post-json: " + JSON.stringify(data));

  const fetch_data = {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-Type": "application/json",
    },
    referrer: "no-referrer",
    body: JSON.stringify(data),
  };

  return fetch(url, fetch_data)
    .then((response) => {
      if (!response.ok) {
        console.log("error");
        if (state.retry_count++ < MAX_RETRY_COUNT) {
          return http_postRequest(url, data, state);
        }
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => httpResp.http_handleResponses(json))

    .catch((error) => {
      console.log("error: http_postRequest(): ", error);
    });
}
</script>

<button id="test_rpm_start" type="button" on:click={() => {
  httpFetch.http_fetchByMask(httpFetch.FETCH_SETTINGS_ALL);
  //http_postRequest("/cmd.json", {"mcu":{"test-rj":1}});
}}>
  Fetch All Settings
</button>
<hr>

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

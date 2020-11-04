<script>
'use strict';
import { _ } from "services/i18n";
import * as appDebug from "app/app_debug.js";
import * as httpFetch from "app/fetch.js";
import {Auto,G,M,GM} from "stores/curr_shutter.js";
import { onMount,onDestroy } from "svelte";
import {Autos} from "stores/shutters.js";

let on_destroy = [];
onMount(() => {
    on_destroy.push(Auto.subscribe(auto => automaticOptions_updHtml(auto)));
  });
onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
});

$: {
    $GM;
    httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO | httpFetch.FETCH_SHUTTER_NAME);
  }

function hClick_Reload() {
  httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
}

function hClick_Save() {
  // disable button for 5 seconds while data is being sent to shutter
  // motor by RF
  document.getElementById("asvb").disabled = true;
  setTimeout(() => { document.getElementById("asvb").disabled = false; }, 5000);

  req_automatic();
}


// -------------  auto div -------------------------------


 function automaticOptions_updHtml(auto) {
     auto = auto ? auto : {};
     console.log("auto upd", JSON.stringify(auto));
    document.getElementById('tfti').value = ("f" in auto) ? auto.f : "";

    document.getElementById('tati').value = ("astro" in auto) ? auto.astro : "";
    document.getElementById('twti').value = ("weekly" in auto) ? auto.weekly : "";

    let f = ("f" in auto) ? auto.f : "";
    document.getElementById('tdci').checked = f.indexOf("D") >= 0;
    document.getElementById('twci').checked = f.indexOf("W") >= 0;
    document.getElementById('taci').checked = f.indexOf("A") >= 0;
    document.getElementById('trci').checked = f.indexOf("R") >= 0;
    document.getElementById('tsci').checked = f.indexOf("S") >= 0;
    document.getElementById('tmci').checked = f.indexOf("M") >= 0;

    let up_elem = document.getElementById('tduti');
    let down_elem = document.getElementById('tddti');
    up_elem.value = "";
    down_elem.value = "";

    if ("daily" in auto) {
      let d = auto.daily;
      let l = auto.daily.length;
      up_elem.value = d.startsWith("-") ? "" : d.substring(0, 2) + ":" + d.substring(2, 4);
      down_elem.value = d.endsWith("-") ? "" : d.substring(l - 4, l - 2) + ":" + d.substring(l - 2);
    }
    if ("asmin" in auto) {
      document.getElementById("id_astroTime").innerHTML = "(today: " + Math.floor((auto.asmin / 60)).toString().padStart(2, '0') + ":" + (auto.asmin % 60).toString().padStart(2, '0') + ")";
    } else {
      document.getElementById("id_astroTime").innerHTML = "";
    }

  }

function req_automatic() {
  let url = '/cmd.json';
  let tfmcu = { to: "tfmcu", timer: {} };
  let auto = tfmcu.timer;
  let has_daily = false, has_weekly = false, has_astro = false;

  auto.g = $G;
  auto.m = $M;

  let f = "i";
  f += document.getElementById('tmci').checked ? "M" : "m";
  f += ((has_daily = document.getElementById('tdci').checked)) ? "D" : "d";
  f += ((has_weekly = document.getElementById('twci').checked)) ? "W" : "w";
  f += ((has_astro = document.getElementById('taci').checked)) ? "A" : "a";
  f += document.getElementById('trci').checked ? "R" : "r";
  f += document.getElementById('tsci').checked ? "S" : "s";
  auto.f = f;

  if (has_weekly)
    auto.weekly = document.getElementById('twti').value;
  if (has_astro)
    auto.astro = document.getElementById('tati').value;
  if (has_daily) {
    let tdu = document.getElementById('tduti').value;
    let tdd = document.getElementById('tddti').value;
    let td = tdu.length !== 5 ? "-" : tdu.substring(0, 2) + tdu.substring(3, 5);
    td += tdd.length !== 5 ? "-" : tdd.substring(0, 2) + tdd.substring(3, 5);
    auto.daily = td;
  }

  appDebug.dbLog(JSON.stringify(tfmcu));
  httpFetch.http_postRequest(url, tfmcu);
}
</script>


<style type="text/scss">
@media only screen and (min-device-width: 361px) {
  #twti {
    width:30em;
  }
}
</style>

<div id="autodiv" class="auto">
  <table class="top_table">
    <tr id="tf_tr" style="display:none;">
      <td><label>flags</label></td>
      <td></td>
      <td><input id="tfti" style="width:5em;" type = "text" name = "g" value=""></td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.daily')}</label></td>
      <td><input id="tdci" class="cb" type = "checkbox" value=""></td>
      <td> <label><input id="tduti" type="time" value=""> &#x25b3;</label><br>
           <label><input id="tddti" type="time" value=""> &#x25bd;</label>
      </td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.weekly')}</label></td>
      <td><input id="twci" class="cb" type = "checkbox" value=""></td>
      <td><input id="twti" type = "text" name = "g" value=""></td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.astro')}</label></td>
      <td><input id="taci" class="cb" type = "checkbox" value=""></td>
      <td><input id="tati" style="width:5em;" type = "number" min="-90" max="90" name = "g" value="">
        <label> </label><span id="id_astroTime"></span></td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.random')}</label></td>
      <td><input id="trci" class="cb" type = "checkbox" value=""></td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.sun')}</label></td>
      <td><input id="tsci" class="cb" type = "checkbox" value=""></td>
    </tr>
    <tr>
      <td><label>{$_('app.auto.manual')}</label></td>
      <td><input id="tmci" class="cb" type = "checkbox" value=""></td>
    </tr>
  </table>

  <br>
  <button id="arlb" class="sb" type="button" on:click={hClick_Reload}>{$_('app.reload')}</button>
  <button id="asvb" class="sb" type="button" on:click={hClick_Save}>{$_('app.save')}</button>
  <br>
</div>


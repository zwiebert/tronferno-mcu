<div id="autodiv" class="auto">
  <hr>
  <h3>Automatic</h3>
  <table>
    <tr id="tf_tr">
      <td><label>flags</label></td>
      <td></td>
      <td><input id="tfti" style="width:5em;" type = "text" name = "g" value=""></td>
    </tr>
    <tr>
      <td><label>daily</label></td>
      <td><input id="tdci" class="cb" type = "checkbox" value=""></td>
      <td> &#x25b2;<input id="tduti" type="time" value=""> &#x25bc;<input id="tddti" type="time" value=""></td>
    </tr>
    <tr>
      <td><label>weekly</label></td>
      <td><input id="twci" class="cb" type = "checkbox" value=""></td>
      <td><input id="twti" style="width:50vw;" type = "text" name = "g" value=""></td>
    </tr>
    <tr>
      <td><label>astro</label></td>
      <td><input id="taci" class="cb" type = "checkbox" value=""></td>
      <td><input id="tati" style="width:5em;" type = "number" min="-90" max="90" name = "g" value="">
        <label> </label><span id="id_astroTime"></span></td>
    </tr>
    <tr>
      <td><label>random</label></td>
      <td><input id="trci" class="cb" type = "checkbox" value=""></td>
    </tr>
    <tr>
      <td><label>sun</label></td>
      <td><input id="tsci" class="cb" type = "checkbox" value=""></td>
    </tr>
    <tr>
      <td><label>manual</label></td>
      <td><input id="tmci" class="cb" type = "checkbox" value=""></td>
    </tr>
  </table>

  <br>
  <button id="arlb" class="sb" type="button">Reload</button>
  <button id="asvb" class="sb" type="button">Save</button>
  <br>
</div>


<script context="module">
'use strict';
import * as appDebug from './app_debug.js';
import * as appState from './app_state';
import * as httpFetch from './fetch.js';

export function auto_cbHtml() {
  document.getElementById("arlb").onclick = () => httpFetch.http_fetchByMask(httpFetch.FETCH_AUTO);
  document.getElementById("asvb").onclick = () => {
    // disable button for 5 seconds while data is being sent to shutter
    // motor by RF
    document.getElementById("asvb").disabled = true;
    setTimeout(() => { document.getElementById("asvb").disabled = false; }, 5000);

    req_automatic();
  };
}

// -------------  auto div -------------------------------
function req_automatic() {
  let url = '/cmd.json';
  let tfmcu = { to: "tfmcu", timer: {} };
  let auto = tfmcu.timer;
  let has_daily = false, has_weekly = false, has_astro = false;

  auto.g = appState.ast.g;
  auto.m = appState.ast.m;

  let f = "i";
  f += document.getElementById('tmci').checked ? "M" : "m";
  f += (has_daily = document.getElementById('tdci').checked) ? "D" : "d";
  f += (has_weekly = document.getElementById('twci').checked) ? "W" : "w";
  f += (has_astro = document.getElementById('taci').checked) ? "A" : "a";
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

function clearAuto_updHtml() {
  document.getElementById('tfti').value = "";
  document.getElementById('tduti').value = "";
  document.getElementById('tddti').value = "";
  document.getElementById('tati').value = "";
  document.getElementById('twti').value = "";
  document.getElementById('tdci').checked = false;
  document.getElementById('twci').checked = false;
  document.getElementById('taci').checked = false;
  document.getElementById('trci').checked = false;
  document.getElementById('tsci').checked = false;
  document.getElementById('tmci').checked = false;
}

</script>

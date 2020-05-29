<script context="module">
  'use strict';
  import * as shutterName from './shutter_name.js';
  import * as appState from './app_state.js';
  import * as httpFetch from './fetch.js';

  //------------ cmd div --------------
  function hClick_G() {
  appState.ast.g_next();
  }

  function hClick_M() {
  appState.ast.m_next();
  }
  
  function hClick_P() {
    onPos(document.getElementById("spi").value);
  }

  function hClick_Stop() {
    httpFetch.http_postShutterCommand('stop');
  }
  
  function hClick_Up() {
    httpFetch.http_postShutterCommand('up');
  }
  
  function hClick_Down() {
    httpFetch.http_postShutterCommand('down');
  }
  
  function hClick_Sun() {
    httpFetch.http_postShutterCommand('sun-down');
  }
  
  function hChange_Name() {
    shutterName.shn_fromHtml_toMcu();
  }
  
  function hChange_Pos() {
    onPos(document.getElementById("spr").value);
  }
  
  function onPos(pct) {
  let tfmcu = { to: "tfmcu" };
  tfmcu.send = {
  g: appState.ast.g,
  m: appState.ast.m,
  p: pct,
  };

  let url = '/cmd.json';
  httpFetch.http_postRequest(url, tfmcu);
  }


</script>

<style>

      #senddiv {
      background-color: #eee;
      display: grid;
      width: 100%;
      grid-template-columns: 1fr 1fr 1fr 1fr;
      grid-template-rows: repeat(6, 1fr);
      grid-template-areas:
      "mn  mn  mn  ."
      "gi  pi  mi pr"
      "gb  pl  mb pr"
      ".   ub  .  pr"
      ".   sb  .  pr"
      "pb  db  .  pr"
      ;
      justify-items: center;
      align-items: center;
      grid-gap: 2px;
      }
      
      #senddiv {
      width: 500px;
      }
   
      .sb {width: 5em; margin: 2px; }

      #sgb, #smb { border-radius: 40%; }
      #sgi, #spi, #smi { width: 2.5em; height: 2em; text-align: center; }
      #sgb, #smb { width: 2em; height: 2em; }
      #spr { height: 4em; }

      #ssb { font-size: 60%;  font-stretch: ultra-condensed; height: 4em; border-radius: 100% }
      #sub, #sdb { font-size: 125%; border-radius: 100% }

      #smn { grid-area: mn; }
      #sgi { grid-area: gi; }
      #sgb { grid-area: gb; }
      #spi { grid-area: pi; }
      #spb { grid-area: pl; }
      #smi { grid-area: mi; }
      #smb { grid-area: mb; }
      #sub { grid-area: ub; }
      #ssb { grid-area: sb; }
      #sdb { grid-area: db; }
      #spr { grid-area: pr; }
      #sspb { grid-area: pb; }
      
      input[type="range"] {
      /*position: absolute;*/
      /*top: 40%; */
      transform: rotate(270deg);
      }
      
</style>

<div id="senddiv" class="send">
  <input id="smn" type = "text" name = "name" value="" on:change={hChange_Name}>
  <input id="sgi" type = "text" name = "g" value="A">
  <input id="spi" type = "number" min="0" max="100" name = "p" value="100">
  <input id="spr" type = "range" min="0" max="100" name = "p" value="100" on:change={hChange_Pos}>
  <input id="smi" type = "text" name = "m" value="A">
  <br>
  <button id="sgb" type="button" on:click={hClick_G}>G</button>
  <button id="spb" type="button" on:click={hClick_P}>Position</button>
  <button id="smb" type="button" on:click={hClick_M}>E</button>
  <br><button id="sub" class="sb" type="button" on:click={hClick_Up}>&#x25b2;</button>
  <br><button id="ssb" class="sb" type="button" on:click={hClick_Stop}>STOP</button>
  <br><button id="sdb" class="sb" type="button" on:click={hClick_Down}>&#x25bc;</button>
  <br><button id="sspb" class="sb" type="button" on:click={hClick_Sun}>Sun</button>
</div>
<script>
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import * as httpFetch from "../app/fetch.js";
  import { RfCapture } from "../store/rf.js";

  function do_capture() {
    let tfmcu = { to: "tfmcu", mcu: { "rf-capture": 2046 } };
    httpFetch.http_postCommand(tfmcu);
    setTimeout(() => {
      get_capture();
    }, 1000);
  }

  function get_capture() {
    let tfmcu = { to: "tfmcu", mcu: { "rf-capture": "?" } };
    httpFetch.http_postCommand(tfmcu);
  }

  function bufferToHex (buffer) {
    return [...new Uint8Array (buffer)]
        .map (b => b.toString (16).padStart (2, "0"))
        .join ("");
  }

  function bufferToLevel (buffer) {
    let result = "";
    for (let n of buffer) {
      for (let b=0; b < 8; ++b) {
        let l = n & 1;
        n >>= 1;
        result += l ? "-" : "_";

      }
    }
    return result;
  }

  let rf_cap_hex = $derived(bufferToHex($RfCapture));
  let rf_cap_lvl = $derived(bufferToLevel($RfCapture));

</script>
<div class="main-area text-center">
  <h4 class="text-center" use:tippy={{ content: $_("panes.rfcap.tt.header") }}>{$_("panes.rfcap.header")}</h4>

  <button class="sb" type="button" onclick={do_capture} use:tippy={{ content: $_("panes.rfcap.tt.do_capture") }}>{$_("panes.rfcap.do_capture")}</button>


<div>
    <textarea class="hscfg" value={rf_cap_hex} cols={60} rows={16} disabled={true}></textarea>
</div>

<div>
    <textarea class="hscfg" value={rf_cap_lvl} cols={60} rows={16} disabled={true}></textarea>
</div>

</div>


<script>
  import { AppLogTxt, AppLog } from "stores/app_log.js";

  function htmlEntities(str) {
    let safeString = String(str).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
    return String(safeString).replace(/\n/g, "<br>\n");
  }

  function dummy_fun(x) {

  }
  $: {
    dummy_fun($AppLog);
    console.log("scroll");
    let elem = document.getElementById('app_log_div');
    if (elem)
       elem.scrollTop = elem.scrollHeight;
  }
</script>

<div id="app_log_div" class="w-11/12 h-full border-black border-2 bg-gray-400 overflow-auto">
  {@html htmlEntities($AppLogTxt)}
</div>

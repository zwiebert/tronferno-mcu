<script>
  import { AppLogTxt } from "stores/app_log.js";

  function htmlEntities(str) {
    let safeString = String(str).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
    return String(safeString).replace(/\n/g, "<br>\n");
  }
</script>

<div class="w-11/12 border-black border-2 bg-gray-400 overflow-auto h-screen">
  {@html htmlEntities($AppLogTxt)}
</div>

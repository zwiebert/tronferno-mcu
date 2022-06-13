<script>
  "use strict";

  import { SepMode } from "stores/sep";
  import { GuiUserLevel } from "stores/app_state";

</script>


<select bind:value={$GuiUserLevel}>
  <option value={0}>expert</option>
  <option value={10}>admin</option>
  <option value={20}>user</option>
  <option value={30}>kid</option>
</select>
<script>
  import Pane2411 from "app/ui2411.svelte";
  import { GuiAcc } from "stores/app_state";
  import { MenuCount } from "stores/ui2411.js";
  import ShutterAuto from "app/shutter_auto.svelte";
</script>

<style>
  .div-2411 {
    border: thin solid grey;
    border-radius: 1rem 1rem 10rem 10rem;
  }
  button {
    background-color: white;
    border: thin solid grey;
  }
</style>

<div class="main-area">
  <div class="bg-white p-2 div-2411">
    <Pane2411 />
  </div>
</div>


{#if $MenuCount === 1 && $GuiAcc.shutter_auto}
<div class="area">
  <div class="bg-white rounded-xl">
  <ShutterAuto />
</div>
</div>
{/if}
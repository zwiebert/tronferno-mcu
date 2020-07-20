<script>
  "use strict";
  import { Auto } from "./store/curr_shutter.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    //on_destroy.push(Auto.subscribe(auto => automaticOptions_updHtml(auto)));
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: hasWeekly = "f" in $Auto && $Auto.f.indexOf("W") >= 0;
  $: hasDaily = "f" in $Auto && $Auto.f.indexOf("D") >= 0;
  $: hasDailyUp = "daily" in $Auto && !$Auto.daily.startsWith('-');
  $: hasDailyDown = "daily" in $Auto && !$Auto.daily.endsWith('-');
  $: hasSun = "f" in $Auto && $Auto.f.indexOf("S") >= 0;
  $: hasAstro = "f" in $Auto && $Auto.f.indexOf("A") >= 0;
  $: hasRandom = "f" in $Auto && $Auto.f.indexOf("R") >= 0;
  $: hasManual = "f" in $Auto && $Auto.f.indexOf("M") >= 0;

  import FaLongArrowAltUp from "svelte-icons/fa/FaLongArrowAltUp.svelte";
  import FaLongArrowAltDown from "svelte-icons/fa/FaLongArrowAltDown.svelte";
  import FaRegCalendarAlt from "svelte-icons/fa/FaRegCalendarAlt.svelte";
  import FaSun from "svelte-icons/fa/FaSun.svelte";
  import FaMoon from "svelte-icons/fa/FaMoon.svelte";
  import FaDiceFive from "svelte-icons/fa/FaDiceFive.svelte";
  import FaRegClock from "svelte-icons/fa/FaRegClock.svelte";
  import FaRegHandPaper from "svelte-icons/fa/FaRegHandPaper.svelte";
</script>

<style>

</style>

<div class="flex p-1">
  <div class="h-4 w-4 text-gray-800">
    {#if hasDailyUp}
      <FaLongArrowAltUp />
    {/if}
  </div>
  <div class="h-4 w-4 text-gray-800">
    {#if hasDailyDown}
      <FaLongArrowAltDown />
    {/if}
  </div>
  <div class="h-4 w-4 text-gray-800">
    {#if hasWeekly}
      <FaRegCalendarAlt />
    {/if}
  </div>
  <div class="h-4 w-4 text-yellow-700">
    {#if hasSun}
      <FaSun />
    {/if}
  </div>
  <div class="h-4 w-4 text-yellow-700">
    {#if hasAstro}
      <FaMoon />
    {/if}
  </div>
  <div class="h-4 w-4 text-gray-800">
    {#if hasRandom}
      <FaDiceFive />
    {/if}
  </div>
  <div class="h-4 w-4 text-gray-800">
    {#if false}
      <FaRegClock />
    {/if}
  </div>
  <div class="h-4 w-4 text-gray-800">
    {#if hasManual}
      <FaRegHandPaper />
    {/if}
  </div>
</div>

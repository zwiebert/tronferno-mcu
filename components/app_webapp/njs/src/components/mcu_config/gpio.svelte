<script>
  export let name;
  export let value;

  let gpio_number = parseInt(name.substring(4), 10);
</script>

<select class="config-input" id="cfg_{name}" bind:value={value}>
  <option value="d">Remove GPIO</option>
  <option value="i">In, FloatingGate</option>
  {#if gpio_number < 34}
    <option value="ih">In, PullUp</option>
    <option value="il">In, PullDown</option>
    <option value="Ol">Out, InitLevelLow</option>
    <option value="Oh">Out, InitLevelHigh</option>
    <option value="ol">Out, OD, Low</option>
    <option value="oh">Out, OD, High</option>
    <option value="Ql">In, Out, Low</option>
    <option value="Qh">In, Out, High</option>
    <option value="ql">In, Out, OD, Low</option>
    <option value="qh">In, Out, OD, High</option>
  {/if}
</select>

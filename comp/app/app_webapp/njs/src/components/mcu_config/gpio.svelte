<script>
  export let name;
  export let value;

  let gpio_number = parseInt(name.substring(4), 10);
</script>

<select class="config-input" id="cfg_{name}" bind:value={value}>
  <option value="i">In, FloatingGate</option>
  {#if gpio_number < 34}
    <option value="ih">In, PullUp</option>
    <option value="il">In, PullDown</option>
    <option value="O">Out</option>
    <option value="Ol">Out, InitLevelLow</option>
    <option value="Oh">Out, InitLevelHigh</option>
    <option value="o">Out, OpenDrain</option>
    <option value="ol">Out, OD, Low</option>
    <option value="oh">Out, OD, High</option>
    <option value="Q">In, Out</option>
    <option value="Ql">In, Out, Low</option>
    <option value="Qh">In, Out, High</option>
    <option value="q">In, Out, OD</option>
    <option value="ql">In, Out, OD, Low</option>
    <option value="qh">In, Out, OD, High</option>
  {/if}
</select>

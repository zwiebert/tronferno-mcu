<script>
  export let name;
  export let value;
  export let min;
  export let max;
  export let step;
</script>

<input
  class="config-input number"
  type="number"
  id="cfg_{name}"
  {name}
  {value}
  {min}
  {max}
  {step} />

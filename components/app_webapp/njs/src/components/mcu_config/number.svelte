<script>
  let {
    name,
    value = $bindable(),
    min,
    max,
    step
  } = $props();
</script>

<input class="config-input number" type="number" id="cfg_{name}" {name} bind:value {min} {max} {step} />

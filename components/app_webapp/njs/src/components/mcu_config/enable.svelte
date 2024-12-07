<script>
  let { name, value = $bindable() } = $props();
</script>

<input class="config-input cb" type="checkbox" id="cfg_{name}" {name} bind:checked={value} />

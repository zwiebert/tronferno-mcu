<script>
  export let name;
  export let value;
</script>

<select class="config-input" id="cfg_{name}" bind:value>
  <option value={0}>average</option>
  <option value={1}>not too late or dark</option>
  <option value={2}>not too early or bright</option>
</select>
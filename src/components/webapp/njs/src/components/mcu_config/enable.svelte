<script>
    export let name;
    export let value;
  </script>

<input
class="config-input cb"
type="checkbox"
id="cfg_{name}"
{name}
checked={value} />
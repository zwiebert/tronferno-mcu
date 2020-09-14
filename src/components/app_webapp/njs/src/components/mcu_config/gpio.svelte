<script>
  export let name;
  export let value;
</script>

<select class="config-input" id="cfg_{name}" {value}>
  <option value="i">Input (Pull.FLoating)</option>
  <option value="ih">Input (Pull.Up)</option>
  <option value="il">Input (Pull.Down)</option>
  <option value="O">Output</option>
  <option value="Ol">Output (Level.Low)</option>
  <option value="Oh">Output (Level.High)</option>
  <option value="o">Output (OpenDrain)</option>
  <option value="ol">Output (OpenDrain + Level.Low)</option>
  <option value="oh">Output (OpenDrain + Level.High)</option>
  <option value="Q">Input/Output</option>
  <option value="Ql">Input/Output (Level.Low)</option>
  <option value="Qh">Input/Output (Level.High)</option>
  <option value="q">Input/Output (OpenDrain)</option>
  <option value="ql">Input/Output (OpenDrain + Level.Low)</option>
  <option value="qh">Input/Output (OpenDrain + Level.High)</option>
</select>

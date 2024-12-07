<script>
  "use strict";
  import { _ } from "../services/i18n";
  import * as httpFetch from "../app/fetch.js";
  import { Cc1101Config, Cc1101Status } from "../store/mcu_config.js";
  import { onMount } from "svelte";
  import { AppLogCountRx } from "../store/app_log.js";
  import tippy from "sveltejs-tippy";

  const HTTP_FETCH_MASK = httpFetch.FETCH_CC1101_CONFIG;

  onMount(() => {
    httpFetch.http_fetchByMask(HTTP_FETCH_MASK);
  });

  const CC1101_IOCFG2 = 0x00; // GDO2 Output Pin Configuration
  const CC1101_IOCFG1 = 0x01; // GDO1 Output Pin Configuration
  const CC1101_IOCFG0 = 0x02; // GDO0 Output Pin Configuration
  const CC1101_FIFOTHR = 0x03; // RX FIFO and TX FIFO Thresholds
  const CC1101_SYNC1 = 0x04; // Sync Word, High Byte
  const CC1101_SYNC0 = 0x05; // Sync Word, Low Byte
  const CC1101_PKTLEN = 0x06; // Packet Length
  const CC1101_PKTCTRL1 = 0x07; // Packet Automation Control
  const CC1101_PKTCTRL0 = 0x08; // Packet Automation Control
  const CC1101_ADDR = 0x09; // Device Address
  const CC1101_CHANNR = 0x0a; // Channel Number
  const CC1101_FSCTRL1 = 0x0b; // Frequency Synthesizer Control
  const CC1101_FSCTRL0 = 0x0c; // Frequency Synthesizer Control
  const CC1101_FREQ2 = 0x0d; // Frequency Control Word, High Byte
  const CC1101_FREQ1 = 0x0e; // Frequency Control Word, Middle Byte
  const CC1101_FREQ0 = 0x0f; // Frequency Control Word, Low Byte
  const CC1101_MDMCFG4 = 0x10; // Modem Configuration
  const CC1101_MDMCFG3 = 0x11; // Modem Configuration
  const CC1101_MDMCFG2 = 0x12; // Modem Configuration
  const CC1101_MDMCFG1 = 0x13; // Modem Configuration
  const CC1101_MDMCFG0 = 0x14; // Modem Configuration
  const CC1101_DEVIATN = 0x15; // Modem Deviation Setting
  const CC1101_MCSM2 = 0x16; // Main Radio Control State Machine Configuration
  const CC1101_MCSM1 = 0x17; // Main Radio Control State Machine Configuration
  const CC1101_MCSM0 = 0x18; // Main Radio Control State Machine Configuration
  const CC1101_FOCCFG = 0x19; // Frequency Offset Compensation Configuration
  const CC1101_BSCFG = 0x1a; // Bit Synchronization Configuration
  const CC1101_AGCCTRL2 = 0x1b; // AGC Control
  const CC1101_AGCCTRL1 = 0x1c; // AGC Control
  const CC1101_AGCCTRL0 = 0x1d; // AGC Control
  const CC1101_WOREVT1 = 0x1e; // High Byte Event0 Timeout
  const CC1101_WOREVT0 = 0x1f; // Low Byte Event0 Timeout
  const CC1101_WORCTRL = 0x20; // Wake On Radio Control
  const CC1101_FREND1 = 0x21; // Front End RX Configuration
  const CC1101_FREND0 = 0x22; // Front End TX Configuration
  const CC1101_FSCAL3 = 0x23; // Frequency Synthesizer Calibration
  const CC1101_FSCAL2 = 0x24; // Frequency Synthesizer Calibration
  const CC1101_FSCAL1 = 0x25; // Frequency Synthesizer Calibration
  const CC1101_FSCAL0 = 0x26; // Frequency Synthesizer Calibration
  const CC1101_RCCTRL1 = 0x27; // RC Oscillator Configuration
  const CC1101_RCCTRL0 = 0x28; // RC Oscillator Configuration
  const CC1101_FSTEST = 0x29; // Frequency Synthesizer Calibration Control
  const CC1101_PTEST = 0x2a; // Production Test
  const CC1101_AGCTEST = 0x2b; // AGC Test
  const CC1101_TEST2 = 0x2c; // Various Test Settings
  const CC1101_TEST1 = 0x2d; // Various Test Settings
  const CC1101_TEST0 = 0x2e; // Various Test Settings

  $: cc1101Status = $Cc1101Status || "0000000000000000000000000000";
  $: rssiDbm = get_rssi_dbm(cc1101Status);

  $: cc1101Config = $Cc1101Config;
  $: cc1101ConfigArr = parse_regString($Cc1101Config);

  $: agcctrl2 = cc1101ConfigArr[CC1101_AGCCTRL2] || 0;
  $: agcctrl2_max_dvga_gain = 0;
  $: agcctrl2_max_lna_gain = 0;
  $: agcctrl2_magn_target = 1;

  $: {
    agcctrl2_max_dvga_gain;
    agcctrl2_max_lna_gain;
    agcctrl2_magn_target;
    cc1101ConfigArr[CC1101_AGCCTRL2] = get_reg_value(CC1101_AGCCTRL2);
  }

  $: agcctrl1_agc_lna_priority = 0;
  $: agcctrl1_carrier_sense_rel_thr = 0;
  $: agcctrl1_carrier_sense_abs_thr = 0;
  $: {
    agcctrl1_agc_lna_priority;
    agcctrl1_carrier_sense_rel_thr;
    agcctrl1_carrier_sense_abs_thr;
    cc1101ConfigArr[CC1101_AGCCTRL1] = get_reg_value(CC1101_AGCCTRL1);
  }

  $: agcctrl0_hyst_level = 0;
  $: agcctrl0_wait_time = 0;
  $: agcctrl0_agc_freeze = 0;
  $: agcctrl0_filter_length = 0;
  $: {
    agcctrl0_hyst_level;
    agcctrl0_wait_time;
    agcctrl0_agc_freeze;
    agcctrl0_filter_length;
    cc1101ConfigArr[CC1101_AGCCTRL0] = get_reg_value(CC1101_AGCCTRL0);
  }

  $: modemcfg4_chanbw_em = 0;

  $: modemcfg4_drate_e = 0x06;
  $: modemcfg3_drate_m = 0xa3;

  $: {
    modemcfg4_chanbw_em;
    modemcfg4_drate_e;
    cc1101ConfigArr[CC1101_MDMCFG4] = get_reg_value(CC1101_MDMCFG4);
  }

  $: fsctrl1_freq_if = 0;
  $: {
    fsctrl1_freq_if;
    cc1101ConfigArr[CC1101_FSCTRL1] = get_reg_value(CC1101_FSCTRL1);
  }

  const fosc_mhz = 26.0;
  $: freq_mhz = 0;
  $: freq = 0; //(freq_mhz / fosc_mhz) * 2 ** 16;

  $: {
    cc1101ConfigArr = parse_regString($Cc1101Config);
  }

  $: min_rx = 0;
  $: max_rx = 0;
  $: freq_scan_inProgress = false;

  function get_rssi_dbm(statusString) {
    const RSSI_offset = 74;
    const RSSI_dec = Number.parseInt(statusString.substr(2 * (0x34 - 0x30), 2), 16);
    return RSSI_dec >= 128 ? (RSSI_dec - 256) / 2 - RSSI_offset : RSSI_dec / 2 - RSSI_offset;
  }
  function fetch_status() {
    let tfmcu = { to: "tfmcu", mcu: { "cc1101-status": "?" } };
    httpFetch.http_postCommand(tfmcu);
  }
  let fetchStatusTimer = null;

  function upd_reg_value(regAddr) {
    const res = get_reg_value(regAddr);
    if (res === -1) return;
    cc1101ConfigArr[regAddr] = res;
  }

  function get_reg_value(regAddr) {
    let result = 0;
    switch (regAddr) {
      case CC1101_AGCCTRL2:
        result |= agcctrl2_max_dvga_gain << 6;
        result |= agcctrl2_max_lna_gain << 3;
        result |= agcctrl2_magn_target << 0;
        break;

      case CC1101_AGCCTRL1:
        result |= agcctrl1_agc_lna_priority << 6;
        result |= agcctrl1_carrier_sense_rel_thr << 4;
        result |= 0b1111 & (agcctrl1_carrier_sense_abs_thr << 0);
        break;

      case CC1101_AGCCTRL0:
        result |= agcctrl0_hyst_level << 6;
        result |= agcctrl0_wait_time << 4;
        result |= agcctrl0_agc_freeze << 2;
        result |= agcctrl0_filter_length << 0;
        break;

      case CC1101_MDMCFG4:
        result |= modemcfg4_chanbw_em << 4;
        result |= modemcfg4_drate_e << 0;
        break;

      case CC1101_FSCTRL1:
        result |= fsctrl1_freq_if << 0;
        break;

      case CC1101_FREQ2:
        result = (freq >> 16) & 0xff;
        break;
      case CC1101_FREQ1:
        result = (freq >> 8) & 0xff;
        break;
      case CC1101_FREQ0:
        result = freq & 0xff;
        break;

      default:
        return -1;
        break;
    }

    return result;
  }

  function set_reg_value(regAddr, regVal) {
    switch (regAddr) {
      case CC1101_AGCCTRL2:
        agcctrl2_max_dvga_gain = 0b11 & (regVal >> 6);
        agcctrl2_max_lna_gain = 0b111 & (regVal >> 3);
        agcctrl2_magn_target = 0b111 & (regVal >> 0);
        break;

      case CC1101_AGCCTRL1:
        agcctrl1_agc_lna_priority = 0b1 & (regVal >> 6);
        agcctrl1_carrier_sense_rel_thr = 0b11 & (regVal >> 4);
        agcctrl1_carrier_sense_abs_thr = 0b1111 & (regVal >> 0);
        break;

      case CC1101_AGCCTRL0:
        agcctrl0_hyst_level = 0b11 & (regVal >> 6);
        agcctrl0_wait_time = 0b11 & (regVal >> 4);
        agcctrl0_agc_freeze = 0b11 & (regVal >> 2);
        agcctrl0_filter_length = 0b11 & (regVal >> 0);
        break;

      case CC1101_MDMCFG4:
        modemcfg4_chanbw_em = 0b1111 & (regVal >> 4);
        modemcfg4_drate_e = 0b1111 & (regVal >> 0);
        break;

      case CC1101_FSCTRL1:
        fsctrl1_freq_if = 0b1111 & (regVal >> 0);
        break;

      case CC1101_FREQ2:
        freq = (regVal << 16) | (freq & 0x00ffff);
        break;
      case CC1101_FREQ1:
        freq = (regVal << 8) | (freq & 0xff00ff);
        break;
      case CC1101_FREQ0:
        freq = regVal | (freq & 0xffff00);
        break;

      default:
        return;
    }
    cc1101ConfigArr[regAddr] = regVal;
  }

  function create_regArr() {
    let res = [];
    freq = (freq_mhz / fosc_mhz) * 2 ** 16;
    freq_updMhz();
    for (let i = 0; i < 48; ++i) {
      const rv = get_reg_value(i);
      res.push(rv === -1 ? null : rv);
    }
    return res;
  }

  function parse_regString(rs) {
    if (!rs) return [];

    let res = [];
    for (let i = 0; i < rs.length; i += 2) {
      const rv = Number.parseInt(rs.substr(i, 2), 16);
      res.push(rv);
    }
    return res;
  }

  function create_regString(ra) {
    let res = "";
    for (let i = 0; i < 48; ++i) {
      const rv = ra[i];
      res += rv === null ? "__" : ("0" + ra[i].toString(16)).substr(-2);
    }

    return res;
  }

  function freq_updMhz() {
    freq_mhz = (fosc_mhz / 2 ** 16) * freq;
  }

  function revert_gui() {
    if (cc1101Config) {
      const ra = parse_regString(cc1101Config);
      for (let i = 0; i < ra.length; ++i) {
        set_reg_value(i, ra[i]);
      }
      freq_updMhz();
    }
  }

  $: {
    if (cc1101Config) {
      const ra = parse_regString(cc1101Config);
      for (let i = 0; i < ra.length; ++i) {
        set_reg_value(i, ra[i]);
      }
      freq_updMhz();
    }
  }

  function save_changes() {
    const rs = create_regString(create_regArr());
    httpFetch.http_postCommand({ to: "tfmcu", config: { "cc1101-config": rs } });
    setTimeout(() => {
      httpFetch.http_fetchByMask(HTTP_FETCH_MASK);
    }, 500);
  }

  function reset_to_defaults() {
    httpFetch.http_postCommand({ to: "tfmcu", config: { "cc1101-config": "" } });
    setTimeout(() => {
      httpFetch.http_fetchByMask(HTTP_FETCH_MASK);
    }, 500);
  }

  function apply_changes() {
    const rs = create_regString(create_regArr());
    httpFetch.http_postCommand({ to: "tfmcu", mcu: { "cc1101-config": rs } });
    setTimeout(() => {
      httpFetch.http_fetchByMask(HTTP_FETCH_MASK);
    }, 500);
  }

  function reload() {
    cc1101ConfigArr = parse_regString($Cc1101Config);
    revert_gui();
    httpFetch.http_fetchByMask(HTTP_FETCH_MASK);
    httpFetch.http_postCommand({ to: "tfmcu", mcu: { "cc1101-config": "?" } });
  }

  /**
   * \brief   scan for reception while changing carrier frequency
   * \param   min_mhz  start the scan at this frequency
   * \param   max_mhz  stop the scan at this frequency
   * \param   step_mhz increment frequency by this amount at each step
   * \param   step_ms  increment frequency after this timeout
   */
  function freq_scan(min_mhz, max_mhz, step_mhz, step_ms) {
    if (freq_scan_inProgress) {
      freq_scan_inProgress = false;
      return;
    }
    freq_scan_inProgress = true;
    const saved_mhz = freq_mhz;
    const saved_chanbw = modemcfg4_chanbw_em;

    modemcfg4_chanbw_em = 0b1111; // select smallest bandwith
    min_rx = 0;
    max_rx = 0;

    let current_mhz = min_mhz;
    const start_rx_count = $AppLogCountRx;
    let rx_count = $AppLogCountRx;

    let fun = () => {
      if (!freq_scan_inProgress) {
        freq_mhz = saved_mhz;
        modemcfg4_chanbw_em = saved_chanbw;
        apply_changes();
        return;
      }
      freq_mhz = current_mhz;
      if (min_rx === 0 && $AppLogCountRx > start_rx_count + 1) {
        min_rx = current_mhz;
      }

      if (rx_count < $AppLogCountRx) {
        max_rx = current_mhz;
      }
      rx_count = $AppLogCountRx;
      current_mhz += step_mhz;
      apply_changes();

      if (current_mhz > max_mhz) {
        freq_mhz = saved_mhz;
        if (min_rx && max_rx) {
          freq_mhz = (min_rx + max_rx) / 2;
        }
        modemcfg4_chanbw_em = saved_chanbw;
        save_changes();
        freq_scan_inProgress = false;
      } else {
        setTimeout(fun, step_ms);
      }
    };

    fun();
  }
</script>

<h5 class="bg-green-200 text-center">Experimental Register Editor</h5>
<div class="flex flex-col content-center justify-center">
  <div class="area">
    <h4>AGC_Control</h4>
    <table>
      <tbody>
        <tr> <td>AGCCTRL2</td><td>{(cc1101ConfigArr[CC1101_AGCCTRL2] || 0).toString(16)}</td></tr>
        <tr>
          <td use:tippy={{ content: "Reduces the maximum allowable DVGA gain." }}>MAX_DVGA_GAIN</td>
          <td>
            <select class="w-44" bind:value={agcctrl2_max_dvga_gain}>
              <option value={0}>All gain can be used</option>
              <option value={1}>The highest gain setting can not be used</option>
              <option value={2}>The 2 highest gain setting can not be used</option>
              <option value={3}>The 3 highest gain setting can not be used</option>
            </select>
          </td>
        </tr>
        <tr>
          <td use:tippy={{ content: "Sets the maximum allowable LNA + LNA 2 gain relative to the maximum possible gain." }}>MAX_LNA_GAIN</td>
          <td>
            <select class="w-44" bind:value={agcctrl2_max_lna_gain}>
              <option value={0}>Maximum possible LNA + LNA 2 gain</option>
              <option value={1}>Approx. 2.6 dB below maximal possible gain</option>
              <option value={2}>Approx. 6.1 dB below maximal possible gain</option>
              <option value={3}>Approx. 7.4 dB below maximal possible gain</option>
              <option value={4}>Approx. 9.2 dB below maximal possible gain</option>
              <option value={5}>Approx. 11.5 dB below maximal possible gain</option>
              <option value={6}>Approx. 14.6 dB below maximal possible gain</option>
              <option value={7}>Approx. 17.1 dB below maximal possible gain</option>
            </select>
          </td>
        </tr>

        <tr>
          <td use:tippy={{ content: "These bits set the target value for the averaged amplitude from the digital channel filter (1 LSB = 0 dB)." }}
            >MAGN_TARGET</td
          >
          <td>
            <select bind:value={agcctrl2_magn_target}>
              <option value={0}>24 dB</option>
              <option value={1}>27 dB</option>
              <option value={2}>30 dB</option>
              <option value={3}>33 dB</option>
              <option value={4}>36 dB</option>
              <option value={5}>38 dB</option>
              <option value={6}>40 dB</option>
              <option value={7}>42 dB</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>
    <br />
    <table>
      <tbody>
        <tr> <td>AGCCTRL1</td><td>{(cc1101ConfigArr[CC1101_AGCCTRL1] || 0).toString(16)}</td></tr>
        <tr>
          <td
            use:tippy={{
              content:
                "Selects between two different strategies for LNA and LNA 2 gain adjustment. When 1, the LNA gain is decreased first. When 0, the LNA 2 gain is decreased to minimum before decreasing LNA gain.",
            }}>AGC_LNA_PRIORITY</td
          >
          <td>
            <select class="w-36" bind:value={agcctrl1_agc_lna_priority}>
              <option value={0}>LNA2 gain decreased first</option>
              <option value={1}>LNA gain decreased first</option>
            </select>
          </td>
        </tr>

        <tr>
          <td use:tippy={{ content: "Sets the relative change threshold for asserting carrier sense" }}>CARRSENS_REL_THR</td>
          <td>
            <select class="w-36" bind:value={agcctrl1_carrier_sense_rel_thr}>
              <option value={0}>Relative carrier sense threshold disabled</option>
              <option value={1}>6 db increase in RSSI value</option>
              <option value={2}>10 db increase in RSSI value</option>
              <option value={3}>14 db increase in RSSI value</option>
            </select>
          </td>
        </tr>

        <tr>
          <td
            use:tippy={{
              content:
                "Sets the absolute RSSI threshold for asserting carrier sense. The 2-complement signed threshold is programmed in steps of 1 dB and is relative to the MAGN_TARGET setting.",
            }}>CARRSENS_ABS_THR</td
          >
          <td>
            <select class="w-36" bind:value={agcctrl1_carrier_sense_abs_thr}>
              <option value={-8}>Absolute carrier sense threshold disabled</option>
              <option value={-7}>7 dB below MAGN_TARGET setting</option>
              <option value={-6}>6 dB below MAGN_TARGET setting</option>
              <option value={-5}>5 dB below MAGN_TARGET setting</option>
              <option value={-4}>4 dB below MAGN_TARGET setting</option>
              <option value={-3}>3 dB below MAGN_TARGET setting</option>
              <option value={-2}>2 dB below MAGN_TARGET setting</option>
              <option value={-1}>1 dB below MAGN_TARGET setting</option>
              <option value={0}>At MAGN_TARGET setting</option>
              <option value={1}>1 dB above MAGN_TARGET setting</option>
              <option value={2}>2 dB above MAGN_TARGET setting</option>
              <option value={3}>3 dB above MAGN_TARGET setting</option>
              <option value={4}>4 dB above MAGN_TARGET setting</option>
              <option value={5}>5 dB above MAGN_TARGET setting</option>
              <option value={6}>6 dB above MAGN_TARGET setting</option>
              <option value={7}>7 dB above MAGN_TARGET setting</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>
    <br />
    <table>
      <tbody>
        <tr> <td>AGCCTRL0</td><td>{(cc1101ConfigArr[CC1101_AGCCTRL0] || 0).toString(16)}</td></tr>
        <tr>
          <td use:tippy={{ content: "Sets the level of hysteresis on the magnitude deviation (internal AGC signal that determine gain changes)." }}
            >HYST_LEVEL</td
          >
          <td>
            <select class="w-44" bind:value={agcctrl0_hyst_level}>
              <option value={0}>No hysteresis, small symmetric dead zone, high gain</option>
              <option value={1}>Low hysteresis, small symmetric dead zone, medium gain</option>
              <option value={2}>Medium hysteresis, medium symmetric dead zone, medium gain</option>
              <option value={3}>Large hysteresis, large symmetric dead zone, low gain</option>
            </select>
          </td>
        </tr>

        <tr>
          <td
            use:tippy={{
              content:
                "Sets the number of channel filter samples from a gain adjustment has been made until the AGC algorithm starts accumulating new samples.",
            }}>WAIT_TIME</td
          >
          <td>
            <select class="w-44" bind:value={agcctrl0_wait_time}>
              <option value={0}>8</option>
              <option value={1}>16</option>
              <option value={2}>24</option>
              <option value={3}>32</option>
            </select>
          </td>
        </tr>

        <tr>
          <td use:tippy={{ content: "Control when the AGC gain should be frozen." }}>AGC_FREEZE</td>
          <td>
            <select class="w-44" bind:value={agcctrl0_agc_freeze}>
              <option value={0}>Normal operation. Always adjust gain when required.</option>
              <option value={1}>The gain setting is frozen when a sync word has been found.</option>
              <option value={2}>Manually freeze the analogue gain setting and continue to adjust the digital gain.</option>
              <option value={3}>Manually freezes both the analogue and the digital gain setting. Used for manually overriding the gain.</option>
            </select>
          </td>
        </tr>

        <tr>
          <td use:tippy={{ content: "Sets the OOK/ASK decision boundary for OOK/ASK reception." }}>FILTER_LENTGTH</td>
          <td>
            <select class="w-44" bind:value={agcctrl0_filter_length}>
              <option value={0}>4 dB OOK/ASK decision boundary</option>
              <option value={1}>8 dB OOK/ASK decision boundary</option>
              <option value={2}>12 dB OOK/ASK decision boundary</option>
              <option value={3}>16 dB OOK/ASK decision boundary</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>
  </div>
  <div class="area">
    <h4>Modem Configuration</h4>
    <table>
      <tbody>
        <tr> <td>MODEMCFG4</td><td>{(cc1101ConfigArr[CC1101_MDMCFG4] || 0).toString(16)}</td></tr>
        <tr>
          <td use:tippy={{ content: "Sets the decimation ratio for the delta-sigma ADC input stream and thus the channel bandwidth." }}>CHANBW</td>
          <td>
            <select bind:value={modemcfg4_chanbw_em}>
              <option value={0b1111}>58 kHz</option>
              <option value={0b1110}>68 kHz</option>
              <option value={0b1101}>81 kHz</option>
              <option value={0b1100}>102 kHz</option>
              <option value={0b1011}>116 kHz</option>
              <option value={0b1010}>135 kHz</option>
              <option value={0b1001}>162 kHz</option>
              <option value={0b1000}>203 kHz</option>
              <option value={0b0111}>232 kHz</option>
              <option value={0b0110}>270 kHz</option>
              <option value={0b0101}>325 kHz</option>
              <option value={0b0100}>406 kHz</option>
              <option value={0b0011}>464 kHz</option>
              <option value={0b0010}>541 kHz</option>
              <option value={0b0001}>650 kHz</option>
              <option value={0b0000}>812 kHz</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>
  </div>

  <div class="area">
    <h4>FSCTRL</h4>
    <table>
      <tbody>
        <tr> <td>FSCTRL1</td><td>{(cc1101ConfigArr[CC1101_FSCTRL1] || 0).toString(16)}</td></tr>
        <tr>
          <td
            use:tippy={{
              content:
                "The desired IF frequency to employ in RX. Subtracted from FS base frequency in RX and controls the digital complex mixer in the demodulator. (See datasheet for equation). The default value gives an IF frequency of 381kHz, assuming a 26.0 MHz crystal.",
            }}>FREQ_IF</td
          >
          <td>
            <select bind:value={fsctrl1_freq_if}>
              <option value={1}>25.39 kHz</option>
              <option value={2}>50.78 kHz</option>
              <option value={3}>76.17 kHz</option>
              <option value={4}>101.56 kHz</option>
              <option value={5}>126.95 kHz</option>
              <option value={6}>152.34 kHz</option>
              <option value={7}>177.73 kHz</option>
              <option value={8}>203.12 kHz</option>
              <option value={9}>228.515 kHz</option>
              <option value={10}>253.91 kHz</option>
              <option value={11}>279.3 kHz</option>
              <option value={12}>304.69 kHz</option>
              <option value={13}>330.08 kHz</option>
              <option value={14}>355.47 kHz</option>
              <option value={15}>380.86 kHz</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</div>

<div class="area">
  <h4>Carrier Frequency</h4>
  <table>
    <tbody>
      <tr>
        <td>MHz</td>
        <td><input type="number" min="430" max="440" step="0.0001" bind:value={freq_mhz} /></td>
      </tr>
      <tr><td>Min</td><td>{min_rx}</td></tr>
      <tr><td>Max</td><td>{max_rx}</td></tr>
      <tr><td>Middle</td><td>{(min_rx + max_rx) / 2}</td></tr>
    </tbody>
  </table>

  <button
    type="button"
    use:tippy={{
      content: "Scan frequency of original 2411 and use that frequency as CC1101 carrier frequency. Hold the STOP button on your 2411 during the entire scan!",
    }}
    on:click={() => {
      freq_scan(433.85, 433.99, 0.005, 750);
    }}>scan</button
  >
  {#if freq_scan_inProgress}
    <h5>Hold STOP button on 2411 central unit</h5>
  {/if}
</div>

<div class="area">
  <table>
    <tbody>
      <tr><td>StatusRegs</td><td class="text-sm font-mono">{cc1101Status}</td></tr>
      <tr><td>RSSI (in dBm)</td><td>{rssiDbm}</td></tr>
    </tbody>
  </table>

  <button
    class={fetchStatusTimer === null ? "" : "text-red-800"}
    type="button"
    on:click={() => {
      if (fetchStatusTimer === null) {
        fetchStatusTimer = setInterval(() => {
          fetch_status();
        }, 750);
      } else {
        clearInterval(fetchStatusTimer);
        fetchStatusTimer = null;
      }
    }}>Fetch Status Periodically</button
  >
</div>

<button
  type="button"
  use:tippy={{ content: "Modify CC1101 config registers, but don't save them permanently (These changes will be lost after MCU Restart)" }}
  on:click={apply_changes}>Apply Changes</button
>
<br />
<button type="button" use:tippy={{ content: "Get current CC1101 Config Registers from MCU" }} on:click={reload}>{$_("app.reload")}</button>
<button type="button" use:tippy={{ content: "Save CC1101 config registers to MCU permanently" }} on:click={save_changes}>{$_("app.save")}</button>

<button type="button" use:tippy={{ content: "Erase saved CC1101 configuration registers on MCU" }} on:click={reset_to_defaults}>Reset to Defaults</button>
<br />

<style lang="scss">
  @use "../styles/app.scss" as *;
  table,
  td,
  tr {
    border-color: $color_border_main_area;
    border-style: solid;
    border-width: 1px;
    border-radius: 0.75rem;
    overflow: hidden;
    border-collapse: collapse;
    margin: 0rem;
    padding: 0rem 0.25rem;
  }
</style>

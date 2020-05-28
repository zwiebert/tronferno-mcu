'use strict';
// src/main.js
//import { version } from '../package.json';
import { onContentLoaded } from './misc.js';
import * as testing from './testing.js';

export default function() {
  //console.log('version ' + version);
  testing.testing_genHtml();
  onContentLoaded();
  testing.testing_netota();
}

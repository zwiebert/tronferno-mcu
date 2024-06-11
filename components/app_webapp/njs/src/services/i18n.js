import { getLocaleFromNavigator, init, addMessages, _, json } from "svelte-i18n";
import {en} from "../services/i18n_en";
import {de} from "../services/i18n_de";


function setupI18n() {
  addMessages("en", en);
  addMessages("de", de);

  init({
    fallbackLocale: "en",
    initialLocale: getLocaleFromNavigator(),
  });
}

export { _, setupI18n, json };

#!/bin/sh

xxd -i tfmcu.html | sed 's/unsigned/const/' > tfmcu_html.h
xxd -i tfmcu.js | sed 's/unsigned/const/' > tfmcu_js.h


#!/bin/sh

PATH=$PATH:$IDF_PATH/tools


if ! command -v idf.py >/dev/null 2>&1
then
   if test -f "$IDF_TOOLS_EXPORT_CMD" 
   then
    echo "variable exists: IDF_TOOLS_EXPORT_CMD=<$IDF_TOOLS_EXPORT_CMD>"
    unset IDF_PYTHON_ENV_PATH
    . "$IDF_TOOLS_EXPORT_CMD" 
   elif test -n "$IDF_PATH" -a -f "$IDF_PATH/export.sh" 
   then
     echo "variable exists: IDF_PATH=<$IDF_PATH>"
       export IDF_PATH_FORCE=1
       unset IDF_PYTHON_ENV_PATH
       . "$IDF_PATH/export.sh"
   else
     echo "using path <$HOME/esp/esp-idf>"
       export IDF_PATH="$HOME/esp/esp-idf"
       export IDF_PATH_FORCE=1
       unset IDF_PYTHON_ENV_PATH
       . "$IDF_PATH/export.sh"
   fi
fi

if [ $? -eq 0 ]; then
  idf.py $@
else
     echo "Could not export ESP-IDF"
     exit 1
fi



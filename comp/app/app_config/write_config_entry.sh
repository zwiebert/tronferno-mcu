#!/bin/sh

cfg_prefix=$1
cfg_item=$2
cfg_item_tf=$cfg_prefix$cfg_item
cfg_item_config=CONFIG_${cfg_prefix}${cfg_item}
cfg_item_no=NO_$cfg_item
cfg_item_cxx=$(echo $cfg_item | sed s/USE_/use_/)

cat <<EOF
#ifdef $cfg_item_config
#define  $cfg_item
#ifdef __cplusplus
constexpr bool $cfg_item_cxx = true;
#else
#define $cfg_item_cxx 1
#endif
#else
#define $cfg_item_no
#ifdef __cplusplus
constexpr bool $cfg_item_cxx = false;
#else
#define $cfg_item_cxx 0
#endif
#endif
EOF

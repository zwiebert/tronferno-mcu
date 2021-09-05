/*
 * \file   fernotron/repeater/repeater.hh
 * \brief repeat RF commands of specified senders
 */

#pragma once

#include <stdint.h>
#include <fernotron_trx/raw/fer_msg_plain.h>

#ifdef __cplusplus
  extern "C" {
#endif


bool ferRep_repeatCommand(struct Fer_Cmd cmd);

/**
 * \brief Setup module
 * \param List of configured senders
 */
bool ferRep_setup(const char *senders);


#ifdef __cplusplus
  }
#endif

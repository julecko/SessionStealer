#ifndef SESSIONSTEALER_UTILS_H
#define SESSIONSTEALER_UTILS_H

#include "sessionstealer/cli_args.h"
#include "dlls/discovery/discovery.h"

const char *enum_browser_to_char(discovery_browser_name_t e);
discovery_browser_t *pick_browser(discovery_browser_list_t *discovered_browsers, cli_args_browser_t args_browser);

#endif

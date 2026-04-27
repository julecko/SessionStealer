#ifndef DLL_EDGE_H
#define DLL_EDGE_H

#include "dlls/discovery/discovery.h"
#include "dlls/chromium/chromium_exports.h"

extern connect_websocket_fn connect_websocket_ptr;
extern ws_send_fn ws_send_ptr;
extern ws_recv_fn ws_recv_ptr;
extern close_websocket_fn close_websocket_ptr;

int import_browser_internal(const discovery_browser_t *browser, const char *filepath);
int export_browser_internal(const discovery_browser_t *browser, const char *filepath);

#endif

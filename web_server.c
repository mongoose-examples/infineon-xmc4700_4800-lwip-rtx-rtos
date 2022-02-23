#include <DAVE.h>
#include "mongoose.h"

#define MG_THREAD_PRIORITY   (osPriorityLow)
#define MG_THREAD_STACK_SIZE      (4096)

static struct mg_mgr mgr;
static const char *s_listening_address = "http://192.168.68.143:8000";
sys_thread_t mg_thread;

static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    mg_http_reply(c, 200, "", "hello, %s!\n", "world");
  }
  (void) fn_data;
  (void) ev_data;
}

static void mg_serve_thread(void *arg) {
  (void)arg;

  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, s_listening_address, cb, &mgr);

  for(;;) {
	mg_mgr_poll(&mgr, 10);
  }
}

void mongoose_init() {
  mg_thread = sys_thread_new("mg_thread", mg_serve_thread, NULL, MG_THREAD_STACK_SIZE, (int)MG_THREAD_PRIORITY);
}

#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
}

void fb_write(const void *buf, off_t offset, size_t len) {
  int index, screen_x, screen_y;
  int w = 0; 
  int h = 0;
  getScreen(&w, &h);
  for(int i = 0; i < len / 4; i++) {
    index = offset / 4 + i;
    screen_y = index / w;
    screen_x = index % w;
    _draw_rect(buf + i *4, screen_x, screen_y, 1, 1);
  }
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}

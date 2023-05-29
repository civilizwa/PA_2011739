#include "common.h"
#include "memory.h"
#include "fs.h"

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
#define DEFAULT_ENTRY ((void *)0x8048000)

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  int size = fs_filesz(fd);
  int ppnum=size/PGSIZE;
  if(size%PGSIZE!=0){
    ppnum++;
  }
  void *pa;
  void *va;

  Log("loaded: [%d]%s size:%d", fd, filename, nbyte);

 for(int i=0;i<ppnum;i++){
  pa=new_page();
  _map(as,va,pa);
  fs_read(fd,pa,PGSIZE);
  va++PGSIZE;
 }

  return (uintptr_t)DEFAULT_ENTRY;
}

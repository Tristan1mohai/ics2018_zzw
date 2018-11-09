#include "common.h"
#include "syscall.h"

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_exit : _halt(c->GPR2); break;
		case SYS_yield: _yield(); c->eax = 0; break;
		case SYS_write: 
				Log("write: %s", (char *)c->GPR3);
				if (c->GPR2==1||c->GPR2==2){
						for (size_t i=0;i<c->GPR4;++i){
								_putc(*(char *)(c->GPR3+i));
						}
				} else {
						assert(0);
				}
				c->eax = c->GPR4;
				break;
		case SYS_brk:
				c->eax = 0;
				break;
		case SYS_open:
		    c->eax = fs_open((char *)c->ebx, (int)c->ecx, (int)c->edx);
				break;
		case SYS_close:
				c->eax = fs_close(c->ebx);
				break;
		case SYS_read:
				TODO();
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

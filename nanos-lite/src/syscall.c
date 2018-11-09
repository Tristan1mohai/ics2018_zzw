#include "common.h"
#include "syscall.h"
#include "fs.h"


_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_exit : _halt(c->GPR2); break;
		case SYS_yield: _yield(); c->eax = 0; break;
		case SYS_write: 
				//Log("write: %s", (char *)c->GPR3);
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
				c->eax = fs_close((int)c->ebx);
				break;
		case SYS_lseek:
				c->eax = fs_lseek((int)c->ebx, (off_t)c->ecx, (int)c->edx);
				break;
		case SYS_read:
				c->eax = fs_read((int)c->ebx, (void *)c->ecx, (size_t)c->edx);
				break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

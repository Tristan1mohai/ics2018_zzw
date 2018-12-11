#include <am.h>
#include <x86.h>
#define mylog(v) \
		printf(# v "= %#x\n", tf->v)
static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecsys();
void vecnull();

_Context* irq_handle(_Context *tf) {
  _Context *next = tf;
	//XXX: Cannot Print eflags in difftest

  if (user_handler) {
    _Event ev = {0};
    switch (tf->irq) {
      case 0x80: ev.event = _EVENT_SYSCALL; break;
      case 0x81: ev.event = _EVENT_YIELD; break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  printf("In kontext\n");
  _Context *c = (_Context*)stack.end - 1;
  c->eip= (intptr_t)entry;
  c->cs = 8;
  return c;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}

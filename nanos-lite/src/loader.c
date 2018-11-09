#include "proc.h"

#define DEFAULT_ENTRY 0x4000000
uint8_t buffer[0x300000];

static uintptr_t loader(PCB *pcb, const char *filename) {
	size_t size = get_ramdisk_size();
	Log("Size of ramdisk = %u", size);
	ramdisk_read(buffer, 0, size);
	Log("1");
	memcpy((uintptr_t *)DEFAULT_ENTRY, buffer, size);
	Log("ramdisk initialized!");
  
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
	
	rtl_update_ZFSF(&t0,id_dest->width);
	
	rtl_xor(&t0,&t0,&t0);
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
	operand_write(id_dest,&t0);

	rtl_update_ZFSF(&t0,id_dest->width);
	
	rtl_xor(&t0,&t0,&t0);
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
	operand_write(id_dest,&t0);

	rtl_update_ZFSF(&t0,id_dest->width);
	
	rtl_xor(&t0,&t0,&t0);
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
	operand_write(id_dest,&t0);

	rtl_update_ZFSF(&t0,id_dest->width);
	
	rtl_xor(&t0,&t0,&t0);
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);	
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t0, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&t0, &id_dest->val);
	operand_write(id_dest, &t0);

  print_asm_template1(not);
}

make_EHelper(ror) {
  rtl_mv(&t0, &id_src->val);
	rtl_mv(&t2, &id_dest->val);
	while (t0--){
			rtl_andi(&t1, &t2, 0x1);
			rtl_set_CF(&t1);
		  t2 = (t2>>1)+(t1<<((id_dest->width)-1));
  }	
	operand_write(id_dest, &t2);

	print_asm_template2(ror);
}

make_EHelper(rol){
  rtl_mv(&t0, &id_src->val);
	rtl_mv(&t2, &id_dest->val);
	while (t0--){
			rtl_msb(&t1, &t2, id_dest->width);
			rtl_set_CF(&t1);
			t2 = (t2<<1) + (t1);
	}
	operand_write(id_dest, &t2);

	print_asm_template2(rol);
}

make_EHelper(rcr){
		TODO();
}

make_EHelper(rcl){
		TODO();
}

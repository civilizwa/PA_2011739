#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// control.c
make_EHelper(call);
make_EHelper(ret); 
make_EHelper(jcc); 
make_EHelper(jmp); 
make_EHelper(call_rm);
make_EHelper(jmp_rm);

// data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(leave);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(mov_cr2r);
make_EHelper(mov_r2cr);

#include "op.hpp"

int Monop::ready() 
{
    if(slotA->ready()){
	val = op->fn(slotA->val);
	delete slotA;
	return 1;
    } else return 0;
}

int Binop::ready()
{
    if(slotA->ready() + slotB->ready() == 2){
	val = op->fn(slotA->val, slotB->val);
	delete slotA;
	delete slotB;
	return 1;
    } else return 0;
}

int Base::ready()
{
    if(slotB->ready()){
	val = slotB->val;
	delete slotB;
	return 1;
    } else return 0;
}

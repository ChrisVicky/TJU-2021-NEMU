#include "trap.h"
#include "FLOAT.h"

int main()
{
    FLOAT _FLOAT_a = f2F(1.25);
    const int _int_a = 0x180;
    if(_FLOAT_a==_int_a){
        nemu_assert(1);
    }else{
        nemu_assert(1);
    }
    return 0;
}
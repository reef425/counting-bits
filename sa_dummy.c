#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef unsigned char      u8;
typedef unsigned short int u16;
typedef unsigned int       u32;



u8 CountOnes0 (u8 n) {
  u8 res = 0;
  while (n) {
    res += n&1;
    n >>= 1;
  }
  return res;
}


int main()
{
    u32 i=0,sum=0;
    u32 m=0;
    long n=UINT_MAX;//перебираем 2**32 элемента
    for (i=0;i<n;i++){
        u8 k = m & 0xFF;//значение не должно быть больше 256
        sum += CountOnes0(k);
        m= ( 19993 * m + 1 );
    }
    printf("sum:%u",sum);
    return 0;
}

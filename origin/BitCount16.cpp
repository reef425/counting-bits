/**
 * ������� ��������� � ������������ ������ ���������� �������� ��������� ����� 
 * � ������ �������� 16 ���.
 *
 * ���������� ��������� � 
 *  - Visual C++ (��� � ������ 32-�, ��� � 64-� ���)
 *  - Intel C++
 *  - GCC
 *  - CLang
 * 
 *         [ ����� ] �������� ���� ���������� (Zealint)
 * [ ���� �������� ] 08.02.2016
 *
 * � ������ ������� "������ � ����������������" (http://zealcomputing.ru)
 * ������ �������� ��������� ������ ������� �������� � ������ 011.
 * ����������� � ��������� ��������� �������� � ������������.
 */

#include <stdio.h>

// ���������������, ����� ��������� ������������ ���� �������
// ����������� � ������������ � ������� main().
//#define DEBUG

// ���������� ��� ��������
typedef unsigned char      u8;
typedef unsigned short int u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

//
// ����� ���� ������� �������� ���
//

// ������� �������
static u8 CountOnes0 ( u16 n ) {
  u8 res = 0;
  while ( n ) {
    res += ( n & 1 );
    n >>= 1;
  }
  return res;
}

// ���������������� �������� �������� ���������� ����
static u8 CountOnes1 ( u16 n ) {
  u8 res = 0;
  while ( n ) {
    res ++;
    n &= n - 1;
  }
  return res;
}

// ����������� � ������� ������� 8 � 16 ��� (���������� ������� ����)
u8 BitsSetTableFF [ 0xFF + 1 ];
u8 BitsSetTableFFFF [ 0xFFFF + 1 ];

static u8 CountOnes2_FF ( u16 n ) {  
  u8 * p = ( u8 * ) & n;
  return BitsSetTableFF [ p [ 0 ] ] 
       + BitsSetTableFF [ p [ 1 ] ];
}

static u8 CountOnes2_FFFF ( u16 n ) {  
  return BitsSetTableFFFF [ n ];
}

// ���� � ���������� � �������� �� ������� � 32 �����
static u8 CountOnes8Bits ( u8 n ) {  
  if ( n == 0 )  return 0;
  if ( n == 0xFF )  return 8;
  n = ( 0x010101 * n & 0x249249 ) % 7;
  if ( n == 0 )  return 7;
  return n;
}
static u8 CountOnes3 ( u16 n ) {  
  return CountOnes8Bits ( n & 0xFF ) + CountOnes8Bits ( n >> 8 );
}

// ���� � ����� ����������� � �������� �� ������� � 64 �����
static u8 CountOnes3_x64 ( u16 n ) {  
  u8 leastBit = n & 1;
  n >>= 1;
  if ( n == 0 )  return leastBit;
  if ( n == 0x7FFF )  return leastBit + 15;
  return leastBit + ( 0x200040008001llu * n & 0x111111111111111llu ) % 15;
}

// ���� � ����� ����������� � 64 �����
static u8 CountOnes3_x64_m ( u16 n ) {    
  u8 leastBit = n & 1;
  n >>= 1;
  return leastBit + ( ( ( n * 0x200040008001llu & 0x111111111111111llu ) * 0x111111111111111llu >> 56 ) & 0xF );
}

// ������������ ������������ ������ "��������-�-���������"
static u8 CountOnes4 ( u16 n ) {
  n = ( ( n >> 1 ) & 0x5555 ) + ( n & 0x5555 );
  n = ( ( n >> 2 ) & 0x3333 ) + ( n & 0x3333 );
  n = ( ( n >> 4 ) & 0x0F0F ) + ( n & 0x0F0F );
  n = ( ( n >> 8 ) & 0x00FF ) + ( n & 0x00FF );
  return n;
}

// ������������ ������������ ������ "��������-�-���������",
// �� ����������������
static u8 CountOnes4_opt ( u16 n ) {
  n -= ( n >> 1 ) & 0x5555;
  n = ( ( n >> 2 ) & 0x3333 ) + ( n & 0x3333 );
  n = ( ( n >> 4 ) + n ) & 0x0F0F;
  n = ( ( n >> 8 ) + n );
  return n; // ����� ���������� ������� ��������� �� 8 ������� �����.
}

// ��������������� �������
static u8 CountOnes5 ( u16 n ) {
  n -= ( n >> 1 ) & 0x5555;
  n = ( ( n >> 2 ) & 0x3333 ) + ( n & 0x3333 );
  n = ( ( ( ( n >> 4 ) + n ) & 0x0F0F ) * 0x0101 ) >> 8;
  return n; // ����� ���������� ������� ��������� �� 8 ������� �����.
}

int main ( ) {

  u32 n = 0, s = 0;
  
  // ���������� ������� ��� ��������� � �������������
  for ( u32 i = 0; i < 0x100; i ++ )
    BitsSetTableFF [ i ] = CountOnes0 ( i );
  for ( u32 i = 0; i < 0x10000; i ++ )
    BitsSetTableFFFF [ i ] = CountOnes0 ( i );

/**
 * ���� ������ DEBUG �������, ������ ����� ����������� ������������.
 * ����������� ��� 65536 ��������� 16-������� ����� �� ���� ��������.
 * ��� ������� ������ ������� ���������� ���������.
 * ���� �� � �������, ��������� ������� "OK".
 * ���� ���� ������, ��������� �������� �������, � ������� ���������� ������.
 */

#ifdef DEBUG
  while ( n != 0x10000 ) {
    u8 ones = CountOnes0 ( n ); // ��������� �����
    if ( CountOnes1 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes1\n" );
      return 1;
    }
    if ( CountOnes2_FF ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes2_FF\n" );
      return 1;
    }
    if ( CountOnes2_FFFF ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes2_FFFF\n" );
      return 1;
    }
    if ( CountOnes3 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3\n" );
      return 1;
    }
    if ( CountOnes3_x64 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3_x64 %u\n", n );
      return 1;
    }
    if ( CountOnes3_x64_m ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3_x64_m\n" );
      return 1;
    }
    if ( CountOnes4 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes4\n" );
      return 1;
    }
    if ( CountOnes4_opt ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes4_opt\n" );
      return 1;
    }
    if ( CountOnes5 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes5\n" );
      return 1;
    }
    n ++;
  }
  fprintf ( stderr, "OK\n" );
  return 0;
#endif

/**
 * ���� ��� ������������ ������� ������ �������.
 * ��������� ������������������ ������� �� 2^32 ������ � ��������� �������.
 */
  n = 0;
  do {      
    
    u16 k = n & 0xFFFF; // ����� ������� ����� �� ������, �� ����������� 65535.
    
    /**
     * ����� ��� ����� ������������������ �����.
     * ����� ���������������� ���� ������ � ��������� ��������� (����� ������ DEBUG ��������),
     * ������� ����� ������� ����� � ������.
     * ������ ������ "s+=k" ����� ��� ����, ����� �������������� ����� ������ "�������" �����.
     * ���� �������� s+=... �� ���� ������� ����� ��� ����, 
     * ����� ���������� ������ �� ������ ���� �� �������������,
     * ����� 's' ����� ��������� � ����� ������.
     */

    s += k;
    //s += CountOnes0 ( k );
    //s += CountOnes1 ( k );
    //s += CountOnes2_FF ( k );
    //s += CountOnes2_FFFF ( k );
    //s += CountOnes3 ( k );
    //s += CountOnes3_x64 ( k );
    //s += CountOnes3_x64_m ( k );
    //s += CountOnes4 ( k );
    //s += CountOnes4_opt ( k );
    //s += CountOnes5 ( k );

    // ��� ����� ������������ �������������� ������ ��� 2^32 �����,
    // ������ ������ �� ������ ����. �������� �� ����� 0 (������� ������ ����).
    n = ( 19993 * n + 1 );

  } while ( n != 0 ); // � ������� ��������� ����� ������� ��� 2^32 ����� ��������.

  // ������� 's', ����� ���������� �� ������ ����������� ���� �� �������������.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

/**
 * ������� ��������� � ������������ ������ ���������� �������� ��������� �����
 * � ������ �������� 8 ���.
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
static u8 CountOnes0 ( u8 n ) {
  u8 res = 0;
  while ( n ) {
    res += n & 1;
    n >>= 1;
  }
  return res;
}

// ���������������� �������� �������� ���������� ����
static u8 CountOnes1 ( u8 n ) {
  u8 res = 0;
  while ( n ) {
    res ++;
    n &= n - 1;
  }
  return res;
}

// ����������� � ������� ������� (���������� ������� ����)
u8 BitsSetTableFF [ 0xFF + 1 ];
static u8 CountOnes2_FF ( u8 n ) {  
  return BitsSetTableFF [ n ];
}

// ���� � ���������� � �������� �� �������, �� ������ �� 32 ����
static u8 CountOnes3 ( u8 n ) {  
  if ( n == 0 )  return 0;
  if ( n == 0xFF )  return 8;
  n = ( 0x010101 * n & 0x249249 ) % 7;
  if ( n == 0 )  return 7;
  return n;
}

// ���� � ���������� � �������� �� ������� � 64 �����
static u8 CountOnes3_x64 ( u8 n ) {  
  return ( ( u64 ) 0x08040201 * n & 0x111111111 ) % 15;
}

// ������ ������ ������� �� ��������� � �����
static u8 CountOnes3_x64_m ( u8 n ) {  
  if ( n == 0xFF )  return 8;
  return ( ( u64 ( 0x010101 * n & 0x249249 ) * 0x249249 ) >> 21 ) & 0x7;
}

// ������������ ������������
static u8 CountOnes4 ( u8 n ) {
  n = ( ( n >> 1 ) & 0x55 ) + ( n & 0x55 );
  n = ( ( n >> 2 ) & 0x33 ) + ( n & 0x33 );
  n = ( ( n >> 4 ) & 0x0F ) + ( n & 0x0F );
  return n;
}

// ������������ ������������, ����������������
static u8 CountOnes4_opt ( u8 n ) {
  n -= ( n >> 1 ) & 0x55;
  n = ( ( n >> 2 ) & 0x33 ) + ( n & 0x33 );
  n = ( ( n >> 4 ) + n ) & 0x0F;
  return n;
}

int main ( ) {

  u32 n = 0, s = 0;
  
  // ���������� ������� ��� ��������� � �������������
  for ( u32 i = 0; i < 0x100; i ++ )
    BitsSetTableFF [ i ] = CountOnes0 ( i );

/**
 * ���� ������ DEBUG �������, ������ ����� ����������� ������������.
 * ����������� ��� 256 ��������� 8-������� ����� �� ���� ��������.
 * ��� ������� ������ ������� ���������� ���������.
 * ���� �� � �������, ��������� ������� "OK".
 * ���� ���� ������, ��������� �������� �������, � ������� ���������� ������.
 */

#ifdef DEBUG
  while ( n != 0x100 ) {
    u8 ones = CountOnes0 ( n ); // ��������� �����
    if ( CountOnes1 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes1\n" );
      return 1;
    }
    if ( CountOnes2_FF ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes2_FF\n" );
      return 1;
    }
    if ( CountOnes3 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3\n" );
      return 1;
    }
    if ( CountOnes3_x64 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3_x64\n" );
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
    
    u8 k = n & 0xFF; // ����� ������� ����� �� ������, �� ����������� 255.
    
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
    //s += CountOnes3 ( k );
    //s += CountOnes3_x64 ( k );
    //s += CountOnes3_x64_m ( k );
    //s += CountOnes4 ( k );
    //s += CountOnes4_opt ( k );

    // ��� ����� ������������ �������������� ������ ��� 2^32 �����,
    // ������ ������ �� ������ ����. �������� �� ����� 0 (������� ������ ����).
    n = ( 19993 * n + 1 ); 

  } while ( n != 0 ); // � ������� ��������� ����� ������� ��� 2^32 ����� ��������.

  // ������� 's', ����� ���������� �� ������ ����������� ���� �� �������������.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

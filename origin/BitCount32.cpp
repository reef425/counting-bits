/**
 * ������� ��������� � ������������ ������ ���������� �������� ��������� ����� 
 * � ������ �������� 32 ����.
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
static u8 CountOnes0 ( u32 n ) {
  u8 res = 0;
  while ( n ) {
    res += ( n & 1 );
    n >>= 1;
  }
  return res;
}

// ���������������� �������� �������� ���������� ����
static u8 CountOnes1 ( u32 n ) {
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

static u8 CountOnes2_FF ( u32 n ) {  
  u8 * p = ( u8 * ) & n;
  return BitsSetTableFF [ p [ 0 ] ] 
       + BitsSetTableFF [ p [ 1 ] ]
       + BitsSetTableFF [ p [ 2 ] ]
       + BitsSetTableFF [ p [ 3 ] ];
}

static u8 CountOnes2_FFFF ( u32 n ) {  
  u16 * p = ( u16 * ) & n;
  return BitsSetTableFFFF [ p [ 0 ] ]
       + BitsSetTableFFFF [ p [ 1 ] ];
}

// ���� � ����������� � �������� �� ������� � 64 �����
static u8 CountOnes3_x64 ( u32 n ) {    
  if ( n == 0 )  return 0;
  if ( n + 1 == 0 )  return 32;
  u64 res;
  res =  ( n & 0xFFF ) * 0x1001001001001llu & 0x84210842108421llu;
  res += ( ( n & 0xFFF000 ) >> 12 ) * 0x1001001001001llu & 0x84210842108421llu;
  res += ( n >> 24 ) * 0x1001001001001llu & 0x84210842108421llu;
  res %= 0x1F;
  if ( res == 0 )  return 31;
  return res;
}

// ���� � ����� ����������� � 64 �����
static u8 CountOnes3_x64_m ( u32 n ) {    
  if ( n + 1 == 0 )  return 32;
  u64 res;
  res =  ( n & 0xFFF ) * 0x1001001001001llu & 0x84210842108421llu;
  res += ( ( n & 0xFFF000 ) >> 12 ) * 0x1001001001001llu & 0x84210842108421llu;
  res += ( n >> 24 ) * 0x1001001001001llu & 0x84210842108421llu;
  res = ( res * 0x84210842108421llu >> 55 ) & 0x1F;  
  return res;
}

// ������������ ������������ ������ "��������-�-���������"
static u8 CountOnes4 ( u32 n ) {
  n = ( ( n >> 1 ) & 0x55555555 ) + ( n & 0x55555555 );
  n = ( ( n >> 2 ) & 0x33333333 ) + ( n & 0x33333333 );
  n = ( ( n >> 4 ) & 0x0F0F0F0F ) + ( n & 0x0F0F0F0F );
  n = ( ( n >> 8 ) & 0x00FF00FF ) + ( n & 0x00FF00FF );
  n = ( ( n >> 16 ) & 0x0000FFFF ) + ( n & 0x0000FFFF );
  return n;
}

// ������������ ������������ ������ "��������-�-���������",
// �� ����������������
static u8 CountOnes4_opt ( u32 n ) {
  n -= ( n >> 1 ) & 0x55555555;
  n = ( ( n >> 2 ) & 0x33333333 ) + ( n & 0x33333333 );
  n = ( ( n >> 4 ) + n ) & 0x0F0F0F0F;
  n = ( ( n >> 8 ) + n ) & 0x00FF00FF;
  n = ( ( n >> 16 ) + n );
  return n; // ����� ���������� ������� ��������� �� 8 ������� �����.
}

// ��������������� �������
static u8 CountOnes5 ( u32 n ) {
  n -= ( n >> 1 ) & 0x55555555;
  n = ( ( n >> 2 ) & 0x33333333 ) + ( n & 0x33333333 );
  n = ( ( ( ( n >> 4 ) + n ) & 0x0F0F0F0F ) * 0x01010101 ) >> 24;
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
 * ����������� ��� 2^32 ��������� 32-������� ����� �� ���� ��������.
 * ��� ������� ������ ������� ���������� ���������.
 * ���� �� � �������, ��������� ������� "OK".
 * ���� ���� ������, ��������� �������� �������, � ������� ���������� ������.
 */

#ifdef DEBUG
  do {
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
    if ( CountOnes3_x64 ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3_x64\n" );
      return 1;
    }
    if ( CountOnes3_x64_m ( n ) != ones ) {
      fprintf ( stderr, "ERROR in CountOnes3_x64_2\n" );
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
    
  } while ( ++ n != 0 );
  fprintf ( stderr, "OK\n" );
  return 0;
#endif

/**
 * ���� ��� ������������ ������� ������ �������.
 * ��������� ������������������ ������� �� 2^32 ������ � ��������� �������.
 */
  n = 0;
  do {      
        
    /**
     * ����� ��� ����� ������������������ �����.
     * ����� ���������������� ���� ������ � ��������� ��������� (����� ������ DEBUG ��������),
     * ������� ����� ������� ����� � ������.
     * ������ ������ "s+=k" ����� ��� ����, ����� �������������� ����� ������ "�������" �����.
     * ���� �������� s+=... �� ���� ������� ����� ��� ����, 
     * ����� ���������� ������ �� ������ ���� �� �������������,
     * ����� 's' ����� ��������� � ����� ������.
     */

    s += n;
    //s += CountOnes0 ( n );
    //s += CountOnes1 ( n );
    //s += CountOnes2_FF ( n );
    //s += CountOnes2_FFFF ( n );
    //s += CountOnes3_x64 ( n );
    //s += CountOnes3_x64_m ( n );
    //s += CountOnes4 ( n );
    //s += CountOnes4_opt ( n );
    //s += CountOnes5 ( n );

    // ��� ����� ������������ �������������� ������ ��� 2^32 �����,
    // ������ ������ �� ������ ����. �������� �� ����� 0 (������� ������ ����).
    n = ( 19993 * n + 1 ); 

  } while ( n != 0 ); // � ������� ��������� ����� ������� ��� 2^32 ����� ��������.

  // ������� 's', ����� ���������� �� ������ ����������� ���� �� �������������.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

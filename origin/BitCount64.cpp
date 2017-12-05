/**
 * ������� ��������� � ������������ ������ ���������� �������� ��������� �����
 * � ������ �������� 64 ����.
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
static u8 CountOnes0 ( u64 n ) {
  u8 res = 0;
  while ( n ) {
    res += ( n & 1 );
    n >>= 1;
  }
  return res;
}

// ���������������� �������� �������� ���������� ����
static u8 CountOnes1 ( u64 n ) {
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

static u8 CountOnes2_FF ( u64 n ) {  
  u8 * p = ( u8 * ) & n;
  return BitsSetTableFF [ p [ 0 ] ] 
       + BitsSetTableFF [ p [ 1 ] ]
       + BitsSetTableFF [ p [ 2 ] ]
       + BitsSetTableFF [ p [ 3 ] ]
       + BitsSetTableFF [ p [ 4 ] ]
       + BitsSetTableFF [ p [ 5 ] ]
       + BitsSetTableFF [ p [ 6 ] ]
       + BitsSetTableFF [ p [ 7 ] ];
}

static u8 CountOnes2_FFFF ( u64 n ) {  
  u16 * p = ( u16 * ) & n;
  return BitsSetTableFFFF [ p [ 0 ] ]
       + BitsSetTableFFFF [ p [ 1 ] ]
       + BitsSetTableFFFF [ p [ 2 ] ]
       + BitsSetTableFFFF [ p [ 3 ] ];
}

// ������������ ������������ ������ "��������-�-���������"
static u8 CountOnes4 ( u64 n ) {
  n = ( ( n >> 1 ) & 0x5555555555555555llu ) + ( n & 0x5555555555555555llu );
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( n >> 4 ) & 0x0F0F0F0F0F0F0F0Fllu ) + ( n & 0x0F0F0F0F0F0F0F0Fllu );
  n = ( ( n >> 8 ) & 0x00FF00FF00FF00FFllu ) + ( n & 0x00FF00FF00FF00FFllu );
  n = ( ( n >> 16 ) & 0x0000FFFF0000FFFFllu ) + ( n & 0x0000FFFF0000FFFFllu );
  n = ( ( n >> 32 ) & 0x00000000FFFFFFFFllu ) + ( n & 0x00000000FFFFFFFFllu );
  return n;
}

// ������������ ������������ ������ "��������-�-���������",
// �� ����������������
static u8 CountOnes4_opt ( u64 n ) {
  n -= ( n >> 1 ) & 0x5555555555555555llu;
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( n >> 4 ) + n ) & 0x0F0F0F0F0F0F0F0Fllu;
  n = ( ( n >> 8 ) + n ) & 0x00FF00FF00FF00FFllu;
  n = ( ( n >> 16 ) + n ) & 0x0000FFFF0000FFFFllu;
  n = ( ( n >> 32 ) + n );
  return n; // ����� ���������� ������� ��������� �� 8 ������� �����.
}

// ��������������� �������
static u8 CountOnes5 ( u64 n ) {
  n -= ( n >> 1 ) & 0x5555555555555555llu;
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( ( ( n >> 4 ) + n ) & 0x0F0F0F0F0F0F0F0Fllu ) * 0x0101010101010101 ) >> 56;
  return n; // ����� ���������� ������� ��������� �� 8 ������� �����.
}

int main ( ) {

  u32 s = 0;
  u64 n;
  
  // ���������� ������� ��� ��������� � �������������
  for ( u32 i = 0; i < 0x100; i ++ )
    BitsSetTableFF [ i ] = CountOnes0 ( i );
  for ( u32 i = 0; i < 0x10000; i ++ )
    BitsSetTableFFFF [ i ] = CountOnes0 ( i );

/**
 * ���� ������ DEBUG �������, ������ ����� ����������� ������������.
 * ����������� ��� 2^32 ��������� 32-������� ����� �� ���� ��������.
 * 64-������� ����� ���������� ������� ������������ ������� �������� � �������.
 * (����������, ��� �� ��� ������ �������� ������������ ������ �������).
 * ��� ������� ������ ������� ���������� ���������.
 * ���� �� � �������, ��������� ������� "OK".
 * ���� ���� ������, ��������� �������� �������, � ������� ���������� ������.
 */

#ifdef DEBUG
  do {
    n = ( u64 ) s << 32 | s; // ������ �������� ������� 32 ���� � �������.
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
    
  } while ( ++ s != 0 );
  fprintf ( stderr, "OK\n" );
  return 0;
#endif

/**
 * ���� ��� ������������ ������� ������ �������.
 * ��������� ������������������ ������� �� 2^32 ������ � ��������� �������.
 */
  u32 i = 0;
  do {      
     
    u64 k = ( u64 ) i << 32 | i; 
    
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
    s += CountOnes0 ( k );
    s += CountOnes1 ( k );
    s += CountOnes2_FF ( k );
    s += CountOnes2_FFFF ( k );    
    s += CountOnes4 ( k );
    s += CountOnes4_opt ( k );
    s += CountOnes5 ( k );

    // ��� ����� ������������ �������������� ������ ��� 2^32 �����,
    // ������ ������ �� ������ ����. �������� �� ����� 0 (������� ������ ����).
    i = ( 19993 * i + 1 ); 

  } while ( i != 0 ); // � ������� ��������� ����� ������� ��� 2^32 ����� ��������.

  // ������� 's', ����� ���������� �� ������ ����������� ���� �� �������������.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

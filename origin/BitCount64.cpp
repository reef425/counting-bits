/**
 * Учебная программа с реализациями разных алгоритмов подсчёта единичных битов
 * в числах размером 64 бита.
 *
 * Компиляция проверена в 
 *  - Visual C++ (как в режиме 32-х, так и 64-х бит)
 *  - Intel C++
 *  - GCC
 *  - CLang
 * 
 *         [ Автор ] Караваев Артём Михайлович (Zealint)
 * [ Дата создания ] 08.02.2016
 *
 * В рамках проекта "Беседы о программировании" (http://zealcomputing.ru)
 * Полное описание принципов работы функций смотрите в беседе 011.
 * Подробности о структуре программы смотрите в комментариях.
 */

#include <stdio.h>

// Откомментируйте, чтобы запустить тестирование всех функций
// Подробности о тестировании в функции main().
//#define DEBUG

// Псевдонимы для удобства
typedef unsigned char      u8;
typedef unsigned short int u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

//
// Далее идут функции подсчёта бит
//

// Наивный подсчёт
static u8 CountOnes0 ( u64 n ) {
  u8 res = 0;
  while ( n ) {
    res += ( n & 1 );
    n >>= 1;
  }
  return res;
}

// Последовательное удаление младшего единичного бита
static u8 CountOnes1 ( u64 n ) {
  u8 res = 0;
  while ( n ) {
    res ++;
    n &= n - 1;
  }
  return res;
}

// Предподсчёт с помощью таблицы 8 и 16 бит (заполнение таблицы ниже)
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

// Параллельное суммирование блоков "разделяй-и-влавствуй"
static u8 CountOnes4 ( u64 n ) {
  n = ( ( n >> 1 ) & 0x5555555555555555llu ) + ( n & 0x5555555555555555llu );
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( n >> 4 ) & 0x0F0F0F0F0F0F0F0Fllu ) + ( n & 0x0F0F0F0F0F0F0F0Fllu );
  n = ( ( n >> 8 ) & 0x00FF00FF00FF00FFllu ) + ( n & 0x00FF00FF00FF00FFllu );
  n = ( ( n >> 16 ) & 0x0000FFFF0000FFFFllu ) + ( n & 0x0000FFFF0000FFFFllu );
  n = ( ( n >> 32 ) & 0x00000000FFFFFFFFllu ) + ( n & 0x00000000FFFFFFFFllu );
  return n;
}

// Параллельное суммирование блоков "разделяй-и-влавствуй",
// но оптимизированное
static u8 CountOnes4_opt ( u64 n ) {
  n -= ( n >> 1 ) & 0x5555555555555555llu;
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( n >> 4 ) + n ) & 0x0F0F0F0F0F0F0F0Fllu;
  n = ( ( n >> 8 ) + n ) & 0x00FF00FF00FF00FFllu;
  n = ( ( n >> 16 ) + n ) & 0x0000FFFF0000FFFFllu;
  n = ( ( n >> 32 ) + n );
  return n; // Здесь происходит неявное обрезание по 8 младшим битам.
}

// Комбинированный вариант
static u8 CountOnes5 ( u64 n ) {
  n -= ( n >> 1 ) & 0x5555555555555555llu;
  n = ( ( n >> 2 ) & 0x3333333333333333llu ) + ( n & 0x3333333333333333llu );
  n = ( ( ( ( n >> 4 ) + n ) & 0x0F0F0F0F0F0F0F0Fllu ) * 0x0101010101010101 ) >> 56;
  return n; // Здесь происходит неявное обрезание по 8 младшим битам.
}

int main ( ) {

  u32 s = 0;
  u64 n;
  
  // Заполнение таблицы для алгоритма с предподсчётом
  for ( u32 i = 0; i < 0x100; i ++ )
    BitsSetTableFF [ i ] = CountOnes0 ( i );
  for ( u32 i = 0; i < 0x10000; i ++ )
    BitsSetTableFFFF [ i ] = CountOnes0 ( i );

/**
 * Если макрос DEBUG включён, значит будет выполняться тестирование.
 * Проверяются все 2^32 возможных 32-битовых чисел на всех функциях.
 * 64-битовое число получается простым копированием младшей половины в старшую.
 * (Разумеется, это не даёт полной гарантии правильности работы функций).
 * Все функции должны вернуть одинаковый результат.
 * Если всё в порядке, программа выведет "OK".
 * Если есть ошибки, выведется название функции, в которой обнаружена ошибка.
 */

#ifdef DEBUG
  do {
    n = ( u64 ) s << 32 | s; // Просто копируем младшие 32 бита в старшие.
    u8 ones = CountOnes0 ( n ); // Эталонный ответ
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
 * Цикл для тестирования времени работы функций.
 * Прогоняет откомментированную функцию на 2^32 тестах в хаотичном порядке.
 */
  u32 i = 0;
  do {      
     
    u64 k = ( u64 ) i << 32 | i; 
    
    /**
     * Далее идёт серия закомментированных строк.
     * Нужно откомментировать одну строку и запустить программу (когда макрос DEBUG выключен),
     * замеряя таким образом время её работы.
     * Первая строка "s+=k" нужна для того, чтобы протестировать время работы "пустого" цикла.
     * Сама операция s+=... во всех строках нужна для того, 
     * чтобы компилятор вообще не удалил цикл за ненадобностью,
     * число 's' затем выводится в поток ошибок.
     */

    s += k;
    s += CountOnes0 ( k );
    s += CountOnes1 ( k );
    s += CountOnes2_FF ( k );
    s += CountOnes2_FFFF ( k );    
    s += CountOnes4 ( k );
    s += CountOnes4_opt ( k );
    s += CountOnes5 ( k );

    // При таком перечислении гарантированно пройдём все 2^32 чисел,
    // причём каждое по одному разу. Закончим на числе 0 (условие выхода ниже).
    i = ( 19993 * i + 1 ); 

  } while ( i != 0 ); // К моменту нарушения этого условию все 2^32 чисел пройдены.

  // Выводим 's', чтобы компилятор не удалил вышестоящий цикл за ненадобностью.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

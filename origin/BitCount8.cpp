/**
 * Учебная программа с реализациями разных алгоритмов подсчёта единичных битов
 * в числах размером 8 бит.
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
static u8 CountOnes0 ( u8 n ) {
  u8 res = 0;
  while ( n ) {
    res += n & 1;
    n >>= 1;
  }
  return res;
}

// Последовательное удаление младшего единичного бита
static u8 CountOnes1 ( u8 n ) {
  u8 res = 0;
  while ( n ) {
    res ++;
    n &= n - 1;
  }
  return res;
}

// Предподсчёт с помощью таблицы (заполнение таблицы ниже)
u8 BitsSetTableFF [ 0xFF + 1 ];
static u8 CountOnes2_FF ( u8 n ) {  
  return BitsSetTableFF [ n ];
}

// Трюк с умножением и остатком от деления, не выходя за 32 бита
static u8 CountOnes3 ( u8 n ) {  
  if ( n == 0 )  return 0;
  if ( n == 0xFF )  return 8;
  n = ( 0x010101 * n & 0x249249 ) % 7;
  if ( n == 0 )  return 7;
  return n;
}

// Трюк с умножением и остатком от деления в 64 битах
static u8 CountOnes3_x64 ( u8 n ) {  
  return ( ( u64 ) 0x08040201 * n & 0x111111111 ) % 15;
}

// Замена взятия остатка на умножение и сдвиг
static u8 CountOnes3_x64_m ( u8 n ) {  
  if ( n == 0xFF )  return 8;
  return ( ( u64 ( 0x010101 * n & 0x249249 ) * 0x249249 ) >> 21 ) & 0x7;
}

// Параллельное суммирование
static u8 CountOnes4 ( u8 n ) {
  n = ( ( n >> 1 ) & 0x55 ) + ( n & 0x55 );
  n = ( ( n >> 2 ) & 0x33 ) + ( n & 0x33 );
  n = ( ( n >> 4 ) & 0x0F ) + ( n & 0x0F );
  return n;
}

// Параллельное суммирование, оптимизированное
static u8 CountOnes4_opt ( u8 n ) {
  n -= ( n >> 1 ) & 0x55;
  n = ( ( n >> 2 ) & 0x33 ) + ( n & 0x33 );
  n = ( ( n >> 4 ) + n ) & 0x0F;
  return n;
}

int main ( ) {

  u32 n = 0, s = 0;
  
  // Заполнение таблицы для алгоритма с предподсчётом
  for ( u32 i = 0; i < 0x100; i ++ )
    BitsSetTableFF [ i ] = CountOnes0 ( i );

/**
 * Если макрос DEBUG включён, значит будет выполняться тестирование.
 * Проверяются все 256 возможных 8-битовых чисел на всех функциях.
 * Все функции должны вернуть одинаковый результат.
 * Если всё в порядке, программа выведет "OK".
 * Если есть ошибки, выведется название функции, в которой обнаружена ошибка.
 */

#ifdef DEBUG
  while ( n != 0x100 ) {
    u8 ones = CountOnes0 ( n ); // Эталонный ответ
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
 * Цикл для тестирования времени работы функций.
 * Прогоняет откомментированную функцию на 2^32 тестах в хаотичном порядке.
 */
  n = 0;
  do {      
    
    u8 k = n & 0xFF; // Вызов функции будет на числах, не превышающих 255.
    
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
    //s += CountOnes0 ( k );
    //s += CountOnes1 ( k );
    //s += CountOnes2_FF ( k );
    //s += CountOnes3 ( k );
    //s += CountOnes3_x64 ( k );
    //s += CountOnes3_x64_m ( k );
    //s += CountOnes4 ( k );
    //s += CountOnes4_opt ( k );

    // При таком перечислении гарантированно пройдём все 2^32 чисел,
    // причём каждое по одному разу. Закончим на числе 0 (условие выхода ниже).
    n = ( 19993 * n + 1 ); 

  } while ( n != 0 ); // К моменту нарушения этого условию все 2^32 чисел пройдены.

  // Выводим 's', чтобы компилятор не удалил вышестоящий цикл за ненадобностью.
  fprintf ( stderr, "%u\n", s );

  return 0;
}

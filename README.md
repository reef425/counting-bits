# counting-bits
Counting Bits код программы демонстрирующий подсчет битов.  Источник алгоритма статья на [habrahabr](https://habrahabr.ru/post/276957/)

*Простой алгоритм для подчета битов*

```
int bitsCount(int n) {
  int res = 0;
  while (n) {
    res += n&1;
    n >>= 1;
  }
  return res;
}
```

#include "lexp.h"

/* atom, primitive, cons, closure and nil tags for NaN boxing */
ttyp ATOM = 0x7ff8, PRIM = 0x7ff9, CONS = 0x7ffa, CLOS = 0x7ffb, NIL = 0x7ffc, MACR = 0x7ffd;

/* NaN-boxing specific functions:
   box(t,i): returns a new NaN-boxed double with tag t and ordinal i
   ord(x):   returns the ordinal of the NaN-boxed double x
   num(n):   convert or check number n (does nothing, e.g. could check for NaN)
   equ(x,y): returns nonzero if x equals y */
lexp box(ttyp t, hsptyp i) {
  lexp x;
  *(unsigned long long*)&x = (unsigned long long)t << 48 | i;
  return x;
}

hsptyp ord(lexp x) {
  return *(unsigned long long*)&x;      /* the return value is narrowed to 32 bit unsigned integer to remove the tag */
}

bool equ(lexp x, lexp y) {
  return *(unsigned long long*)&x == *(unsigned long long*)&y;
}


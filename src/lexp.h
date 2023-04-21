
#include <stdbool.h>

/* Type of the lisp expresion type */
#define ttyp unsigned

/* Lisp object */
#define lexp double

#define NUM_FMT	"lg"

/*  Convert (floating-point) number to integer */
#define numtoint(n) (n<1e16 && n>-1e16 ? (long long)n : n)

/* typof(x) returns the tag bits of a NaN-boxed Lisp expression x */
#define typof(x) (*(unsigned long long*)&x >> 48)

/* heap or stack pointer type */
#define hsptyp unsigned

lexp box(ttyp t, hsptyp i);
hsptyp ord(lexp x);
bool equ(lexp x, lexp y);
extern ttyp ATOM, PRIM, CONS, CLOS, NIL;

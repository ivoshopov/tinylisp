
/* Type of the lisp expresion type */
#define ttyp unsigned

/* Lisp object */
#define lexp double

/* typof(x) returns the tag bits of a NaN-boxed Lisp expression x */
#define typof(x) (*(unsigned long long*)&x >> 48)

/* heap or stack pointer type */
#define hsptyp unsigned

lexp box(ttyp t, hsptyp i);
hsptyp ord(lexp x);
extern ttyp ATOM, PRIM, CONS, CLOS, NIL;

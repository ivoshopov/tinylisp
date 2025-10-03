
#include <stdbool.h>

/* Type of the lisp expression type */
#define ttyp unsigned int

/* Lisp object */
typedef double lexp; /* typedef is preferred over #define because gdb pretty printer won't works */

#define NUM_FMT	"lg"

/* This gives you plenty of room for:
 * Sign: -
 * Decimal point: .
 * Scientific notation: e+XXX
 * Significant digits (typically 15–17)
 * Null terminator \0 */
#define MAX_DOUBLE_STR 32

/*  Convert (floating-point) number to integer */
#define numtoint(n) (n<1e16 && n>-1e16 ? (long long)n : n)

/* typof(x) returns the tag bits of a NaN-boxed Lisp expression x */
#define typof(x) (*(unsigned long long*)&x >> 48)

/* heap or stack pointer type */
#define hsptyp unsigned
#define UINT_MAX_STR "4294967295"
#define MAX_HSPTYP_STR 11

lexp box(ttyp t, hsptyp i);
hsptyp ord(lexp x);
bool equ(lexp x, lexp y);
extern ttyp ATOM, PRIM, CONS, CLOS, NIL, MACR, BOX, ERR;

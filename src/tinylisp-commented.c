/* tinylisp-commented.c with NaN boxing by Robert A. van Engelen 2022 */
/* tinylisp.c but adorned with comments in an (overly) verbose C style */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexp.h"

/* we only need two types to implement a Lisp interpreter:
        iobj unsigned integer (either 16 bit, 32 bit or 64 bit unsigned)
        lexp Lisp expression (double with NaN boxing)
   iobj variables and function parameters are named as follows:
        i    any unsigned integer, e.g. a NaN-boxed ordinal value
        t    a NaN-boxing tag
   lexp  variables and function parameters are named as follows:
        x,y  any Lisp expression
        n    number
        t    list
        f    function or Lisp primitive
        p    pair, a cons of two Lisp expressions
        e,d  environment, a list of pairs, e.g. created with (define v x)
        v    the name of a variable (an atom) or a list of variables */
#define iobj unsigned

/* address of the atom heap is at the bottom of the cell stack */
#define A (char*)cell

/* number of cells for the shared stack and atom heap, increase N as desired */
#define N 1024

/* hp: heap pointer, A+hp with hp=0 points to the first atom string in cell[]
   sp: stack pointer, the stack starts at the top of cell[] with sp=N
   safety invariant: hp <= sp<<3 */
hsptyp hp = 0;
hsptyp sp = N;

/* cell[N] array of Lisp expressions, shared by the stack and atom heap */
lexp cell[N];

/* Lisp constant expressions () (nil), #t, ERR, and the global environment env */
lexp nil, tru, err, env;

lexp num(lexp n) {
  return n;
}

bool equ(lexp x, lexp y) {
  return *(unsigned long long*)&x == *(unsigned long long*)&y;
}

/* interning of atom names (Lisp symbols), returns a unique NaN-boxed ATOM */
lexp atom(const char *s) {
  hsptyp i = 0;
  while (i < hp && strcmp(A+i, s))              /* search for a matching atom name on the heap */
    i += strlen(A+i)+1;
  if (i == hp) {                                /* if not found */
    hp += strlen(strcpy(A+i, s))+1;             /*   allocate and add a new atom name to the heap */
    if (hp > sp<<3)                             /* abort when out of memory */
      abort();
  }
  return box(ATOM, i);
}

/* construct pair (x . y) returns a NaN-boxed CONS */
lexp cons(lexp x, lexp y) {
  cell[--sp] = x;                               /* push the car value x */
  cell[--sp] = y;                               /* push the cdr value y */
  if (hp > sp<<3)                               /* abort when out of memory */
    abort();
  return box(CONS, sp);
}

/* return the car of a pair or ERR if not a pair */
lexp car(lexp p) {
  return typof(p) == CONS || typof(p) == CLOS ? cell[ord(p)+1] : err;
}

/* return the cdr of a pair or ERR if not a pair */
lexp cdr(lexp p) {
  return typof(p) == CONS || typof(p) == CLOS ? cell[ord(p)] : err;
}

/* construct a pair to add to environment e, returns the list ((v . x) . e) */
lexp pair(lexp v, lexp x, lexp e) {
  return cons(cons(v, x), e);
}

/* construct a closure, returns a NaN-boxed CLOS */
lexp closure(lexp v, lexp x, lexp e) {
  return box(CLOS, ord(pair(v, x, equ(e, env) ? nil : e)));
}

/* look up a symbol in an environment, return its value or ERR if not found */
lexp assoc(lexp v, lexp e) {
  while (typof(e) == CONS && !equ(v, car(car(e))))
    e = cdr(e);
  return typof(e) == CONS ? cdr(car(e)) : err;
}

/* not(x) is nonzero if x is the Lisp () empty list */
bool not(lexp x) {
  return typof(x) == NIL;
}

/* let(x) is nonzero if x is a Lisp let/let* pair */
bool let(lexp x) {
  return typof(x) != NIL && !not(cdr(x));
}

/* return a new list of evaluated Lisp expressions t in environment e */
lexp eval(lexp, lexp);
lexp evlis(lexp t, lexp e) {
  return typof(t) == CONS ? cons(eval(car(t), e), evlis(cdr(t), e)) : typof(t) == ATOM ? assoc(t,e) : nil;
}

/* Lisp primitives:
   (eval x)            return evaluated x (such as when x was quoted)
   (quote x)           special form, returns x unevaluated "as is"
   (cons x y)          construct pair (x . y)
   (car p)             car of pair p
   (cdr p)             cdr of pair p
   (add n1 n2 ... nk)  sum of n1 to nk
   (sub n1 n2 ... nk)  n1 minus sum of n2 to nk
   (mul n1 n2 ... nk)  product of n1 to nk
   (div n1 n2 ... nk)  n1 divided by the product of n2 to nk
   (int n)             integer part of n
   (< n1 n2)           #t if n1<n2, otherwise ()
   (eq? x y)           #t if x equals y, otherwise ()
   (not x)             #t if x is (), otherwise ()
   (or x1 x2 ... xk)   first x that is not (), otherwise ()
   (and x1 x2 ... xk)  last x if all x are not (), otherwise ()
   (cond (x1 y1)
         (x2 y2)
         ...
         (xk yk))      the first yi for which xi evaluates to non-()
   (if x y z)          if x is non-() then y else z
   (let* (v1 x1)
         (v2 x2)
         ...
         y)            sequentially binds each variable v1 to xi to evaluate y
   (lambda v x)        construct a closure
   (define v x)        define a named value globally */
lexp f_eval(lexp t, lexp e) {
  return eval(car(evlis(t, e)), e);
}

lexp f_quote(lexp t, lexp _) {
  return car(t);
}

lexp f_cons(lexp t, lexp e) {
  t = evlis(t, e);
  return cons(car(t), car(cdr(t)));
}

lexp f_car(lexp t, lexp e) {
  return car(car(evlis(t, e)));
}

lexp f_cdr(lexp t, lexp e) {
  return cdr(car(evlis(t, e)));
}

lexp f_add(lexp t, lexp e) {
  lexp n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n += car(t);
  return num(n);
}

lexp f_sub(lexp t, lexp e) {
  lexp n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n -= car(t);
  return num(n);
}

lexp f_mul(lexp t, lexp e) {
  lexp n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n *= car(t);
  return num(n);
}

lexp f_div(lexp t, lexp e) {
  lexp n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n /= car(t);
  return num(n);
}

lexp f_int(lexp t, lexp e) {
  lexp n = car(evlis(t, e));
  return n<1e16 && n>-1e16 ? (long long)n : n;
}

lexp f_lt(lexp t, lexp e) {
  return t = evlis(t, e), car(t) - car(cdr(t)) < 0 ? tru : nil;
}

lexp f_eq(lexp t, lexp e) {
  return t = evlis(t, e), equ(car(t), car(cdr(t))) ? tru : nil;
}

lexp f_not(lexp t, lexp e) {
  return not(car(evlis(t, e))) ? tru : nil;
}

lexp f_or(lexp t,lexp e) {
  lexp x = nil;
  while (typof(t) != NIL && not(x = eval(car(t),e)))
    t = cdr(t);
  return x;
}

lexp f_and(lexp t,lexp e) {
  lexp x = nil;
  while (typof(t) != NIL && !not(x = eval(car(t),e)))
    t = cdr(t);
  return x;
}

lexp f_cond(lexp t, lexp e) {
  while (typof(t) != NIL && not(eval(car(car(t)), e)))
    t = cdr(t);
  return eval(car(cdr(car(t))), e);
}

lexp f_if(lexp t, lexp e) {
  return eval(car(cdr(not(eval(car(t), e)) ? cdr(t) : t)), e);
}

lexp f_leta(lexp t, lexp e) {
  for (; let(t); t = cdr(t))
    e = pair(car(car(t)), eval(car(cdr(car(t))), e), e);
  return eval(car(t), e);
}

lexp f_lambda(lexp t, lexp e) {
  return closure(car(t), car(cdr(t)), e);
}

lexp f_define(lexp t, lexp e) {
  env = pair(car(t), eval(car(cdr(t)), e), env);
  return car(t);
}

/* table of Lisp primitives, each has a name s and function pointer f */
struct {
  const char *s;
  lexp (*f)(lexp, lexp);
} prim[] = {
  {"eval",   f_eval},
  {"quote",  f_quote},
  {"cons",   f_cons},
  {"car",    f_car},
  {"cdr",    f_cdr},
  {"+",      f_add},
  {"-",      f_sub},
  {"*",      f_mul},
  {"/",      f_div},
  {"int",    f_int},
  {"<",      f_lt},
  {"eq?",    f_eq},
  {"or",     f_or},
  {"and",    f_and},
  {"not",    f_not},
  {"cond",   f_cond},
  {"if",     f_if},
  {"let*",   f_leta},
  {"lambda", f_lambda},
  {"define", f_define},
  {0}};

/* create environment by extending e with variables v bound to values t */
lexp bind(lexp v, lexp t, lexp e) {
  return typof(v) == NIL ? e :
         typof(v) == CONS ? bind(cdr(v), cdr(t), pair(car(v), car(t), e)) :
         pair(v, t, e);
}

/* apply closure f to arguments t in environemt e */
lexp reduce(lexp f, lexp t, lexp e) {
  return eval(cdr(car(f)), bind(car(car(f)), evlis(t, e), not(cdr(f)) ? env : cdr(f)));
}

/* apply closure or primitive f to arguments t in environment e, or return ERR */
lexp apply(lexp f, lexp t, lexp e) {
  return typof(f) == PRIM ? prim[ord(f)].f(t, e) :
         typof(f) == CLOS ? reduce(f, t, e) :
         err;
}

/* evaluate x and return its value in environment e */
lexp eval(lexp x, lexp e) {
  return typof(x) == ATOM ? assoc(x, e) :
         typof(x) == CONS ? apply(eval(car(x), e), cdr(x), e) :
         x;
}

/* tokenization buffer and the next character that we are looking at */
char buf[40], see = ' ';

/* advance to the next character */
void look() {
  int c = getchar();
  see = c;
  if (c == EOF)
    exit(0);
}

/* return nonzero if we are looking at character c, ' ' means any white space */
bool seeing(char c) {
  return c == ' ' ? see > 0 && see <= c : see == c;
}

/* return the look ahead character from standard input, advance to the next */
char get() {
  char c = see;
  look();
  return c;
}

/* tokenize into buf[], return first character of buf[] */
char scan() {
  iobj i = 0;
  while (seeing(' '))
    look();
  if (seeing('(') || seeing(')') || seeing('\''))
    buf[i++] = get();
  else
    do
      buf[i++] = get();
    while (i < 39 && !seeing('(') && !seeing(')') && !seeing(' '));
  buf[i] = 0;
  return *buf;
}

/* return the Lisp expression read from standard input */
lexp parse();
lexp read() {
  scan();
  return parse();
}

/* return a parsed Lisp list */
lexp list() {
  lexp x;
  if (scan() == ')')
    return nil;
  if (!strcmp(buf, ".")) {
    x = read();
    scan();
    return x;
  }
  x = parse();
  return cons(x, list());
}

/* return a parsed Lisp expression x quoted as (quote x) */
lexp quote() {
  return cons(atom("quote"), cons(read(), nil));
}

/* return a parsed atomic Lisp expression (a number or an atom) */
lexp atomic() {
  lexp n; iobj i;
  return (sscanf(buf, "%lg%n", &n, &i) > 0 && !buf[i]) ? n :
         atom(buf);
}

/* return a parsed Lisp expression */
lexp parse() {
  return *buf == '(' ? list() :
         *buf == '\'' ? quote() :
         atomic();
}

/* display a Lisp list t */
void print(lexp);
void printlist(lexp t) {
  for (putchar('('); ; putchar(' ')) {
    print(car(t));
    t = cdr(t);
    if (typof(t) == NIL)
      break;
    if (typof(t) != CONS) {
      printf(" . ");
      print(t);
      break;
    }
  }
  putchar(')');
}

/* display a Lisp expression x */
void print(lexp x) {
  if (typof(x) == NIL)
    printf("()");
  else if (typof(x) == ATOM)
    printf("%s", A+ord(x));
  else if (typof(x) == PRIM)
    printf("<%s>", prim[ord(x)].s);
  else if (typof(x) == CONS)
    printlist(x);
  else if (typof(x) == CLOS)
    printf("{%u}", ord(x));
  else
    printf("%.10lg", x);
}

/* garbage collection removes temporary cells, keeps global environment */
void gc() {
  sp = ord(env);
}

/* Lisp initialization and REPL */
int main() {
  iobj i;
  printf("tinylisp");
  nil = box(NIL, 0);
  err = atom("ERR");
  tru = atom("#t");
  env = pair(tru, tru, nil);
  for (i = 0; prim[i].s; ++i)
    env = pair(atom(prim[i].s), box(PRIM, i), env);
  while (1) {
    printf("\n%u>", sp-hp/8);
    print(eval(read(), env));
    gc();
  }
}

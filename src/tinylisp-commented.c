/* tinylisp-commented.c with NaN boxing by Robert A. van Engelen 2022 */
/* tinylisp.c but adorned with comments in an (overly) verbose C style */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <lexp.h>
#include <io.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <unused.h>
#include <primitive.h>
#include <assert.h>


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

/* Lisp constant expressions () (nil), #t, and the global environment env */
lexp nil, tru, env;

lexp num(lexp n) {
  return n;
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

char* unbox_atom(lexp atom) {
  assert( typof(atom) == ATOM );
  return (char*)cell + ord(atom);
}

lexp err(lexp err_msg) {
  assert( typof(err_msg) == CONS );
  /* We are going to wrap err_msg in the ERR type */
  return box(ERR, ord(err_msg));
}

lexp unbox_err(lexp err) {
  assert( typof(err) == ERR );
  return box(CONS, ord(err));
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
  return typof(p) == CONS || typof(p) == CLOS || typof(p) == MACR ? cell[ord(p)+1] : mk_error("Couldn't take the car of: ", p);
}

/* return the cdr of a pair or ERR if not a pair */
lexp cdr(lexp p) {
  return typof(p) == CONS || typof(p) == CLOS || typof(p) == MACR ? cell[ord(p)] : mk_error("Couldn't take the cdr of: ", p);
}

/* construct a pair to add to environment e, returns the list ((v . x) . e) */
lexp pair(lexp v, lexp x, lexp e) {
  return cons(cons(v, x), e);
}

/* construct a closure, returns a NaN-boxed CLOS */
lexp closure(lexp v, lexp x, lexp e) {
  return box(CLOS, ord(pair(v, x, equ(e, env) ? nil : e)));
}

/* construct a macro, returns a NaN-boxed MACR */
lexp macro(lexp v, lexp x) {
  return box(MACR, ord(cons(v, x)));
}

/* look up a symbol in an environment, return its value or ERR if not found */
lexp assoc(lexp v, lexp e) {
  while (typof(e) == CONS && !equ(v, car(car(e))))
    e = cdr(e);
  return typof(e) == CONS ? cdr(car(e)) : mk_error("Couldn't find the symbol: ", v); // TODO: add  " in envirnment: ", e);
}

/* look up a symbol in a associative list and return a pair or nil if not found */
lexp assoc_pair(lexp v, lexp e) {
  while (typof(e) == CONS && !equ(v, car(car(e))))
    e = cdr(e);
  return typof(e) == CONS ? car(e) : nil;
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
  UNUSED(_);
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
  /* TODO: verify car(t) is a number */
  while (!not(t = cdr(t)))
    n += car(t);
    /* TODO: verify car(t) is a number */
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
  return numtoint(n);
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
  return nil;
}

lexp f_macro(lexp t, lexp e) {
  UNUSED(e);
  return macro(car(t), car(cdr(t)));
}

lexp f_assoc(lexp t, lexp e) {
  return t = evlis(t,e),assoc_pair(car(t),car(cdr(t)));
}

/* table of Lisp primitives, each has a name s and function pointer f */
PRIMITIVE_SECTION struct primitive core_prim[] = {
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
  {"macro",  f_macro},
  {"assoc",  f_assoc},
  };

/* create environment by extending e with variables v bound to values t */
lexp bind(lexp v, lexp t, lexp e) {
  return typof(v) == NIL ? e :
         typof(v) == CONS ? bind(cdr(v), cdr(t), pair(car(v), car(t), e)) :
         pair(v, t, e);
}

/* apply closure f to arguments t in environemt e */
lexp reduce(lexp f, lexp t, lexp e) {
  /* for more info read SICP - Envirnment model
     https://www.youtube.com/watch?v=SDsxFreEYsc&list=PL7BcsI5ueSNFPCEisbaoQ0kXIDX9rR5FF */
  /* car(car(f)) is lambda local parameters
     cdr(car(f)) is lambda body */
  return eval(cdr(car(f)), bind(car(car(f)), evlis(t, e), not(cdr(f)) ? env : cdr(f)));
}

lexp expand(lexp f, lexp t, lexp e) {
  return eval(eval(cdr(f), bind(car(f), t, env)), e);
}

/* apply closure or primitive f to arguments t in environment e, or return ERR */
lexp apply(lexp f, lexp t, lexp e) {
  struct primitive *prim = &__start_primitives;
  return typof(f) == PRIM ? prim[ord(f)].f(t, e) :
         typof(f) == CLOS ? reduce(f, t, e) :
         typof(f) == MACR ? expand(f, t, e) :
         mk_error("Couldn't apply/call:", f);
}

/* evaluate x and return its value in environment e */
lexp _eval(lexp x, lexp e) {
  /* First we check does the lisp expression is an ATOM. If we call the eval
     with "#t" or "car" or some other symbol we search for an association in
     the environment. Later we test does we have an expression made by CONS (a
     list of expressions,symbols or numbers). Finally if the x isn't ATOM or
     CONS we return x since a number evaluate to itself. */
  return typof(x) == ATOM ? assoc(x, e) :
         typof(x) == CONS ? apply(eval(car(x), e), cdr(x), e) :
         x;	/* in case we eval a number it lead to it self */
}

#if CONFIG_TRACE
static struct io_typ *trace_port;
static lexp trace_port_key;
lexp lexp_write(struct io_typ *port, lexp exp);

lexp trace_line(int indent, lexp line) {
  line = cons(line, nil);
  for (int i=indent; i>1; i--)
    line = cons(atom("  "), line);
  line = cons(sp, line);
  line = cons(atom("trace"), line);
  return line;
}

void print(lexp);
lexp eval(lexp x,lexp e) {
  static int indent = 0;
  indent++;
  lexp y = _eval(x,e);
  if (trace_port != NULL) {
    lexp line = cons(x, nil);
    line = cons(atom("eval"), line);
    line = trace_line(indent, line);
    lexp_write(trace_port, line);

    line = cons(y, nil);
    line = trace_line(indent+1, line);
    lexp_write(trace_port, line);
  }
  indent--;
  return y;
}

#else /* CONFIG_TRACE */
inline lexp eval(lexp x,lexp e) {
  return _eval(x, e);
}
#endif /* CONFIG_TRACE */

/* tokenization buffer and the next character that we are looking at */
char buf[40];
static char see = ' ';

/* return nonzero if we are looking at character c, ' ' means any white space */
bool seeing(char c) {
  return c == ' ' ? see <= c : see == c;
}

/* return a parsed atomic Lisp expression (a number or an atom) */
lexp atomic() {
  lexp n; iobj i;
  return (sscanf(buf, "%"NUM_FMT"%n", &n, &i) > 0 && !buf[i]) ? n :
         atom(buf);
}





/* Call a function for each char of a string. */
int str_for_each(const char *str, struct io_primitive *stream )
{
  for( ; *str != '\0'; str++ ) {
    int rc = stream->write(stream, *str);
    if ( rc != 0 )
      return rc;
  }
  return 0;
}


lexp stream_write(struct io_primitive *stream, lexp);

/* display a Lisp list t */
void printlist(struct io_primitive *stream, lexp t) {
  for (stream->write(stream, '('); ; stream->write(stream, ' ')) {
    stream_write(stream, car(t));
    t = cdr(t);
    if (typof(t) == NIL)
      break;
    if (typof(t) != CONS) {
      str_for_each(" . ", stream);
      stream_write(stream, t);
      break;
    }
  }
  stream->write(stream, ')');
}

int hsptyp_to_str(hsptyp n, char *str, size_t strsz){
  int rc = snprintf( str, strsz, "%u", n );
  if ( rc > 0 && rc < (int)strsz )
    return 0;
  return -1;
}

int num_to_str(lexp n, char *str, size_t strsz){
  int rc = snprintf( str, strsz, "%"NUM_FMT, n );
  if ( rc > 0 && rc < (int)strsz )
    return 0;
  return -1;
}


/* display a Lisp expression x */
lexp stream_write(struct io_primitive *stream, lexp x) {
  char indx_str[MAX_HSPTYP_STR];
  char num_str[MAX_DOUBLE_STR];
  struct primitive *prim = &__start_primitives;
  if (typof(x) == NIL) {
    /* TODO: check for error */
    str_for_each("()", stream);
  } else if (typof(x) == ATOM) {
    /* TODO: check for error */
    str_for_each(unbox_atom(x), stream);
  } else if (typof(x) == PRIM) {
    stream->write(stream, '<');
    /* TODO: check for error */
    str_for_each(prim[ord(x)].s, stream);
    stream->write(stream, '>');
  } else if (typof(x) == CONS) {
    printlist(stream, x);
  } else if (typof(x) == CLOS) {
    stream->write(stream, '{');
    /* TODO: check for error */
    hsptyp_to_str(ord(x), indx_str, sizeof(indx_str));
    /* TODO: check for error */
    str_for_each(indx_str, stream);
    stream->write(stream, '}');
  } else if (typof(x) == MACR) {
    stream->write(stream, '[');
    /* TODO: check for error */
    hsptyp_to_str(ord(x), indx_str, sizeof(indx_str));
    /* TODO: check for error */
    str_for_each(indx_str, stream);
    stream->write(stream, ']');
  } else if (typof(x) == BOX) {
    str_for_each("(box[", stream);
    /* TODO: check for error */
    hsptyp_to_str(ord(x), indx_str, sizeof(indx_str));
    /* TODO: check for error */
    str_for_each(indx_str, stream);
    str_for_each("])", stream);
  } else if (typof(x) == ERR) {
    str_for_each("(error ", stream);
    printlist(stream, unbox_err(x));
    str_for_each(")", stream);
  } else {
    num_to_str(x, num_str, sizeof(num_str));
    str_for_each(num_str, stream);
  }
  return nil;
}



/* advance to the next character */
void look(struct io_primitive *stream) {
  int c = stream->read(stream);
  see = c;
  if (c == EOF) {
    printf("Exit since EOF reached\n");
    exit(0);
  }
}

/* return the look ahead character from standard input, advance to the next */
char get(struct io_primitive *stream) {
  char c = see;
  look(stream);
  return c;
}

/* tokenize into buf[], return first character of buf[] */
char scan(struct io_primitive *stream) {
  iobj i = 0;
  while (seeing(' '))
    look(stream);
  if (seeing('(') || seeing(')') || seeing('\''))
    buf[i++] = get(stream);
  else
    do
      buf[i++] = get(stream);
    while (i < 39 && !seeing('(') && !seeing(')') && !seeing(' '));
  buf[i] = 0;
  return *buf;
}

lexp stream_read(struct io_primitive *stream);
lexp parse(struct io_primitive *stream);

/* return a parsed Lisp list */
lexp list(struct io_primitive *stream) {
  lexp x;
  if (scan(stream) == ')')
    return nil;
  if (!strcmp(buf, ".")) {
    x = stream_read(stream);
    scan(stream);
    return x;
  }
  x = parse(stream);
  return cons(x, list(stream));
}


/* return a parsed Lisp expression x quoted as (quote x) */
lexp quote(struct io_primitive *stream) {
  return cons(atom("quote"), cons(stream_read(stream), nil));
}


/* return a parsed Lisp expression */
lexp parse(struct io_primitive *stream) {
  return *buf == '(' ? list(stream) :
         *buf == '\'' ? quote(stream) :
         atomic(stream);
}


/* Deserialize character stream to a lisp expression */
/* Allocate memory and return lisp expression in it. */
lexp stream_read(struct io_primitive *stream) {
  scan(stream);
  return parse(stream);
}


lexp lexp_write(struct io_typ *port, lexp exp) {
  if ( port->write == NULL )
    return mk_error("The port could not be written:", nil);
  return port->write(port, exp);
}

lexp lexp_read(struct io_typ *port) {
  if ( port->read == NULL )
    return mk_error("The port could not be read:", nil);
  return port->read(port);
}



#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct io_typ* get_port(const char *name) {
  struct io_typ *port_iter = &__start_ports;
  for ( ; port_iter < &__stop_ports; ++port_iter) {
    if ( strcmp(port_iter->proto, name) == 0 )
      return port_iter;
  }
  return NULL;
}

/* garbage collection removes temporary cells, keeps global environment */
void gc() {
  sp = ord(env);
}

/* Lisp initialization and REPL */
int repl(void) {
  iobj i;
  struct module *mod_iter = &__start_modules;
  struct primitive *prim_iter = &__start_primitives;
  struct io_typ *default_port = get_port(CONFIG_DEFAULT_PORT);
  nil = box(NIL, 0);

  // Initialization of modules
  for ( ; mod_iter < &__stop_modules; ++mod_iter) {
    mod_iter->setup();
  }
  tru = atom("#t");
  env = pair(tru, tru, nil);
#if CONFIG_TRACE
  trace_port_key = atom("trace-port");
  lexp trace_port_value = atom(CONFIG_DEFAULT_TRACE_PORT);
  env = pair(trace_port_key, trace_port_value, env);
#endif
  for (i = 0; prim_iter < &__stop_primitives; ++i, ++prim_iter)
    env = pair(atom(prim_iter->s), box(PRIM, i), env);
  while (1) {
#if CONFIG_TRACE
    trace_port = get_port(unbox_atom(assoc(trace_port_key, env)));
#endif
    /* TODO: handle an error from the write call */
    lexp_write(default_port, eval(lexp_read(default_port), env));
    gc();
  }
}

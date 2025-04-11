#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>
#include <stddef.h> /* for NULL */
#include <io.h>

lexp car(lexp p);
lexp cdr(lexp p);
lexp evlis(lexp t, lexp e);
lexp lexp_write(struct io_typ *port, lexp exp);

struct io_typ* get_port(const char *name);
char* unbox_atom(lexp atom);

lexp f_write_to(lexp t, lexp e) {
        lexp port_name;
        struct io_typ *port;
        t = evlis(t, e);
        port_name = car(t);
        if (typof(port_name) != ATOM)
                return mk_error("Wrong type of first argument to write-to!. it should be an ATOM", port_name);
        port = get_port(unbox_atom(port_name));
        if (port == NULL)
                return mk_error("Couldn't find port called ", port_name);
        t = cdr(t);
        while (typof(t) != NIL) {
                lexp_write(port, car(t));
                t = cdr(t);
        }
        return t;
}


PRIMITIVE_SECTION struct primitive print_primitive[] = {
        {"write-to!", f_write_to},
};

static void init()
{
}

MODULE_SECTION struct module write_to_mod = {
        .setup = init,
};

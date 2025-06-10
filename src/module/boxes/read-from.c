#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>
#include <stddef.h> /* for NULL */
#include <io.h>

lexp car(lexp p);
lexp evlis(lexp t, lexp e);
lexp lexp_read(struct io_typ *port);

struct io_typ* get_port(const char *name);
char* unbox_atom(lexp atom);

lexp f_read_from(lexp t, lexp e) {
        lexp port_name;
        struct io_typ *port;
        t = evlis(t, e);
        port_name = car(t);
        if (typof(port_name) != ATOM)
                return mk_error("Wrong type of first argument to read-from!. It should be an ATOM", port_name);
        port = get_port(unbox_atom(port_name));
        if (port == NULL)
                return mk_error("Couldn't find port called ", port_name);
	return lexp_read(port);
}


PRIMITIVE_SECTION struct primitive read_from_primitive[] = {
        {"read-from!", f_read_from},
};

static void init()
{
}

MODULE_SECTION struct module read_from_mod = {
        .setup = init,
};


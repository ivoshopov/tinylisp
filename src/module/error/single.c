
#include <lexp.h>
#include <module/module.h>

static lexp err;

lexp mk_error( char str, lexp contex )
{
	return err;
}

lexp atom(const char *s);

static void init()
{
	err = atom("ERR");
}

struct module single_err = {
	.setup = init,
};


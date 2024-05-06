
#include <lexp.h>
#include <module/module.h>

static lexp err;

lexp _mk_error( char *file, char *line, char *str, lexp contex )
{
	return err;
}

lexp atom(const char *s);

static void init()
{
	err = atom("ERR");
}

MODULE_SECTION struct module single_err = {
	.setup = init,
};


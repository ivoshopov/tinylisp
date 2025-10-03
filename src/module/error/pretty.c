#include <lexp.h>
#include <module/module.h>
#include <stdio.h>
#include <unused.h>


lexp atom(const char *s);
lexp err(lexp err_msg);
lexp cons(lexp x, lexp y);
extern lexp nil;

lexp _mk_error( char *file, int line, char *str, lexp context )
{
        char err_buff[1024] = "";
        snprintf( err_buff, sizeof(err_buff), "%s", str );
        // atom() allocate memory from the heap on the fly. In case of out of
        // memory we probably won't see the error
        return err(cons(atom(err_buff),
				cons(context,nil)));
}

static void init()
{
}

MODULE_SECTION struct module pretty_err = {
	.setup = init,
};


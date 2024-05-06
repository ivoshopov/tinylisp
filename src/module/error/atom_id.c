#include <lexp.h>
#include <module/module.h>
#include <stdio.h>

lexp atom(const char *s);

lexp _mk_error( char *file, int line, char *str, lexp contex )
{
        char err_buff[20] = "";
        snprintf( err_buff, sizeof(err_buff), "ERR%d", line );
        // atom() allocate memory from the heap on the fly. In case of out of
        // memory we probably won't see the error
        return atom(err_buff);
}

static void init()
{
}

MODULE_SECTION struct module atom_id_err = {
	.setup = init,
};



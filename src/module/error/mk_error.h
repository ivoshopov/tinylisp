
#define mk_error( str, context )    _mk_error( __FILE__, __LINE__, str, context )
extern lexp _mk_error( char *file, int line, char *str, lexp contex );


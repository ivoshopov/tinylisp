#include <lexp.h>
#include <module/module.h>
#include <module/error/mk_error.h>
#include <primitive.h>



lexp car(lexp p);
lexp cdr(lexp p);
lexp evlis(lexp t, lexp e);
extern lexp nil;
extern lexp tru;
extern lexp cell[];
extern lexp env;

/* This it the test sequence. We check does set-box! second argument is in the
 * global environment. This way we guarantee that the BOX keeps valid content.
 * TODO: We do not release the memory from previous content of the BOX
 *
 * (define b (box '(list)))
 * 896:   define => <define>
 * 896:     box => <box>
 * 896:       quote => <quote>
 * 896:     (quote (list)) => (list)
 * 894:   (box (quote (list))) => (box[894])
 * 890: (define b (box (quote (list)))) => ()
 * ()
 * (set-box! b '(this is the stack list))
 * 870:   set-box! => <set-box!>
 * 870:   b => (box[894])
 * 870:     quote => <quote>
 * 870:   (quote (this is the stack list)) => (this is the stack list)
 * 866: (set-box! b (quote (this is the stack list))) => ERR24
 * ERR24
 * (define lst '(this is the second test))
 * 870:   define => <define>
 * 870:     quote => <quote>
 * 870:   (quote (this is the second test)) => (this is the second test)
 * 866: (define lst (quote (this is the second test))) => ()
 * ()
 * (set-box! b lst)
 * 860:   set-box! => <set-box!>
 * 860:   b => (box[894])
 * 860:   lst => (this is the second test)
 * 856: (set-box! b lst) => ()
 * ()
 * (unbox b)
 * 862:   unbox => <unbox>
 * 862:   b => (box[894])
 * 860: (unbox b) => (this is the second test)
 * (this is the second test)
 */

lexp f_set_box(lexp t, lexp e) {
        lexp arg;
        /* TODO: verify t is a BOX */
        t = evlis(t, e);
        arg = car(cdr(t));
        t = car(t);
        if( typof(t) == BOX ) {
                if( ord(arg) < ord(env) )
                        return mk_error("Argument isn't in the global envirment", t);
                cell[ord(t)+1] = arg;
                return nil;
        } else
                return mk_error("Argument is not a box:", t);
}

lexp f_box(lexp t, lexp e) {
        t = evlis(t, e);
        /* TODO: verify t is not an error */
        if( typof(t) == CONS )
                return box(BOX, ord(t));
        else
                return mk_error("Argument is not a cons (pair)", t);
}

lexp f_unbox(lexp t, lexp e) {
        t = evlis(t, e);
        t = car(t);
        if( typof(t) == BOX )
                return car(box(CONS, ord(t)));
        else
                return mk_error("Argument is not a box:", t);
}

lexp f_isbox(lexp t, lexp e) {
        t = evlis(t, e);
        t = car(t);
        return typof(t) == BOX ? tru : nil;
}

PRIMITIVE_SECTION struct primitive box_primitive[] = {
        {"box",    f_box},
        {"set-box!",f_set_box},
        {"unbox",  f_unbox},
        {"isbox",  f_isbox},
};

static void init()
{
}

MODULE_SECTION struct module boxes_array = {
        .setup = init,
};



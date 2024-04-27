import struct
import math

def lexp_type_tag(lexp):
    ''' Extract the type of the expression. It accept lexp converted to unsigned long long only '''
    return lexp >> 48

def lexp_ord(lexp):
    ''' Extract ordinal from lexp. It accept lexp converted to unsigned long long only '''
    return lexp.cast(gdb.lookup_type("unsigned int"))

def to_ull(val):
    ''' Convert lexp to unsigned long long '''
    # Take pointer to the lexp
    ref_to_lexp = val.reference_value()
    # cast it to "unsigned long long*"
    ref_to_lexp_as_ull = ref_to_lexp.reinterpret_cast(gdb.lookup_type("unsigned long long").reference())
    # Take the value as unsigned long long
    return ref_to_lexp_as_ull.referenced_value()


class LexpPrinter:

    ATOM = "0x7ff8"
    PRIM = "0x7ff9"
    CONS = "0x7ffa"
    CLOS = "0x7ffb"
    NIL  = "0x7ffc"
    STR  = "0x7ffd"
    tag_str = {
            ATOM: "ATOM",
            PRIM: "PRIM",
            CONS: "CONS",
            CLOS: "CLOS",
            NIL: "NIL",
            STR: "STR",
            }

    def __init__(self, val):
        self.val = val

    def to_string(self):
        if math.isnan(self.val):
            # Take the value as unsigned long long
            lexp_ull = to_ull(self.val)
            # Take the type of lexp
            lexp_type = self.tag_str[hex(lexp_type_tag(lexp_ull))]
            # Take the ordinal
            ordinal = lexp_ord(lexp_ull)
            if lexp_type == "ATOM":
                # In case of ATOM we can read and print the atom's string by using the "cell" - the heap memory
                return hex(lexp_ull) + " " + lexp_type + " cell[" + hex(ordinal) + "]=\"" + gdb.parse_and_eval('((char*)cell) + ' + hex(ordinal)).string() + "\""
            return hex(lexp_ull) + " " + lexp_type + " @" + hex(ordinal)
        else:
            return self.val.__float__()

def lookup_type(val):
    if str(val.type) == 'lexp':
        return LexpPrinter(val)
    return None

gdb.pretty_printers.append(lookup_type)


def cons_print(data, indent=0):
    ''' Print recursively data structure. It can be used for printing lisp environment.
        Example:
            (gdb) python cons_print("e")
            0x7ffa00000000039e CONS @0x39e
                    0x7ffa0000000003a0 CONS @0x3a0
                            0x7ff800000000005f ATOM cell[0x5f]="a"
                    1
            0x7ffa0000000003a8 CONS @0x3a8
                    0x7ffa0000000003aa CONS @0x3aa
                            0x7ff800000000005b ATOM cell[0x5b]="get"
                    0x7ff9000000000014 PRIM @0x14
    '''
    val = gdb.parse_and_eval(data)
    #if not math.isnan(val) or hex(lexp_type_tag(to_ull(val))) != LexpPrinter.CONS:
    #    print(f"Only CONS expresion is accepted. The type of expresion is: {LexpPrinter.tag_str[hex(lexp_type_tag(to_ull(val)))]}")
    #    return
    ind = "\t"*indent
    print(f'{ind}{val}')
    if math.isnan(val) and hex(lexp_type_tag(to_ull(val))) == LexpPrinter.CONS:
        cons_print('cell[' + hex(lexp_ord(to_ull(val))) + ' + 1]', indent+1)
        cons_print('cell[' + hex(lexp_ord(to_ull(val))) + ']', indent)
    #if math.isnan(val) and hex(lexp_type_tag(to_ull(val))) == LexpPrinter.NIL:
    #    return
    #cons_print('cell[' + hex(lexp_ord(to_ull(val))) + ' + 1]', indent+1)
    #print(f"{ind}\t{gdb.parse_and_eval('cell[' + hex(lexp_ord(to_ull(val))) + ' + 1]')}")
    #print(f"{ind}{gdb.parse_and_eval('cell[' + hex(lexp_ord(to_ull(val))) + ']')}")

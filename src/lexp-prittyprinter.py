import struct
import math

def lexp_type_tag(lexp):
    return lexp >> 48

def lexp_ord(lexp):
    return struct.unpack('<L', struct.pack('<d', lexp)[0:4])[0]

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
            # Take pointer to the lexp
            ref_to_lexp = self.val.reference_value()
            # cast it to "unsigned long long*"
            ref_to_lexp_as_ull = ref_to_lexp.reinterpret_cast(gdb.lookup_type("unsigned long long").reference())
            # Take the value as unsigned long long
            lexp_ull = ref_to_lexp_as_ull.referenced_value()
            # Take the type of lexp
            lexp_type = self.tag_str[hex(lexp_type_tag(lexp_ull))]
            # Take the ordinal
            ordinal = lexp_ull.cast(gdb.lookup_type("unsigned int"))
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

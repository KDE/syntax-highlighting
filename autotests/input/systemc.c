#include "systemc.h"

/*
 multi line comment
 */

// declare some module
SC_MODULE(gate)
{
    // inputs
    sc_in<bool> inA, inB;

    // outputs
    sc_out<bool> out;

    // C function
    void do_something()
    {
        out.write(inA.read() || inB.read());
    }

    // constructor
    SC_CTOR(gate)
    {
        // register method
        SC_METHOD(do_something);
    }
};

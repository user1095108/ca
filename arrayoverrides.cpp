#include "arrayoverrides.hpp"

#ifndef NO_CA_ARRAY_OVERRIDES

unsigned char * ca::copy_to_lim(ca::CopyToLim &args)
{
    unsigned char const *b   = args.b;
    unsigned char       *b_o = args.b_o;

    auto const numTillLim    = args.lim - b;
    auto const numTillEdge_o = args.edge_o - b_o;
    if (numTillLim > numTillEdge_o) {
        std::copy(b, b + numTillEdge_o, b_o);
        b += numTillEdge_o;
        b_o = args.start_o;
    }
    return std::copy(b, args.lim, b_o);
}

#endif

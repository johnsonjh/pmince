/* MINCE virtual filesystem */

#include "com.h"

#ifdef MINCE
extern char mince_com_name[], mince_com[];
extern int mince_com_size;
extern char mince_swp_name[], mince_swp[];
extern int mince_swp_size;
#endif

struct fakefile fakefs[] = {
#ifdef MINCE
    mince_com_name,
    mince_com,
    &mince_com_size,

    mince_swp_name,
    mince_swp,
    &mince_swp_size,
#endif
    0, 0, 0
};

int nfakefiles = sizeof(fakefs)/sizeof(fakefs[0]) - 1;

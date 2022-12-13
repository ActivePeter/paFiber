#pragma once
#include "iostream"

#ifdef __cplusplus
extern "C" {
#endif
    typedef void *BoostContext;

    typedef struct  {
        BoostContext fctx;
        void *data;
    } BoostContextFrom;

    BoostContext make_fcontext(void *sp,size_t size,void(*fn)(BoostContextFrom));
    BoostContextFrom jump_fcontext(BoostContext const to,void *vp);
    BoostContextFrom ontop_fcontext(BoostContext const to,void *vp,BoostContextFrom(*fn)(BoostContextFrom));

#ifdef __cplusplus
}
#endif

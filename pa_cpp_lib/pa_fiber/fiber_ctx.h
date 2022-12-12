#pragma once

#include <stdlib.h>
typedef void* (*coctx_pfn_t)( void* s, void* s2 );
struct coctx_param_t
{
	const void *s1;
	const void *s2;
};
struct PaFiberCtx
{
#if defined(__i386__)
	void *regs[ 8 ];
#else
	void *regs[ 14 ];
#endif
	size_t ss_size;
	char *ss_sp;
	
};

int coctx_init( PaFiberCtx *ctx );
int coctx_make( PaFiberCtx *ctx,coctx_pfn_t pfn,const void *s,const void *s1 );
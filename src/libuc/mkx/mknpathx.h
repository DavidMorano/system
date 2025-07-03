/* mknpathx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a file-path from multiple component c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKNPATHX_INCLUDE
#define	MKNPATHX_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<mknpathxw.h>


EXTERNC_begin

extern int mknpathx(char *,int,int,...) noex ;

extern int mknpath1(char *,int,cc *) noex ;
extern int mknpath2(char *,int,cc *,cc *) noex ;
extern int mknpath3(char *,int,cc *,cc *,cc *) noex ;
extern int mknpath4(char *,int,cc *,cc *,cc *,cc *) noex ;
extern int mknpath5(char *,int,cc *,cc *,cc *,cc *,cc *) noex ;
extern int mknpath6(char *,int,cc *,cc *,cc *,cc *,cc *,cc *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline int mknpath(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return mknpathx(dp,dl,na,args ...) ;
}

#endif /* __cplusplus */


#endif /* MKNPATHX_INCLUDE */



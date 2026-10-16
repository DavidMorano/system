/* sncpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNCPYXC_INCLUDE
#define	SNCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uconcepts.hh>		/* LIBU */


EXTERNC_begin

extern int sncpybcx	(char *,int,int,...) noex ;
extern int sncpylcx	(char *,int,int,...) noex ;
extern int sncpyucx	(char *,int,int,...) noex ;
extern int sncpyfcx	(char *,int,int,...) noex ;

extern int sncpybc	(char *,int,cchar *) noex ;
extern int sncpylc	(char *,int,cchar *) noex ;
extern int sncpyuc	(char *,int,cchar *) noex ;
extern int sncpyfc	(char *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus
template<concharp ... Args>
inline int sncpybc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return sncpybcx(dp,dl,na,args ...) ;
} /* end subroutine-template (sncpy) */
template<concharp ... Args>
inline int sncpylc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return sncpylcx(dp,dl,na,args ...) ;
} /* end subroutine-template (sncpy) */
template<concharp ... Args>
inline int sncpyuc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return sncpybux(dp,dl,na,args ...) ;
} /* end subroutine-template (sncpy) */
template<concharp ... Args>
inline int sncpyfc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return sncpyfcx(dp,dl,na,args ...) ;
} /* end subroutine-template (sncpy) */
#endif /* __cplusplus */

#endif /* SNCPYXC_INCLUDE */



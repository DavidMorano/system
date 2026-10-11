/* stpcpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STPCPYXC_INCLUDE
#define	STPCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	* stpcpybcx(char *,int,...) noex ;
extern char	* stpcpylcx(char *,int,...) noex ;
extern char	* stpcpyucx(char *,int,...) noex ;
extern char	* stpcpyfcx(char *,int,...) noex ;

extern char	* stpcpybc (char *,cchar *) noex ;
extern char	* stpcpylc (char *,cchar *) noex ;
extern char	* stpcpyuc (char *,cchar *) noex ;
extern char	* stpcpyfc (char *,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline char *stpcpybc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return stpcpybcx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *stpcpylc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return stpcpylcx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *stpcpyuc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return stpcpyucx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *stpcpyfc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return stpcpyfcx(dp,na,args ...) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* STPCPYXC_INCLUDE */



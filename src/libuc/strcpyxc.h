/* strcpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRCPYXC_INCLUDE
#define	STRCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	* strcpybcx(char *,int,...) noex ;
extern char	* strcpylcx(char *,int,...) noex ;
extern char	* strcpyucx(char *,int,...) noex ;
extern char	* strcpyfcx(char *,int,...) noex ;

extern char	* strcpybc (char *,cchar *) noex ;
extern char	* strcpylc (char *,cchar *) noex ;
extern char	* strcpyuc (char *,cchar *) noex ;
extern char	* strcpyfc (char *,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline char *strcpybc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return strcpybcx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strcpylc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return strcpylcx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strcpyuc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return strcpyucx(dp,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strcpyfc(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return strcpyfcx(dp,na,args ...) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* STRCPYXC_INCLUDE */



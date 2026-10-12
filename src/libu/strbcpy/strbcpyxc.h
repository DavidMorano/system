/* strbcpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRBCPYXC_INCLUDE
#define	STRBCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	* strbcpybcx(char *,int,int,...) noex ;
extern char	* strbcpylcx(char *,int,int,...) noex ;
extern char	* strbcpyucx(char *,int,int,...) noex ;
extern char	* strbcpyfcx(char *,int,int,...) noex ;

extern char	* strbcpybc (char *,int,cchar *) noex ;
extern char	* strbcpylc (char *,int,cchar *) noex ;
extern char	* strbcpyuc (char *,int,cchar *) noex ;
extern char	* strbcpyfc (char *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline char *strbcpybc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return strbcpybcx(dp,dl,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strbcpylc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return strbcpylcx(dp,dl,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strbcpyuc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return strbcpyucx(dp,dl,na,args ...) ;
} /* end subroutine */
template<typename ... Args>
inline char *strbcpyfc(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return strbcpyfcx(dp,dl,na,args ...) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* STRBCPYXC_INCLUDE */



/* mklogid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* make a log-ID c-string, suitable for logging purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
	= 2018-11-27, David A-D- Morano
	I created an inline version for the C language. We will
	likely do something for C++ at some other time.

	= 2023-10-23, David A-D- Morano
	At some point I added an inline (here) for the C++ language.

*/

/* Copyright (c) 1998,2018,23023 David A-D- Morano.  All rights reserved. */

#ifndef	MKLOGID_INCLUDE
#define	MKLOGID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mklogid(char *,int,cchar *,int,int) noex ;
extern int	mklogidpre(char *,int,cchar *,int) noex ;
extern int	mklogidsub(char *,int,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libuc {
    struct logdigmaxer {
	int mkdigmax() noex ;
	operator int () noex {
	    return mkdigmax() ;
	} ;
	int operator () () noex {
	    return mkdigmax() ;
	} ;
    } ; /* end struct (logdigmaxer) */
}

extern libuc::logdigmaxer	logdigmax ;

#endif /* __cplusplus */


#endif /* MKLOGID_INCLUDE */



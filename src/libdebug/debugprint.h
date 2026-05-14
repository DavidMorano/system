/* debugprint HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBUGPRINT_INCLUDE
#define	DEBUGPRINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define DEBUGPRINTF(FMT, ...)					\
    debugprintf("%s: ",__func__) ;				\
    debugprintf(FMT __VA_OPT__(,) __VA_ARGS__)


EXTERNC_begin

extern int debugopen(cchar *) noex ;
extern int debugclose(void) noex ;
extern int debugprint(cchar *,int) noex ;
extern int debugprintf(cchar *,...) noex ;
extern int debugprinthexblock(cchar *,int,cvoid *,int) noex ;
extern int debugprinthex(cchar *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* DEBUGPRINT_INCLUDE */



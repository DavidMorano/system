/* progfindprog HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find the program to execute */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGFINDPROG_INCLUDE
#define	PROGFINDPROG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<sysbase.h>		/* LIBU */
#include	<proginfo.hh>


EXTERNC_begin

extern int progfindprog(proginfo *,char *,ccha *) noex ;

EXTERNC_end


#endif /* PROGFINDPROG_INCLUDE */



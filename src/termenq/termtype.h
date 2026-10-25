/* termtype HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* terminal types (of a sort) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-12-16, David A­D­ Morano
	Updated.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	TERMTYPE_INCLUDE
#define	TERMTYPE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<localmisc.h>		/* for special types */


#define	TERMTYPE	struct termtype


struct termtype {
	cchar		*name ;
	short		pv[4] ;
	short		sv[4] ;
} ; /* end struct */

EXTERNC_begin
extern int termtype(const TERMTYPE *,const short *,const short *) ;
EXTERNC_end


#endif /* TERMTYPE_INCLUDE */



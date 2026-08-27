/* utimezone HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UTIMEZONE retrieval */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UTIMEZONE_INCLUDE
#define	UTIMEZONE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU */


EXTERNC_begin

extern int u_timezone	(TIMEZONE *) noex ;
extern int u_tzname	(char *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
namespace libu {
    extern int uftime(timeb *) noex ;
    extern int ugettimeofday(timeval *,TIMEZONE *) noex ;
} /* end namespace (libu) */
#endif /* __cplusplus */


#endif /* UTIMEZONE_INCLUDE */



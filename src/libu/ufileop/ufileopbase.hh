/* ufileopbase HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	ufileopbase

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILEOPBASE_INCLUDE
#define	UFILEOPBASE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    struct ufileopbaseflags {
	uint		fintr:1 ;	/* request interrupts to return */
    } ; /* end struct (ufileopbaseflags) */
    struct ufileopbase {
	ufileopbaseflags	fl{} ;
	ufileopbase() noex { } ;
	int operator () (cchar *) noex ;
	virtual int callstd(cchar *) noex = 0 ;
    } ; /* end struct (ufileopbase) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* UFILEOPBASE_INCLUDE */



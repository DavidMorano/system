/* utbase HEADER (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UTBASE_INCLUDE
#define	UTBASE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libut {
    struct utbase_fl {
	uint		brkinprogress:1 ;	/* 'in-progress' returns */
	uint		brkintr:1 ;		/* interrupts return */
    } ; /* end struct (utbase_fl) */
    struct utbase {
	UTBIND		*utbindp ;
	UTCALL		*utcallp ;
	UTINFO		*utinfop ;
	utbase_fl	fl{} ;
	utbase		() noex { } ;
	int operator () (int) noex ;
	virtual int callstd(int) noex = 0 ;
    } ; /* end struct (utbase) */
} /* end namespace (libut) */


#endif /* __cplusplus */
#endif /* UTBASE_INCLUDE */



/* usyscallbase HEADER (UNIX® file operations) */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	usyscallbase

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	USYSCALLBASE_INCLUDE
#define	USYSCALLBASE_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    struct usyscallbaseflags {
	uint		fclose:1 ;
	uint		fwrite:1 ;
	uint		fintr:1 ;	/* request interrupts to return */
    } ; /* end struct (usyscallbaseflags) */
    struct usyscallbase {
	usyscallbaseflags	f{} ;
	int		fd = 0 ;
	usyscallbase() noex { } ;
	virtual int callstd() noex = 0 ;
	int handler() noex ;
	int uwrcheck() noex ;
    } ; /* end struct (usyscallbase) */
}


#endif /* __cplusplus (C++ only) */
#endif /* USYSCALLBASE_INCLUDE */



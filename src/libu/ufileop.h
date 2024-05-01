/* ufileop SUPPORT (UNIX® file operations) */
/* lang=C20 */

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

#ifndef	UFILEOP_INCLUDE
#define	UFILEOP_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>		/* |SOCKADDR| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |intptr_t| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<clanguage.h>


#ifdef	__cplusplus

namespace ufileop {
    struct ufileopflags {
	uint		fclose:1 ;
	uint		fwrite:1 ;
	uint		fintr:1 ;	/* request interrupts to return */
    } ; /* end struct (ufileopflags) */
    struct ufileopbase {
	cchar		*fn ;
	ufileopflags	f{} ;
	ufileopbase() noex { } ;
	int operator () (int) noex ;
	virtual int callstd(int) noex = 0 ;
    } ; /* end struct (ufileopbase) */
}

#endif /* __cplusplus */

EXTERNC_begin


EXTERNC_end


#endif /* UFILEOP_INCLUDE */



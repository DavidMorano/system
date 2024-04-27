/* ufiledesc SUPPORT (UNIX® System V System C Inter-Process-Communication) */
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

#ifndef	UFILEDESC_INCLUDE
#define	UFILEDESC_INCLUDE


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

namespace ufiledesc {
    struct ufiledescbase ;
    typedef int (ufiledescbase::*ufiledescbase_m)(int) noex ;
    struct ufiledescbase {
	ufiledescbase_m	m = nullptr ;
	int		sal ;
	SOCKADDR	*sap = nullptr ;
	ufiledescbase() noex : fd(-1), sal(0) { } ;
	int operator () (int) noex ;
    } ; /* end struct (ufiledescbase) */
}

#endif /* __cplusplus */


#endif /* UFILEDESC_INCLUDE */



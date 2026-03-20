/* ufiledesc HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
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
	ufiledesc

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILEDESC_INCLUDE
#define	UFILEDESC_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	<uatx.h>
#include	<uconnect.h>
#include	<ufcntl.h>
#include	<ugetdents.h>
#include	<ulockfile.h>
#include	<ureade.h>
#include	<ureadext.h>
#include	<uregular.h>
#include	<uselect.h>
#include	<usocket.h>
#include	<usockaddr.h>


namespace libu {
    int upoll(POLLFD *,int,int) noex ;
    int uselect(int,fdset *,fdset *,fdset *,TIMEVAL *) noex ;
} /* end namespace (libu) */


#endif /* UFILEDESC_INCLUDE */



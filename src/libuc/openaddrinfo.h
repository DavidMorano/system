/* openaddrinfo HEADER */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	OPENADDRINFO_INCLUDE
#define	OPENADDRINFO_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucopen.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int openaddrinfo(ADDRINFO *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* OPENADDRINFO_INCLUDE */



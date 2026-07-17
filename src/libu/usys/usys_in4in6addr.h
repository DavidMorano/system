/* usys_onc HEADER (Open-Network-Compuring) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSIN4IN6ADDR_INCLUDE
#define	USYSIN4IN6ADDR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<netinet/in.h>		/* |in_addr| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#if	defined(SYSHAS_IN4IN6ADDR) && (SYSHAS_IN4IN6ADDR > 0)

/* nothing needed */

#else /* defined(SYSHAS_IN4IN6ADDR) && (SYSHAS_IN4IN6ADDR > 0) */

struct in4in6_addr {
	u_int32_t               i46a_pad32[3];
	struct in_addr          i46a_addr4;
} ; /* end struct (in4in6_addr) */
 
#endif /* defined(SYSHAS_IN4IN6ADDR) && (SYSHAS_IN4IN6ADDR > 0) */


#endif /* USYSIN4IN6ADDR_INCLUDE */



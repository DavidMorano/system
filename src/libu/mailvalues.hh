/* mailvalues HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* various mail (message) values */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailvalues

	Description:
	This object contains various mail (message) values.

*******************************************************************************/

#ifndef	MAILVALUES_INCLUDE
#define	MAILVALUES_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


struct mailvalues {
	static int	nodenamemult ;
	static int	hostnamemult ;
	static int	mailaliaslen ;
} ; /* end struct (mailvalues) */


extern mailvalues	mailvalue ;


#endif /* __cplusplus (C++ only) */
#endif /* MAILVALUES_INCLUDE */



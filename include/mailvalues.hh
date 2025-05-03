/* mailvalues HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


struct mailvalues {
	static const int	nodenamemult ;
	static const int	hostnamemult ;
	static const int	mailaliaslen ;
} ; /* end struct (mailvalues) */


extern const mailvalues		mailvalue ;


#endif	/* __cplusplus */
#endif /* MAILVALUES_INCLUDE */



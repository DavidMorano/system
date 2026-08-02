/* nlscodes HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* information to create and read an NLS-connect message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Name:
	nlscodes

  	Description:
	This header file contains some pre-processor defines used to
	create an NLS connection message and to parse the result.

	Definitions:
	NLPS		Network-Listener-Process-Server
	NLS		Network-Listener-Service

*******************************************************************************/

#ifndef	NLSCODES_INCLUDE
#define	NLSCODES_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<nlsmsg.h>		/* <- standard defines */


struct nlscodes {
    	static constexpr int	start		= NLSSTART ;
    	static constexpr int	format		= NLSFORMAT ;
    	static constexpr int	sunknown	= NLSUNKNOWN ;
    	static constexpr int	disabled	= NLSDISABLED ;
} ; /* end struct (nlscodes) */

extern const nlscodes	nlscode ;


#endif /* __cplusplus */
#endif /* NLSCODES_INCLUDE */



/* ucopeninfo INCLUDE */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Additional support for the UNIX® OPEN facility.

*******************************************************************************/

#ifndef	UCOPENINFO_INCLUDE
#define	UCOPENINFO_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


struct ucopeninfo {
	cchar		**envv ;
	cchar		*fname ;
	mode_t		operms ;
	int		clinks ;
	int		oflags ;
	int		to ;
	int		opts ;
} ;


#endif /* UCOPENINFO_INCLUDE */



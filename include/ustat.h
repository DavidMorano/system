/* ustat HEADER */
/* lang=C20 */

/* UNIX® USTAT support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX® USTAT related
	operations.

*******************************************************************************/

#ifndef	USTAT_INCLUDE
#define	USTAT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifndef	S_IFNAM
#define	S_IFNAME	0x5000		/* taken from Solaris® */
#endif


/* possible missing S_ISNAM */

#ifndef	S_ISNAM
#ifdef	S_IFNAM
#define	S_ISNAM(mode)	(((mode)&S_IFMT) == S_IFNAM)
#else
#define	S_ISNAM(mode)	0
#endif
#endif /* S_ISNAM */


#endif /* USTAT_INCLUDE */



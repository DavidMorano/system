/* usys_stat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® USYSSTAT support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_stat

	Description:
	This file contains some support for the UNIX® USYSSTAT related
	operations.

*******************************************************************************/

#ifndef	USYSSTAT_INCLUDE
#define	USYSSTAT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


/* missing from some operating systems */
#ifndef	S_IAMB
#define	S_IAMB		0x1FF		/* mask for the permissions field */
#endif
#ifndef	S_IFNAM
#define	S_IFNAM		0x5000		/* MicroSoft XENIX® named file */
#endif
#ifndef	S_IFDOOR
#define	S_IFDOOR	0xD000		/* Solaris® "door" file */
#endif
#ifndef	S_ISNAM
#define	S_ISNAM(fm)	(((fm) & S_IFMT) == S_IFNAM)
#endif
#ifndef	S_ISDOOR
#define	S_ISDOOR(fm)	(((fm) & S_IFMT) == S_IFDOOR)
#endif


#endif /* USYSSTAT_INCLUDE */



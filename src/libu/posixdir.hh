/* posixdir HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* help interface to POSIX |dirent(3c)| facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	posixdir

	Description:
	See the support source file for more information (and
	commentary) on this object.

*******************************************************************************/

#ifndef	POSIXDIR_INCLUDE
#define	POSIXDIR_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |off_t| */
#include	<dirent.h>		/* POSIX® |DIR(3c)| + |dirent_t(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	POSIXDIR_MAGIC	0x97634525


#endif	/* __cplusplus */
#endif /* POSIXDIR_INCLUDE */



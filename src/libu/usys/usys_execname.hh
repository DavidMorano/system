/* usys_execname HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the file-name (file-path) that was used to 'exec' this program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_execname

	Description:
	This file contains the UNIX® system types or subroutines
	that the brain-damaged MacOS operating system does NOT have.
	We are trying in a very small way to make up for some of
	the immense brain-damage within the Apple Darwin operating
	system.

*******************************************************************************/

#ifndef	USYSEXECNAME_INCLUDE
#define	USYSEXECNAME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® system-types */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


namespace usys {
    extern sysret_t usys_execname(char *,int) noex ;
} /* end namespace (usys) */


#endif /* __cplusplus */
#endif /* USYSEXECNAME_INCLUDE */



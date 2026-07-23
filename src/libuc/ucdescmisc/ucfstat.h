/* ucfstat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_fstat

  	Description:
	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UCFSTAT_INCLUDE
#define	UCFSTAT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_fstat(int,ustat *)	noex ;
extern int	uc_fsize(int)		noex ;
extern int	uc_fuid(int)		noex ;
extern int	uc_fgid(int)		noex ;

EXTERNC_end


#endif /* UCFSTAT_INCLUDE */



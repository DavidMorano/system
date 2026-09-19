/* utimeutil HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® Memory-Management interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This file contains the subroutine declarations for various
	UNIX® system Memory-Management subroutines.

	Names:
	errtimer
	itimers
	itimerspec
	itimerval
	timecount
	timespec
	timeval
	timewatch
	utimeutiloutdefs
	utimeutilouts

*******************************************************************************/

#ifndef	UTIMEUTIL_INCLUDE
#define	UTIMEUTIL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */

#include	<errtimer.hh>
#include	<itimers.hh>
#include	<itimerspec.h>
#include	<itimerval.h>
#include	<timecount.hh>
#include	<timespec.h>
#include	<timeval.hh>
#include	<timewatch.hh>
#include	<utimeutiloutdefs.h>
#include	<utimeutilouts.h>


#endif /* UTIMEUTIL_INCLUDE */



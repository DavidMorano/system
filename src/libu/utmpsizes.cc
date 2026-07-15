/* utmpsizes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UTMP entry types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Miscellaneous:

	Description:
	This contains support for UTMP entry types.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utmpx.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"utmpsizes.hh"


/* local defines */

enum missingutmpsizes {
	missingutmpsize_synthetic = 100,
	missingutmpsize_signature,
	missingutmpsize_timeshut,
	missingutmpsize_overlast
} ; /* end enum */

#ifndef	ACCOUNTING
#define ACCOUNTING	9
#endif

#ifndef	SIGNATURE
#define SIGNATURE	missingutmpsize_signature
#endif

#ifndef	SHUTDOWN_TIME
#ifdef	DOWN_TIME
#define	SHUTDOWN_TIME	DOWN_TIME
#else
#define SHUTDOWN_TIME	missingutmpsize_timeshut
#endif /* DOWN_TIME */
#endif /* SHUTDOWN_TIME */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

cint utmpsizes::id		= szof(utmpx::ut_id) ;
cint utmpsizes::user		= szof(utmpx::ut_user) ;
cint utmpsizes::line		= szof(utmpx::ut_line) ;
cint utmpsizes::host		= szof(utmpx::ut_host) ;

const utmpsizes			utmpsize ;



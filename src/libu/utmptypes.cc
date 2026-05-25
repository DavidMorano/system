/* utmptypes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

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

#include	"utmptypes.hh"


/* local defines */

enum missingutmptypes {
	missingutmptype_synthetic = 100,
	missingutmptype_signature,
	missingutmptype_timeshut,
	missingutmptype_overlast
} ; /* end enum */

#ifndef	ACCOUNTING
#define ACCOUNTING	9
#endif

#ifndef	SIGNATURE
#define SIGNATURE	missingutmptype_signature
#endif

#ifndef	SHUTDOWN_TIME
#ifdef	DOWN_TIME
#define	SHUTDOWN_TIME	DOWN_TIME
#else
#define SHUTDOWN_TIME	missingutmptype_timeshut
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

cint utmptypes::empty		= EMPTY ;
cint utmptypes::runlevel	= RUN_LVL ;
cint utmptypes::boottime	= BOOT_TIME ;
cint utmptypes::timeold		= OLD_TIME ;
cint utmptypes::timenew		= NEW_TIME ;
cint utmptypes::procinit	= INIT_PROCESS ;
cint utmptypes::proclogin	= LOGIN_PROCESS ;
cint utmptypes::procuser	= USER_PROCESS ;
cint utmptypes::procdead	= DEAD_PROCESS ;
cint utmptypes::account		= ACCOUNTING ;
cint utmptypes::signature	= SIGNATURE ;
cint utmptypes::timeshut	= SHUTDOWN_TIME ;

const utmptypes			utmptype ;



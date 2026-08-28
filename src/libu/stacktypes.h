/* stacktypes HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* enumerate the various stack types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STACKTYPES_INCLUDE
#define	STACKTYPES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


enum stacktypes {
	stacktype_thread,
	stacktype_default,
	stacktype_overlast
} ; /* end enum */


#endif /* STACKTYPES_INCLUDE */



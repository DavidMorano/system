/* ucproctypes HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® C-language library process types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCPROCTYPES_INCLUDE
#define	UCPROCTYPES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum ucproctypes {
	ucproctype_all,
	ucproctype_user,
	ucproctype_sys,
	ucproctype_session,
	ucproctype_group,
	ucproctype_overlast
} ;


#endif /* UCPROCTYPES_INCLUDE */



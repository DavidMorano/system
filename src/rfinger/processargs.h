/* processargs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Process-Arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROCESSARGS_INCLUDE
#define	PROCESSARGS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>

#include	<proginfo.h>


EXTERNC_begin

extern int processargs(proginfo *pip,cchar *args,vecstr *alp) noex ;

EXTERNC_end


#endif /* PROCESSARGS_INCLUDE */



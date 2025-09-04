/* usigsets HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* File-Once management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USIGSETS_INCLUDE
#define	USIGSETS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<signal.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int u_sigsetempty	(sigset_t *) noex ;
extern int u_sigsetfill		(sigset_t *) noex ;
extern int u_sigsetadd		(sigset_t *,int) noex ;
extern int u_sigsetdel		(sigset_t *,int) noex ;
extern int u_sigsetismem	(const sigset_t *,int) noex ;

EXTERNC_end


#endif /* USIGSETS_INCLUDE */



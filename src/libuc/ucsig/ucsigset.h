/* ucsigset HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSIGSET_INCLUDE
#define	UCSIGSET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_sigsetempty	(mut sigset_t *) noex ;
extern int uc_sigsetfill	(mut sigset_t *) noex ;
extern int uc_sigsetadd		(mut sigset_t *,int) noex ;
extern int uc_sigsetdel		(mut sigset_t *,int) noex ;
extern int uc_sigsetismem	(con sigset_t *,int) noex ;

EXTERNC_end


#endif /* UCSIGSET_INCLUDE */



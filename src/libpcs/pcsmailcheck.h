/* pcsmailcheck HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS get mail-host for a given user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSMAILCHECK_INCLUDE
#define	PCSMAILCHECK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	pcsmailcheck(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSMAILCHECK_INCLUDE */



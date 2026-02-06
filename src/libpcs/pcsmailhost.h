/* pcsmailhost HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS get mail-host for a given user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSMAILHOST_INCLUDE
#define	PCSMAILHOST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	pcsmailhost(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSMAILHOST_INCLUDE */



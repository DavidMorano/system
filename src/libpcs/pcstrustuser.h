/* pcstrustusers HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* is a user a PCS-trusted user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSTRUSTUSER_INCLUDE
#define	PCSTRUSTUSER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>


EXTERNC_begin

extern int	pcstrustuser(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* PCSTRUSTUSER_INCLUDE */



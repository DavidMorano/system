/* getclusters HEADER (Check-ONC) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get all clusters for the specified nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	GETCLUSTERS_INCLUDE
#define	GETCLUSTERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


EXTERNC_begin

extern int	getclusters(cchar *,vecstr *,cchar *) noex ;

EXTERNC_end


#endif /* GETCLUSTERS_INCLUDE */



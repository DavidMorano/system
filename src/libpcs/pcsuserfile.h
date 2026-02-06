/* pcsuserfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* update a record in a PCS userfile */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSUSERFILE_INCLUDE
#define	PCSUSERFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int pcsuserfile(cc *,cc *,cc *,cc *,cc *) noex ;

EXTERNC_end


#endif /* PCSUSERFILE_INCLUDE */



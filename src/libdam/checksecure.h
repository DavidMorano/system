/* checksecure HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* format a number for a three-column-wide field for the VMAIL program */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-09-01, Dave morano
	This was originally made for the PCSPOLL program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CHECKSECURE_INCLUDE
#define	CHECKSECURE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	securefile(cchar *,uid_t,gid_t) noex ;

EXTERNC_end


#endif /* CHECKSECURE_INCLUDE */



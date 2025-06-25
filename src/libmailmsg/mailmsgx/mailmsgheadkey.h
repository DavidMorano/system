/* mailmsgheadkey HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* extract the key from a mail-msg header field (c-string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGHEADKEY_INCLUDE
#define	MAILMSGHEADKEY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mailmsgheadkey(cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* MAILMSGHEADKEY_INCLUDE */



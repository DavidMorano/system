/* openqotd HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENQOTD_INCLUDE
#define	OPENQOTD_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	openqotd(cchar *,int,int,int) noex ;

EXTERNC_end


#endif /* OPENQOTD_INCLUDE */



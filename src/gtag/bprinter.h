/* bprinter HREADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the event of finding a citætion tag */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	BPRINTER_INCLUDE
#define	BPRINTER_INCLUDE

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */


EXTERNC_begin

extern int	bprinter(bfile *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* BPRINTER_INCLUDE */



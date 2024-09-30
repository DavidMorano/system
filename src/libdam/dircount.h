/* dircount HEADER */
/* lang=C20 */

/* retrieve the number of files in a specified directory */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRCOUNT_INCLUDE
#define	DIRCOUNT_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	dircount(cchar *) noex ;

EXTERNC_end


#endif /* DIRCOUNT_INCLUDE */



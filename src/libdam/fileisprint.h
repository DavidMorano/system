/* fileisprint HEADER (file-is-printable) */
/* lang=C20 */

/* is the given file totally printable? */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILEISPRINT_INCLUDE
#define	FILEISPRINT_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	fileisprint(cchar *) noex ;

EXTERNC_end


#endif /* FILEISPRINT_INCLUDE */



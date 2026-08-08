/* varsub_expfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* module to handle variable substitution in strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARSUBEXPFILE_INCLUDE
#define	VARSUBEXPFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<varsub.h>		/* LIBUC */
#include	<bfile.h>		/* LIBB */


EXTERNC_begin

extern int	varsub_expfile(varsub *,bfile *,bfile *) noex ;

EXTERNC_end


#endif /* VARSUBEXPFILE_INCLUDE */



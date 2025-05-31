/* statvfsdir HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* like |statvfs(2)| but will not return zero blocks on automounts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STATVFSDIR_INCLUDE
#define	STATVFSDIR_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	statvfsdir(cchar *,USTATVFS *) noex ;

EXTERNC_end


#endif /* STATVFSDIR_INCLUDE */



/* pcsopendircache HEADER */
/* lang=C20 */

/* open a channel (file-descriptor) to the directory-cache of a directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSOPENDIRCACHE_INCLUDE
#define	PCSOPENDIRCACHE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


EXTERNC_begin

extern int pcsopendircache(cchar *,cchar *,int,mode_t,int) noex ;

EXTERNC_end


#endif /* PCSOPENDIRCACHE_INCLUDE */



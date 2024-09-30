/* fsdirtreestat HEADER */
/* lang=C20 */

/* get status (??) on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FSDIRTREESTAT_INCLUDE
#define	FSDIRTREESTAT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	FSDIRTREE_STAT
#define	FSDIRTREE_STAT		USTAT
#endif


EXTERNC_begin

extern int	fsdirtreestat(cchar *,int,FSDIRTREE_STAT *) noex ;

EXTERNC_end


#endif /* FSDIRTREESTAT_INCLUDE */



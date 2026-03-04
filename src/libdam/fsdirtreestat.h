/* fsdirtreestat HEADER */
/* charset=ISO8859-1 */
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
#include	<sys/stat.h>
#include	<clanguage.h>
#include	<usysbase.h>


#ifndef	FSDIRTREE_STAT
#define	FSDIRTREE_STAT		ustat
#endif

enum typestats {
    	typestat_reg,
    	typestat_lnk,
    	typestat_overlast
} ; /* end enum (typestats) */

EXTERNC_begin

extern int	fsdirtreestat(cchar *,int,ustat *) noex ;

EXTERNC_end


#endif /* FSDIRTREESTAT_INCLUDE */



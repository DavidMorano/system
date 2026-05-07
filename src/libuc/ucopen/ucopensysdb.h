/* ucopensysdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get random data from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCOPENSYSDB_INCLUDE
#define	UCOPENSYSDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sysdbfiles.h>


EXTERNC_begin

extern int	uc_opensysdb(sysdbfiles,int,mode_t) noex ;

EXTERNC_end


#endif /* UCOPENSYSDB_INCLUDE */



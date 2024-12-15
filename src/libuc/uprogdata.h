/* uprogdata SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® program-data cache */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPROGDATA_INCLUDE
#define	UPROGDATA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* registery */
#define	UPROGDATA_DNETLOAD	0	/* netload */
#define	UPROGDATA_DSYSTAT	1	/* sys-stat */


EXTERNC_begin

extern int uprogdata_init() noex ;
extern int uprogdata_set(int,cchar *,int,int) noex ;
extern int uprogdata_get(int,char *,int) noex ;
extern int uprogdata_fini() noex ;

EXTERNC_end


#endif /* UPROGDATA_INCLUDE */



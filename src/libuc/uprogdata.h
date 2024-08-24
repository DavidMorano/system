/* uprogdata SUPPORT */
/* lang=C20 */

/* UNIX® program-data cache */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPROGDATA_INCLUDE
#define	UPROGDATA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* registery */
#define	UPROGDATA_DNETLOAD	0	/* netload */
#define	UPROGDATA_DSYSTAT	1	/* sys-stat */


EXTERNC_begin

extern int uprogdata_init() ;
extern int uprogdata_set(int,cchar *,int,int) ;
extern int uprogdata_get(int,char *,int) ;
extern void uprogdata_fini() ;

EXTERNC_end


#endif /* UPROGDATA_INCLUDE */



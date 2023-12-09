/* strtox INCLUDE */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOX_INCLUDE
#define	STRTOX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int uc_strtoi(cchar *,cchar **,int,int *) noex ;
extern int uc_strtol(cchar *,cchar **,int,long *) noex ;
extern int uc_strtoll(cchar *,cchar **,int,longlong *) noex ;
extern int uc_strtoui(cchar *,cchar **,int,uint *) noex ;
extern int uc_strtoul(cchar *,cchar **,int,ulong *) noex ;
extern int uc_strtoull(cchar *,cchar **,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRTOX_INCLUDE */



/* getgid HEADER */
/* lang=C20 */

/* get the GID for a given group-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETGID_INCLUDE
#define	GETGID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getgid_name(cchar *,int) noex ;
extern int getgid_group(cchar *,int) noex ;
extern int getgid_def(cchar *,gid_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETGID_INCLUDE */



/* groupent HEADER */
/* lang=C20 */

/* subroutines for simple GROUP object (from UNIX� library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-12, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GROUPENT_INCLUDE
#define	GROUPENT_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<grp.h>


#define	GROUPENT	struct group
#define	CGROUPENT	const struct group


#ifdef	__cplusplus
extern "C" {
#endif

extern int groupent_load(GROUP *,char *,int,CGROUP *) noex ;
extern int groupent_parse(GROUP *,char *,int,cchar *,int) noex ;
extern int groupent_size(CGROUP *) noex ;
extern int groupent_format(CGROUP *,char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GROUPENT_INCLUDE */



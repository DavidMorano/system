/* uproguser SUPPPORT */
/* lang=C20 */

/* UNIX® username and possibly user-home-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPROGUSER_INCLUDE
#define	UPROGUSER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int uproguser_init() noex ;
extern int uproguser_nameset(cchar *,int,uid_t,int) noex ;
extern int uproguser_nameget(char *,int,uid_t) noex ;
extern int uproguser_fini() noex ;

EXTERNC_end


#endif /* UPROGUSER_INCLUDE */



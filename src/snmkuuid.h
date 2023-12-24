/* snuuid */
/* lang=C20 */

/* string-UUID (String-UUID) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNUUID_INCLUDE
#define	SNUUID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkuuid.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int snuuid(char *,int,MKUUID *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNUUID_INCLUDE */



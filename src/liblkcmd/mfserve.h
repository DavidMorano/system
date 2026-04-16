/* mfserve HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* support for loadable modules for MFSERVE */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MFSERVE_INCLUDE
#define	MFSERVE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


/* object defines */

#define	MFSERVE_MOD		struct mfserve_mod
#define	MFSERVE_INFO		struct mfserve_info


struct mfserve_mod {
	cchar		*objname ;
	int		objsize ;
	int		endmark ;
} ;

struct mfserve_info {
	int		objsize ;
	int		(*start)(void *,cchar *,void *,cchar **,cchar **) ;
	int		(*check)(void *) ;
	int		(*abort)(void *) ;
	int		(*finish)(void *) ;
} ;


#endif /* MFSERVE_INCLUDE */



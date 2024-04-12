/* usysauxinfo_darwin HEADER */
/* lang=C20 */

/* version %I% last-modified %G% */
/* Apple-Darwin operating system support */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.


*******************************************************************************/

#ifndef	USYSAUXINFO_DARWIN_INCLUDE
#define	USYSAUXINFO_DARWIN_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */

/* USYSAUXINFO_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<signal.h>
#include	<pthread.h>
#include	<time.h>
#include	<clanguage.h>

#include	<sysauxinfonames.h>


/* SYSAUXINFO begin */
#if	defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0)

/* only define what this system has */
#define	SAI_ARCHITECTURE	sysauxinfo_architecture
#define	SAI_PLATFORM		sysauxinfo_platform
#define	SAI_HWPROVIDER		sysauxinfo_hwprovider

#endif /* defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0) */
/* SYSAUXINFO end */


/* always define the following -- even if you do not have it */
#ifdef	__cplusplus
extern "C" {
#endif

extern int sysauxinfo(char *,int,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSAUXINFO_DARWIN finish */


#endif /* USYSAUXINFO_DARWIN_INCLUDE */



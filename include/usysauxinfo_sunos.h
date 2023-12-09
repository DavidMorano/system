/* usysauxinfo_sunos INCLUDE */
/* lang=C20 */

/* version %I% last-modified %G% */
/* Sun-Solaris® (SunOS) operating system support */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types and preprocessor symbols
	that may not be provided.


*******************************************************************************/

#ifndef	USYSAUXINFO_SUNOS_INCLUDE
#define	USYSAUXINFO_SUNOS_INCLUDE


#include	<envstandards.h>

/* USYSAUXINFO_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

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
#define SAI_HWSERIAL		sysauxinfo_hwserial
#define SAI_SRPCDOMAIN		sysauxinfo_srpcdomain

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


#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYSAUXINFO_SUNOS end */


#endif /* USYSAUXINFO_SUNOS_INCLUDE */



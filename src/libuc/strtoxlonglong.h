/* strtoxlonglong HEADER */
/* lang=C20 */

/* numeric-character conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOXLONGLONG_INCLUDE
#define	STRTOXLONGLONG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdintx.h>


#ifdef	__cplusplus

extern "C" {
    extern longlong strtolonglong(const char *,char **,int) noex ;
    extern ulonglong strtoulonglong(const char *,char **,int) noex ;
}

#else /* __cplusplus */

extern longlong strtolonglong(const char *,char **,int) noex ;
extern ulonglong strtoulonglong(const char *,char **,int) noex ;

#endif /* __cplusplus */


#endif /* STRTOXLONGLONG_INCLUDE */



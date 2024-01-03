/* defproc HEADER */
/* lang=C20 */

/* get the Default-Something */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEFPROC_INCLUDE
#define	DEFPROC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecstr.h>
#include	<expcook.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int defproc(vecstr *dlp,cchar **envv,EXPCOOK *,cchar *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* DEFPROC_INCLUDE */



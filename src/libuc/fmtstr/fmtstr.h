/* fmtstr HEADER */
/* lang=C20 */

/* header file for the FMTSTR subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FMTSTR_INCLUDE
#define	FMTSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>
#include	<utypedefs.h>
#include	<clanguag.h>


#define	FMTSTR_OCLEAN	(1<<0)		/* clean data */
#define	FMTSTR_ONOOVERR	(1<<1)		/* do *not* return error on overflow */


#ifdef	__cplusplus
extern "C" {
#endif

extern int	fmtstr(char *,int,int,cchar *,va_list) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* FMTSTR_INCLUDE */



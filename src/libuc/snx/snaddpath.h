/* snaddpath HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* append a c-string to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	snaddpath

	Description:
	These subroutines are used to concatenate a path (as a
	c-string) or a counted path (as a counted c-string) to an
	existing counted path (as a counted c-string).

	Synopsis:
	int snaddpath(char *pbuf,int plen,int pl,cchar *sp) noex
	int snaddpath(char *pbuf,int plen,int pl,cchar *sp,int sl) noex

******************************************************************************/

#ifndef	SNADDPATH_INCLUDE
#define	SNADDPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int snaddpath(char *,int,int,cchar *) noex ;
EXTERNC_end

#ifdef	__cplusplus
extern int snaddpath(char *,int,int,cchar *,int) noex ;
#endif /* __cplusplus */


#endif /* SNADDPATH_INCLUDE */



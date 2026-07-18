/* getaf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® Address-Family (AF) support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getaf

  	Description:
	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	GETAF_INCLUDE
#define	GETAF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getaf		(cchar *,int) noex ;
extern int	getaflen	(int) noex ;
extern cchar	*strafname(int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int getaf(cchar *sp) noex {
	return getaf(sp,-1) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* GETAF_INCLUDE */



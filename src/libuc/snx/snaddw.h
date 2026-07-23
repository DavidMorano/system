/* snaddw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* append a counted c-string to existing counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	snaddw

	Description:
	This subroutine adds a c-string to the end of an existing
	string.  Return the incremental amount added.

	Notes:
	This subroutine sbould be the same as (the older)
	|storebuf_strw(3uc)|.

******************************************************************************/

#ifndef	SNADDW_INCLUDE
#define	SNADDW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int snaddw(char *,int,int,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline int snaddw(char *dbuf,int dlen,int dl,cchar *sp) noex {
	return snaddw(dbuf,dlen,dl,sp,-1) ;
}

#endif /* __cplusplus */


#endif /* SNADDW_INCLUDE */



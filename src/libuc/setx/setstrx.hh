/* setstrx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set-string container (Set-String) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	setstrx

	Description:
	This object implements a set of strings.  This are C-styled
	strings (of course, and as it should be).

*******************************************************************************/

#ifndef	SETSTRX_INCLUDE
#define	SETSTRX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


struct setstrx {
	setstrx &operator = (const setstrx &) = delete ;
	virtual int add(cchar *,int = -1) noex = 0 ;
	virtual int del(cchar *,int = -1) noex = 0 ;
	int loadfile(int,cchar *) noex ;
} ; /* end struct (setstrx) */


#endif /* __cplusplus */
#endif /* SETSTRX_INCLUDE */



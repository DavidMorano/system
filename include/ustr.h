/* ustr INCLUDE */
/* lang=C20 */

/* UNIX® system STREAMS message interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the subroutine declarations for the
	UNIX® system STREAMS message interface.

*******************************************************************************/

#ifndef	USTR_INCLUDE
#define	USTR_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	<usys.h>
#include	<ustropts.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int u_strmsgget(int,STRBUF *,STRBUF *,int *) noex ;
extern int u_strmsggetp(int,STRBUF *,STRBUF *,int *,int *) noex ;
extern int u_strmsgput(int,STRBUF *,STRBUF *,int) noex ;
extern int u_strmsgputp(int,STRBUF *,STRBUF *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* USTR_INCLUDE */



/* svcent HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* subroutines for simple SVCFILE_ENT object management */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-13, David A­D­ Morano
	This was split out of the HOMEPAGE program (where it was originally
	local).

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	SVCENT_INCLUDE
#define	SVCENT_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<svcfile.h>		/* <- the money shot */


#define	SVCENT	svcfile_ent


#ifdef	__cplusplus
struct svcent : svcfile_ent {
	int islib(cchar **) noex ;
	int geval(cchar *,cchar **) noex ;
	int getdeval(cchar *,cchar **) noex ;
} ; /* end struct (svcent) */
#else /* __cplusplus */
typedef	SVCENT		svcent ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	svcent_islib(svcent *,cchar **) noex ;
extern int	svcent_getval(svcent *,cchar *,cchar **) noex ;
extern int	svcent_getdeval(svcent *,cchar *,cchar **) noex ;

EXTERNC_end


#endif /* SVCENT_INCLUDE */



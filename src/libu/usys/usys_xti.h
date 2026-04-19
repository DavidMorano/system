/* usys_xti HEADER (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® XTI support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_xti

	Description:
	This file contains some support for the X/Open Transport
	Interface (XTI) related operations.

*******************************************************************************/

#ifndef	USTSXTI_INCLUDE
#define	USTSXTI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)

/* congratulations: you have it! */

#else /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */


#include	"usys_xtibase.h"
#include	"usys_xtierror.h"
#include	"usys_xtistructs.h"

extern int t_errno ;

EXTERNC_begin

extern voidp	xti_alloc	(int,int,int) noex ;
extern int	xti_open	(cchar *,int,struct t_info *) noex ;
extern int	xti_bind	(int,struct t_bind *,struct t_bind *) noex ;
extern int	xti_listen	(int,struct t_call *) noex ;
extern int	xti_connect	(int,struct t_call *,struct t_call *) noex ;
extern int	xti_accept	(int,int,const struct t_call *) noex ;
extern int	xti_look	(int) noex ;
extern int	xti_sync	(int) noex ;
extern int	xti_close	(int) noex ;
extern int	xti_free	(void *,int) noex ;

EXTERNC_end

#endif /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */


#endif /* USTSXTI_INCLUDE */



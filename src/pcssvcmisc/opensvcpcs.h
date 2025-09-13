/* opensvcpcs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	opensvcpcs

	Description:
	This code module contains some PCS-related utility subroutines.

*******************************************************************************/

#ifndef	OPENSVCPCS_INCLUDE
#define	OPENSVCPCS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bits.h>
#include	<filer.h>
#include	<pcsns.h>

#include	"defs.h"


#define	SUBPCS		struct subpcs_head
#define	SUBPCS_FL	struct subpcs_flags


/* local structures */

struct subpcs_flags {
	uint		full:1 ;
} ;

struct subpcs_head {
	mainv		envv ;
	cchar		*pr ;
	pcsns		ns ;
	SUBPCS_FL	fl ;
	int		w ;
} ;

typedef	SUBPCS		subpcs ;
typedef	SUBPCS_FL	subpcs_fl ;

EXTERNC_begin

extern int subpcs_start(subpcs *,cchar *,cchar **,int) noex ;
extern int subpcs_finish(subpcs *) noex ;
extern int subpcs_af(subpcs *,filer *,cchar *) noex ;
extern int subpcs_def(subpcs *,filer *) noex ;
extern int subpcs_all(subpcs *,filer *) noex ;
extern int subpcs_args(subpcs *,filer *,ARGINFO *,bits *,cchar *) noex ;
extern int subpcs_users(subpcs *,filer *,cchar *,int) noex ;
extern int subpcs_user(subpcs *,filer *,cchar *,int) noex ;

EXTERNC_end


#endif /* OPENSVCPCS_INCLUDE */



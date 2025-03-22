/* callback HEADER */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* surboutine call-back interface */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	callback

	Description:
	This file provides the definition of the CALLBACK object, and its
	method declarations.

*******************************************************************************/

#ifndef	CALLBACK_INCLUDE
#define	CALLBACK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<psem.h>


#define	CALLBACK	struct callback_head


EXTERNC_begin

typedef int (*callback_f)(void *objp,int timid,int arg) noex ;

EXTERNC_end

struct callback_head {
	void		*objp ;		/* object pointer */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	callback_f	memf ;		/* function subroutine (C-linkage) */
	int		timid ;		/* timer-ID */
	int		arg ;		/* function argument */
} ;

typedef CALLBACK	callback ;

EXTERNC_begin

extern int callback_load(callback *,void *,psem *,callback_f,int) noex ;

EXTERNC_end


#endif /* CALLBACK_INCLUDE */



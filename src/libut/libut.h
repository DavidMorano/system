/* uxti HEADER (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uxti

	Description:
	This is the nehanced (cleaned up) API for the
	X/Open Transport Interface.

*******************************************************************************/

#ifndef	UXTI_INCLUDE
#define	UXTI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<snxti.h>

EXTERNC_begin

extern int	ut_open(cchar *,int,UTINFO *) noex ;
extern int	ut_bind(int,UTBIND *,UTBIND *) noex ;
extern int	ut_listen(int,UTCALL *) noex ;
extern int	ut_connect(int,UTCALL *,UTCALL *) noex ;
extern int	ut_accept(int,int,const UTCALL *) noex ;
extern int	ut_look(int) noex ;
extern int	ut_sync(int) noex ;
extern int	ut_close(int) noex ;

extern int	ut_alloc(int,int,int,void **) noex ;
extern int	ut_free(void *,int) noex ;

EXTERNC_end


#endif /* UXTI_INCLUDE */



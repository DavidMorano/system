/* ucopeninfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucopeninfo

	Description:
	Additional support for the UNIX® OPEN facility.

*******************************************************************************/

#ifndef	UCOPENINFO_INCLUDE
#define	UCOPENINFO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX |mode_t| */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	UCOPENINFO	struct ucopeninfo_head


struct ucopeninfo_head {
	mainv		envv ;
	cchar		*fname ;	/* caller argument */
	int		clinks ;
	int		oflags ;
	int		to ;
	int		opts ;
	int		pf ;		/* protocol-family */
	int		pt ;		/* protocol-type */
	int		proto ;		/* protocol */
	mode_t		operms ;
} ; /* end struct (ucopeninfo) */

typedef UCOPENINFO	ucopeninfo ;

EXTERNC_begin

extern int	uc_openex	(cchar *,int,mode_t,int,int) noex ;
extern int	uc_openinfo	(ucopeninfo *) noex ;
extern int	uc_openuserinfo	(ucopeninfo *) noex ;

EXTERNC_end


#endif /* UCOPENINFO_INCLUDE */



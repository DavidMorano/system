/* ucgetxx HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucgeter

	Description:
	This is a (error timer) support object for use by the UCGET
	sub-group of the U library.

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#ifndef	UCGETXX_INCLUDE
#define	UCGETXX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>


namespace ucget {
    struct ucgeter {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	errtimer	to_nosr		= utimeout[uto_nosr] ;
	errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	errtimer	to_mfile	= utimeout[uto_mfile] ;
	errtimer	to_nfile	= utimeout[uto_nfile] ;
	errtimer	to_nolck	= utimeout[uto_nolck] ;
	errtimer	to_nospc	= utimeout[uto_nospc] ;
	errtimer	to_dquot	= utimeout[uto_dquot] ;
	errtimer	to_io		= utimeout[uto_io] ;
	bool		fexit = false ;
	int operator () (int) noex ;
    } ; /* end struct (ucgeter) */
}


#endif /* __cplusplus */
#endif /* UCGETXX_INCLUDE */



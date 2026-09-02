/* cmpord HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* memory order definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	memorg

	Description:
	This file defines some preprocessor variables that
	specify some meory orderings.

*******************************************************************************/

#ifndef	CMPORD_INCLUDE
#define	CMPORD_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<compare>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


/* C-C++ memory order */
#ifndef	cmpord_strong
#define	cmpord_strong	std::strong_ordering
#define	cmpord_partial	std::partial_ordering
#define	cmpord_weak	std::weak_ordering
#endif /* cmpord_relaxed */


#endif /* __cplusplus  (C++ only) */
#endif /* CMPORD_INCLUDE */



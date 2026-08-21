/* memord HEADER */
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

#ifndef	MEMORD_INCLUDE
#define	MEMORD_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<atomic>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


/* C-C++ memory order */
#ifndef	memord_relaxed
#define	memord_relaxed	std::memory_order_relaxed
#define	memord_consume	std::memory_order_consume /* depracted c++26 */
#define	memord_acquire	std::memory_order_acquire
#define	memord_release	std::memory_order_release
#define	memord_acqrel	std::memory_order_acq_rel
#define	memord_seq	std::memory_order_seq_cst
#endif /* memord_relaxed */


#endif /* __cplusplus  (C++ only) */
#endif /* MEMORD_INCLUDE */



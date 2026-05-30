/* clanguage_assert HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEASSERT_INCLUDE
#define	CLANGUAGEASSERT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifdef	__cplusplus
#ifndef	assert_contract
#define	assert_contract		contract_assert
#define	assert_static		static_assert
#endif
#endif /* __cplusplus */


#endif /* CLANGUAGEASSERT_INCLUDE */



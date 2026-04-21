/* concharp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* concept to restrict to various pointers to 'char'*/
/* version %I% last-modified %G% */


/* revision history:

	= 2020-01-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2020 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	concharp

	Description:
	This is a concept that restricts to various pointers to
	the 'char' type.

*******************************************************************************/

#ifndef	CONCHARP_INCLUDE
#define	CONCHARP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<type_traits>
#include	<concepts>
#include	<clanguage.h>


template <typename T>
concept concharp =
    /* Case 1: Is it a pointer to (possibly const) char? */
    (std::is_pointer_v<T> &&
     std::same_as<std::remove_cv_t<std::remove_pointer_t<T>>, char>) ||
    /* Case 2: Is it an array of (possibly const) char? */
    (std::is_array_v<T> &&
     std::same_as<std::remove_cv_t<std::remove_extent_t<T>>, char>) ;


#endif /* CONCHARP_INCLUDE */



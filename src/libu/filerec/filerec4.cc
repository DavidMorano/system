/* filerec4 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filerec

	Description:
	This object implements a set (an un-ordered set) with a key
	consisting of device and inode numbers.

	Symopsis:
	int filerec_start(int n = 0) noex

	Arguments:
	n		suggested starting length

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<utility>		/* |pair(3c++)| */
#include	<unordered_set>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<localmisc.h>

module filerec ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines (methods) */

bool filerec_iter::operator != (const filerec_iter &oit) noex {
    	return (baseit != oit.baseit) ;
}

bool filerec_iter::operator == (const filerec_iter &oit) noex {
    	return (baseit == oit.baseit) ;
}

filerec_ent filerec_iter::operator * () noex {
    	return *baseit ;
}

filerec_iter filerec_iter::operator + (int n) const noex {
    	filerec_iter	it(*this) ;
	for (int i = 0 ; i < n ; i += 1) {
	    ++it.baseit ;
	}
	return it ;
}

filerec_iter &filerec_iter::operator += (int) noex {
    	return *this ;
}

filerec_iter filerec_iter::operator ++ () noex /* pre */ {
    	++baseit ;
	return *this ;
}

filerec_iter filerec_iter::operator ++ (int) noex /* post */ {
    	filerec_iter	it(*this) ;
    	++baseit ;
	return it ;
}



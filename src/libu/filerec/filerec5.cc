/* filerec5 SUPPORT (Module-Implementation-Unit) */
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
	filerec (File-Record)

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
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<strnul.hh>
#include	<localmisc.h>

module filerec ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines (methods) */

int filerec::add(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    strnul s(sp,sl) ;
	    if (ustat sb ; (rs = u_stat(s,&sb)) >= 0) {
		rs = checkin(&sb,s) ;
		f = rs ;
	    } /* end if (u_stat) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? f : rs ;
} /* end method (filerec::add) */



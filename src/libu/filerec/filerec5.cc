/* filerec5 SUPPORT */
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
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
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

int filerec::add(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    {
		strnul s(sp,sl) ;
	        if (ustat sb ; (rs = u_stat(s,&sb)) >= 0) {
		    rs = checkin(&sb,s) ;
		    f = rs ;
		} /* end if (u_stat) */
	    } /* end block */
	} /* end if (valid) */
	return (rs >= 0) ? f : rs ;
} /* end method (filerec::add) */



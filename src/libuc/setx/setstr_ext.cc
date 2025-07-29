/* setstr_ext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extra functions for string vector objects */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	setstr_ext

	Description:
	Below are the so-called extended function for the VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setstr.h"
#include	"setstrx.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct setstrer : setstrx {
        setstr		*op ;
	setstrer() = delete ;
	setstrer(setstr *p) noex : op(p) { } ;
	int add(cchar *,int = -1)			noex override final ;
	int del(cchar *,int = -1)			noex override final ;
    } ; /* end struct (setstrer) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int setstr_loadfile(setstr *op,int fu,cchar *fname) noex {
    	setstrer	vo(op) ;
	return vo.loadfile(fu,fname) ;
}
/* end subroutine (setstr_loadfile) */


/* local subroutines */

int setstrer::add(cchar *sp,int sl) noex {
    	return setstr_add(op,sp,sl) ;
}

int setstrer::del(cchar *sp,int sl) noex {
	return setstr_del(op,sp,sl) ;
}



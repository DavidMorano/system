/* setostr_ext SUPPORT */
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
	setostr_ext

	Description:
	Below are the so-called extended function for the VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setostr.h"
#include	"setstrx.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct setostrer : setstrx {
        setostr		*op ;
	setostrer() = delete ;
	setostrer(setostr *p) noex : op(p) { } ;
	int add(cchar *,int = -1)			noex override final ;
	int del(cchar *,int = -1)			noex override final ;
    } ; /* end struct (setostrer) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int setostr_loadfile(setostr *op,int fu,cchar *fname) noex {
    	setostrer	vo(op) ;
	return vo.loadfile(fu,fname) ;
}
/* end subroutine (setostr_loadfile) */


/* local subroutines */

int setostrer::add(cchar *sp,int sl) noex {
    	return setostr_add(op,sp,sl) ;
}

int setostrer::del(cchar *sp,int sl) noex {
	return setostr_del(op,sp,sl) ;
}



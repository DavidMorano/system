/* field_obj SUPPORT */
/* lang=C++20 */

/* subroutine to parse a line into fields */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­ D­ Morano
	This code module was originally written in VAX assembly
	(back around 1980 or so). It was then written into C-language
	perhaps around 1983 (at AT&T Bell Laboratories).  This
	(below) is a new incarnation based on that older C-language
	code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object support routines for the FIELD object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"field.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* local inline-subroutines */


/* exported variables */


/* exported subroutines */

int field::start(cchar *sp,int sl) noex {
	return field_start(this,sp,sl) ;
}

int field::get(cchar *tp,cchar **rpp) noex {
	return field_get(this,tp,rpp) ;
}

int field::getterm(cchar *tp,cchar **rpp) noex {
	return field_getterm(this,tp,rpp) ;
}

int field::sharg(cchar *tp,char *rbuf,int rlen) noex {
	return field_sharg(this,tp,rbuf,rlen) ;
}

int field::remaining(cchar **rpp) noex {
	return field_remaining(this,rpp) ;
}

void field::dtor() noex {
	cint	rs = field_finish(this) ;
	if (rs < 0) {
	    ulogerror("field",rs,"fini-finish") ;
	}
}

field_co::operator int () noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case fieldmem_finish:
	    rs = field_finish(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (field_co::operator) */



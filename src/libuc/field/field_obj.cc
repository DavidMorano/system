/* field_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to parse a line into fields */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was originally written in C language modeled
	(roughly) from a prior VAX assembly language version (written
	circa 1980 perhaps).  This is why this looks so "ugly"!
	This code comes from stuff dated back to almost the pre-dawn
	era of modern computer languages!  I wrote the original VAX
	assembly stuff also.  This code below was then written into
	C-language perhaps around 1983 (at AT&T Bell Laboratories).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object (continued):
	field

	Description:
	Object support routines for the FIELD object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"field.h"


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

int field::start(cchar *sp,int sl) noex {
	return field_start(this,sp,sl) ;
} /* end method */

int field::get(cchar *tp,cchar **rpp) noex {
	return field_get(this,tp,rpp) ;
} /* end method */

int field::getterm(cchar *tp,cchar **rpp) noex {
	return field_getterm(this,tp,rpp) ;
} /* end method */

int field::word(cchar *tp,cchar **fpp) noex {
	return field_word(this,tp,fpp) ;
} /* end method */

int field::wordphrase(cchar *tp,char *rbuf,int rlen) noex {
	return field_wordphrase(this,tp,rbuf,rlen) ;
} /* end method */

int field::sharg(cchar *tp,char *rbuf,int rlen) noex {
	return field_sharg(this,tp,rbuf,rlen) ;
} /* end method */

int field::srvarg(cchar *tp,char *rbuf,int rlen) noex {
	return field_srvarg(this,tp,rbuf,rlen) ;
} /* end method */

int field::remaining(cchar **rpp) noex {
	return field_remaining(this,rpp) ;
} /* end method */

void field::dtor() noex {
	if (cint rs = field_finish(this) ; rs < 0) {
	    ulogerror("field",rs,"fini-finish") ;
	}
} /* end method */

field_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case fieldmem_rem:
	        rs = field_remaining(op,nullptr) ;
	        break ;
	    case fieldmem_finish:
	        rs = field_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (field_co::operator) */



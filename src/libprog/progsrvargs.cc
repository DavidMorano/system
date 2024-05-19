/* progsrvargs SUPPORT */
/* lang=C++20 */

/* process server file program arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to parse the SERVER program arguments
	from an expanded (substituted) server file entry.  Basically,
	we just "field-SHELL" out arguments and put them into the
	supplied vector-string object.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<terminit.hh>
#include	<field.h>
#include	<fieldterms.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */


/* external subroutines */


/* externals variables */


/* forward references */


/* local structures */

constexpr terminit	sa("\t ") ;


/* exported variables */


/* exported subroutines */

int progsrvargs(PROGINFO *pip,vecstr *alp,cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (pip && abuf) {
	    cint	alen = strlen(abuf) ;
	    rs = SR_INVALID ;
	    if (abuf[0] != '\0') {
	        cint	flen = alen ;
	        char	*fbuf ;
		if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            field	fsb ;
	            if ((rs = field_start(&fsb,abuf,alen)) >= 0) {
	                int	fl ;
			cchar	*t = sa.terms ;
	                while ((fl = field_sharg(&fsb,t,fbuf,flen)) >= 0) {
	                    c += 1 ;
	                    rs = vecstr_add(alp,fbuf,fl) ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = field_finish(&fsb) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progsrvargs) */



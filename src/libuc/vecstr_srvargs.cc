/* vecstr_srvargs */
/* lang=C20 */

/* process server file program arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

	= 2017-11-30, David A­D­ Morano
	This code has been borrowed from the above.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to parse the server program arguments
	from an expanded (substituted) service file entry. Basically,
	we just "field-SHELL" out arguments and put them into the
	supplied vector-string object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<field.h>
#include	<localmisc.h>

#include	"vecstr.h"


/* local defines */


/* external subroutines */


/* externals variables */


/* forward references */


/* local structures */


/* exported subroutines */

int vecstr_srvargs(vecstr *alp,cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if ((abuf != NULL) && (abuf[0] != '\0')) {
	    field	fsb ;
	    cint	alen = strlen(abuf) ;
	    char	terms[32] ;
	    fieldterms(terms,0," \t") ;
	    if ((rs = field_start(&fsb,abuf,alen)) >= 0) {
	        cint	flen = alen ;
	        char	*fbuf{} ;
		if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            int		fl ;
	            while ((fl = field_sharg(&fsb,terms,fbuf,flen)) >= 0) {
	                c += 1 ;
	                rs = vecstr_add(alp,fbuf,fl) ;
	                if (rs < 0) break ;
	            } /* end while */
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;;
		} /* end if (m-a) */
	        rs1 = field_finish(&fsb) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	} /* end if (non-null non-zero) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_srvargs) */



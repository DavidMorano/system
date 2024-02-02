/* getpjid_name SUPPORT */
/* lang=C++20 */

/* get the project-ID for a given project-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This was written to collect this code into one subroutine.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	NameL
	getpjid_name

	Description:
	This subroutine returns a project-ID for a specified
	program-name.

	Synopsis:
	int getpjid_name(cchar *np,int nl) noex

	Arguments:
	np		name of project to look up 
	nl		length of name (in bytes)

	Returns:
	>=0		PJID of given project name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<nulstr.h>
#include	<getax.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int getpjid_name(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		pjid = 0 ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        nulstr		n ;
	        cchar		*name{} ;
	        if ((rs = nulstr_start(&n,sp,sl,&name)) >= 0) {
	            if ((rs = getbufsize(getbufsize_pj)) >= 0) {
	                PROJECT	pj ;
	                cint	pjlen = rs ;
	                char	*pjbuf{} ;
	                if ((rs = uc_malloc((pjlen+1),&pjbuf)) >= 0) {
		            {
	                        rs = getpj_name(&pj,pjbuf,pjlen,name) ;
	                        pjid = pj.pj_projid ;
		            }
	                    rs1 = uc_free(pjbuf) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (memory-allocation) */
	            } /* end if (getbufsize) */
	            rs1 = nulstr_finish(&n) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pjid : rs ;
}
/* end subroutine (getpjid_name) */

int getpjid_proj(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        if (hasalldig(sp,sl)) {
	            if (int v{} ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
	            rs = getpjid_name(sp,sl) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpjid_proj) */



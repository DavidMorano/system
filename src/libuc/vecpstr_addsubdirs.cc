/* vecpstr_addsubdirs */
/* lang=C++20 */

/* find and load UNIX® directories under a given root */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine load all directories and sub-directories
	of a given root in the file-system into a vecpstr (container)
	object.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstring>
#include	<usystem.h>
#include	<fsdirtree.h>
#include	<localmisc.h>

#include	"vecpstr.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int vecpstr_addsubdirs(vecpstr *op,cchar *newsdname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op && newsdname) {
	    char	*fbuf{} ;
	    if ((rs = libmallocxx_mp(&fbuf)) >= 0) {
	        FSDIRTREE	dir ;
		cint		flen = rs ;
	        cint		fo = (FSDIRTREE_MFOLLOW | FSDIRTREE_MDIR) ;
	        if ((rs = fsdirtree_open(&dir,newsdname,fo)) >= 0) {
	            USTAT	sb ;
	            int		fl ;
	            while ((rs = fsdirtree_read(&dir,&sb,fbuf,flen)) > 0) {
	                fl = rs ;
	                if (fbuf[0] != '.') {
	                    c += 1 ;
	                    rs = vecpstr_add(op,fbuf,fl) ;
	                }
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = fsdirtree_close(&dir) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (fsdirtree) */
	        if (rs >= 0) {
	            vecpstr_sort(op,nullptr) ;
	        }
		rs1 = uc_libfree(fbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_addsubdirs) */

int vecpstr_loaddirs(vecpstr *op,cchar *newsdname) noex {
	return vecpstr_addsubdirs(op,newsdname) ;
}
/* end subroutine (vecpstr_loaddirs) */



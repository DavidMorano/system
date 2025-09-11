/* vecstrx_addsubdirs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find and load UNIX® directories under a given root */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecstrx_addsubdirs

	Description:
	This subroutine load all directories and sub-directories
	of a given root in the file-system into a vecstrx (container)
	object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<fsdirtree.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

#pragma		GCC dependency		"mod/ulibvals.ccm"

import ulibvals ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static int		maxpathlen = ulibval.maxpathlen ;


/* exported variables */


/* exported subroutines */

int vecstrx::addsubdirs(cchar *newsdname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (newsdname) {
	    if ((rs = maxpathlen) >= 0) {
		cint	flen = rs ;
	        if (char *fbuf ; (rs = libmem.mall((flen +1),&fbuf)) >= 0) {
	            fsdirtree	dir ;
	            cint	fo = (FSDIRTREE_MFOLLOW | FSDIRTREE_MDIR) ;
	            if ((rs = fsdirtree_open(&dir,newsdname,fo)) >= 0) {
	                ustat sb ;
	                while ((rs = fsdirtree_read(&dir,&sb,fbuf,flen)) > 0) {
	                    if (fbuf[0] != '.') {
	                        c += 1 ;
	                        rs = add(fbuf,rs) ;
	                    }
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = fsdirtree_close(&dir) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (fsdirtree) */
	            if (rs >= 0) {
	                sort(nullptr) ;
	            }
		    rs1 = libmem.free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addsubdirs) */



/* urmdirs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove directories recursively */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_rmdirs

	Description:
	This subroutine deletes (removes) a directory tree.  I
	recursively delete directories as they are encountered.
	Yhis results in a recursive depth-dirst deletion solution.

	Synopsis:
	int u_rmdirs(cchar *dname) noex

	Arguments:
	dname		directory name (or regular file) to unlink

	Returns:
	>=0		number of items removed
	<0		error (system-return)

	Notes: 
	= Stack space:
	Yes, now-a-days (since everything has gone multi-threaded
	-- finally) we are more sensitive about our use of stack
	space.  In our algorithm below, we only allocate (on the
	stack) a single path buffer that is used throughout the
	recursive deletion strategy.  Other than this, stack space
	is (indeed) used for each level of the directory structure
	we encunter for fairly regular automatic stack-frame
	variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usyscalls.h>
#include	<usupport.h>		/* |hasnotdots(3u)| */
#include	<umisc.h>
#include	<posixdirent.hh>
#include	<localmisc.h>

#include	"urmdirs.h"

import libutil ;
import umisc ;				/* |mknpath(3u)| */
import ureserve ;			/* |vecstr(3u)| */
import ulibvals ;			/* |max{x}| */

/* local defines */


/* imported namespaces */

using libu::umemallocstrw ;		/* subroutine */
using libu::umemalloc ;			/* subroutine */
using libu::umemvalloc ;		/* subroutine */
using libu::umemcalloc ;		/* subroutine */
using libu::umemrealloc ;		/* subroutine */
using libu::umemfree ;			/* subroutine */
using libu::umemrsfree ;		/* subroutine */
using libu::hasnotdots ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mgr {
	cchar	*tardname ;
	char	*pbuf ;
	char	*nbuf ;
	int	plen ;
	int	nlen ;
	mgr(cchar *td) noex : tardname(td) { } ;
	operator int () noex ;
	int remover(int) noex ;
	int removedents(int) noex ;
    } ; /* end struct (mgr) */
    struct vars {
	int	maxname ;
	int	maxpath ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct dirents : vecstr {
	int load(char *,int,cchar *) noex ;
    } ; /* end struct (dirents) */
} /* end namespace */


/* forward references */


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int u_rmdirs(cchar *tardname) noex {
	int		rs = SR_FAULT ;
	if (tardname) {
	    rs = SR_INVALID ;
	    if (tardname[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    mgr mo(tardname) ;
		    rs = mo ;
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_rmdirs) */


/* local subroutines */

mgr::operator int () noex {
	cint		sz = ((var.maxpath + 1) + (var.maxname + 1)) ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	plen = var.maxpath ;
	nlen = var.maxname ;
	if (char *a ; (rs = umemalloc(sz,&a)) >= 0) {
	    pbuf = (a + 0) ;
	    nbuf = (a + (plen + 1)) ;
	    if ((rs = mknpath(pbuf,plen,tardname)) >= 0) {
		rs = remover(rs) ;
		c = rs ;
	    }
	    rs1 = umemfree(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (mgr::operator) */

int mgr::remover(int pl) noex {
	int		rs ;
	int		c = 0 ;
	if (ustat sb ; (rs = u_lstat(pbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		if ((rs = removedents(pl)) >= 0) {
		    c = (rs + 1) ;
	            rs = u_rmdir(pbuf) ;
		}
	    } else {
		rs = u_unlink(pbuf) ;
		c = 1 ;
	    }
	} /* end if (u_lstat) */
	return (rs >= 0) ? c : rs ;
} /* end method (mgr::remover) */

int mgr::removedents(int pl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (dirents names ; (rs = names.start) >= 0) {
	    if ((rs = names.load(nbuf,nlen,pbuf)) > 0) {
	        cchar	*sp ;
	        for (int i = 0 ; names.get(i,&sp) >= 0 ; i += 1) {
	            if (sp) {
	                if ((rs = pathnadd(pbuf,plen,pl,sp)) >= 0) {
			    rs = remover(rs) ;
			    c += rs ;
	                } /* end if (pathnadd) */
		    }
	            if (rs < 0) break ;
	        } /* end for */
		pbuf[pl] = '\0' ; /* restore */
	    } /* end if (vecstr_load) */
	    rs1 = names.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirents) */
	return (rs > 0) ? c : rs ;
} /* end method (mgr::removedents) */

int dirents::load(char *nbuf,int nlen,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (posixdirent d ; (rs = d.open(dname)) >= 0) {
	    for (dirent_t de ; (rs = d.read(&de,nbuf,nlen)) > 0 ; ) {
	        if ((nbuf[0] != '.') || hasnotdots(nbuf,rs)) {
	            c += 1 ;
	            rs = add(nbuf,rs) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = d.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (posixdirent) */
	return (rs >= 0) ? c : rs ;
} /* end method (dirents::load) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = ulibval.maxnamelen) >= 0) {
	    maxname = rs ;
	    if ((rs = ulibval.maxpathlen) >= 0) {
		maxpath = rs ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */



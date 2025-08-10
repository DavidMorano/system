/* removes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* removes (recursively as necessary) a directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	removes

	Description:
	This subroutine deletes (removes) a directory tree.  I
	recursively delete directories as they are encountered.
	Yhis results in a recursive depth-dirst deletion solution.

	Synopsis:
	int removes(cchar *dname) noex

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
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<vecpstr.h>
#include	<mkpathx.h>
#include	<mkx.h>
#include	<pathadd.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"removes.h"

import libutil ;

/* local defines */


/* imported namespaces */


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
    struct dirents : vecpstr {
	int load(char *,int,cchar *) noex ;
    } ; /* end struct (dirents) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int removes(cchar *tardname) noex {
	int		rs = SR_FAULT ;
	if (tardname) ylikely {
	    rs = SR_INVALID ;
	    if (tardname[0]) ylikely {
		mgr ro(tardname) ;
		rs = ro ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (removes) */


/* local subroutines */

mgr::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = malloc_mp(&pbuf)) >= 0) ylikely {
	    plen = rs ;
	    if ((rs = malloc_mn(&nbuf)) >= 0) ylikely {
		nlen = rs ;
	        if ((rs = mkpathexp(pbuf,tardname,-1)) > 0) {
		    rs = remover(rs) ;
		    c = rs ;
	        } else if (rs == 0) {
		    if ((rs = mkpath(pbuf,tardname)) >= 0) {
		        rs = remover(rs) ;
		        c = rs ;
		    }
	        } /* end if (mkpathexp) */
	        rs1 = uc_free(nbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (mgr:operator) */

int mgr::remover(int pl) noex {
	int		rs ;
	int		c = 0 ;
	if (ustat sb ; (rs = u_lstat(pbuf,&sb)) >= 0) ylikely {
	    if (S_ISDIR(sb.st_mode)) {
		if ((rs = removedents(pl)) >= 0) {
	            c = (rs + 1) ;
	            rs = u_rmdir(pbuf) ;
		}
	    } else {
		rs = u_unlink(pbuf) ;
		c = 1 ;
	    }
	} /* end if (lstat) */
	return (rs >= 0) ? c : rs ;
} /* end method (mgr::remover) */

int mgr::removedents(int pl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (dirents names ; (rs = names.start) >= 0) ylikely {
	    if ((rs = names.load(nbuf,nlen,pbuf)) > 0) ylikely {
	        cchar	*sp ;
	        for (int i = 0 ; names.get(i,&sp) >= 0 ; i += 1) {
	            if (sp) {
	                if ((rs = pathadd(pbuf,pl,sp)) >= 0) {
			    rs = remover(rs) ;
			    c += rs ;
	                } /* end if (pathadd) */
		    }
	            if (rs < 0) break ;
	        } /* end for */
		pbuf[pl] = '\0' ; /* restore */
	    } /* end if (dirents::load) */
	    rs1 = names.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (names) */
	return (rs > 0) ? c : rs ;
} /* end method (mgr::removedents) */

int dirents::load(char *nbuf,int nlen,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (fsdir d ; (rs = d.open(dname)) >= 0) ylikely {
	    for (fsdir_ent ds ; (rs = d.read(&ds,nbuf,nlen)) > 0 ; ) {
	        if ((nbuf[0] != '.') || hasNotDots(nbuf,rs)) ylikely {
	            c += 1 ;
	            rs = add(nbuf,rs) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = d.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fsdir) */
	return (rs >= 0) ? c : rs ;
} /* end method (dirents::load) */



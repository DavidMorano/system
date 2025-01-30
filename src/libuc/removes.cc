/* removes SUPPORT */
/* encoding=ISO8859-1 */
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
	This subroutine deletes (removes) a directory tree. We
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
	space. In our algorithm below, we only allocate (on the
	stack) a single path buffer that is used throughout the
	recursive deletion strategy. Other than this, stack space
	us (indeed) used for each level of the directory structure
	we encunter for fairly regular automatic stack-frame
	variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<vecpstr.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"rmdirs.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkexpandpath(char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	remover(char *,int) noex ;
static int	removedir(char *,int) noex ;
static int	loadnames(vecpstr *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int removes(cchar *tardname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tardname) {
	    rs = SR_INVALID ;
	    if (tardname[0]) {
	        char	*pbuf{} ;
		if ((rs = malloc_mp(&pbuf)) >= 0) {
	            if ((rs = mkexpandpath(pbuf,tardname,-1)) > 0) {
	                rs = remover(pbuf,rs) ;
	            } else if (rs == 0) {
	                if ((rs = mkpath1(pbuf,tardname)) >= 0) {
	                    rs = remover(pbuf,rs) ;
	                }
	            } /* end if (mkexpandpath) */
		    rs1 = uc_free(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (removes) */


/* local subroutines */

static int remover(char *pbuf,int plen) noex {
	USTAT		sb ;
	int		rs ;
	int		c = 0 ;
	if ((rs = u_lstat(pbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		rs = removedir(pbuf,plen) ;
		c = rs ;
	    } else {
		rs = u_unlink(pbuf) ;
		c = 1 ;
	    }
	} /* end if (lstat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (remover) */

static int removedir(char *pbuf,int plen) noex {
	vecpstr		names ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (plen < 0) plen = strlen(pbuf) ;
	if ((rs = vecpstr_start(&names,0,0,0)) >= 0) {
	    if ((rs = loadnames(&names,pbuf)) > 0) {
	        USTAT	sb ;
	        cchar	*sp ;
	        for (int i = 0 ; vecpstr_get(&names,i,&sp) >= 0 ; i += 1) {
	            if (sp) {
	                if ((rs = pathadd(pbuf,plen,sp)) >= 0) {
	                    cint	pl = rs ;
	                    if ((rs = uc_lstat(pbuf,&sb)) >= 0) {
	                        if (S_ISDIR(sb.st_mode)) {
	                            rs = removedir(pbuf,pl) ;
	                            c += rs ;
	                        } else {
	                            rs = u_unlink(pbuf) ;
	                            c += 1 ;
	                        }
	                    } /* end if (lstat) */
	                } /* end if (pathadd) */
		    }
	            if (rs < 0) break ;
	        } /* end for */
		pbuf[plen] = '\0' ; /* restore */
	    } /* end if (loadnames) */
	    if (rs >= 0) {
	        rs = u_rmdir(pbuf) ;
	        c += 1 ;
	    }
	    rs1 = vecpstr_finish(&names) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (names) */
	return (rs > 0) ? c : rs ;
}
/* end subroutine (removedir) */

static int loadnames(vecpstr *nlp,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*nbuf{} ;
	if ((rs = malloc_mn(&nbuf)) >= 0) {
	    fsdir	d ;
	    fsdir_ent	ds ;
	    cint	nlen = rs ;
	    if ((rs = fsdir_open(&d,dname)) >= 0) {
	        while ((rs = fsdir_read(&d,&ds,nbuf,nlen)) > 0) {
	            cint	sl = rs ;
	            cchar	*sp = ds.name ;
	            if ((sp[0] != '.') || hasNotDots(sp,sl)) {
	                c += 1 ;
	                rs = vecpstr_add(nlp,sp,sl) ;
	            }
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsdir_close(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadnames) */



/* tmpmailboxes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the TMP-MAIL (mailboxes) directory name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmpmailboxes

	Description:
	Find or create the TMP-MAIL (mailboxes) directory.

	Synopsis:
	int tmpmailboxes(char *rbuf,int rlen) noex

	Arguments:
	rbuf		buffer to hold result
	rlen		length of supplied result buffer

	Returns:
	>=0		length of result string (in result buffer)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getnodename.h>
#include	<mallocxx.h>
#include	<estrings.h>		/* |sncpy{x}(3uc)| */
#include	<mkpr.h>
#include	<permx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"tmpmailboxes.h"

import uconstants ;

/* local defines */

#define	PRNAME		"PCS" /* Personal Communication Services */

#ifndef	TMPMAILBOXES
#define	TMPMAILBOXES	"/var/tmp/mailboxes"
#endif

#ifndef	PC_CHOWNRES
#define	PC_CHOWNRES	_PC_CHOWN_RESTRICTED
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	deftmpdir(char *,int) noex ;
static int	chownpcs(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tmpmailboxes(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) {
	    cchar	*tmpmb = TMPMAILBOXES ;
	    rbuf[0] = '\0' ;
	    if (USTAT sb ; (rs = uc_stat(tmpmb,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
		    cint	am = (R_OK|W_OK|X_OK) ;
		    if ((rs = perm(tmpmb,-1,-1,nullptr,am)) >= 0) {
	                rs = sncpy(rbuf,rlen,tmpmb) ;
		        rl = rs ;
		    } else if (isNotAccess(rs)) {
	                rs = deftmpdir(rbuf,rlen) ;
		        rl = rs ;
		    }
	        } else {
	            rs = deftmpdir(rbuf,rlen) ;
		    rl = rs ;
	        }
	    } else if (isNotPresent(rs)) {
	        cmode	dm = (0777 | S_ISVTX) ;
	        if ((rs = uc_mkdir(tmpmb,dm)) >= 0) {
		    if ((rs = uc_minmod(tmpmb,dm)) >= 0) {
		        cint	n = PC_CHOWNRES ; /* chown-restricted */
		        rl = strlen(tmpmb) ;
	                if ((rs = uc_pathconf(tmpmb,n,nullptr)) == 0) {
			    if ((rs = chownpcs(tmpmb)) >= 0) {
			        rs = SR_OK ;
			    } else if (isNotPresent(rs)) {
			        rs = SR_OK ;
			    }
		        }
		    } else if (isNotAccess(rs)) {
	                rs = deftmpdir(rbuf,rlen) ;
	                rl = rs ;
		    }
	        } else if (isNotPresent(rs)) {
	            rs = deftmpdir(rbuf,rlen) ;
	            rl = rs ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (tmpmailboxes) */


/* local subroutine */

static int deftmpdir(char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	cchar		*tmpdir = sysword.w_tmpdir ;
	static cchar	*cp = getenv(varname.tmpdir) ;
	if (cp) {
	    if (USTAT sb ; (rs = uc_stat(cp,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
		    cint	am = (R_OK|W_OK|X_OK) ;
		    if ((rs = perm(cp,-1,-1,nullptr,am)) >= 0) {
	                rs = sncpy(rbuf,rlen,cp) ;
		    } else if (isNotAccess(rs)) {
	                rs = sncpy(rbuf,rlen,tmpdir) ;
		    }
		} else {
	            rs = sncpy(rbuf,rlen,tmpdir) ;
		}
	    } else if (isNotPresent(rs)) {
	        rs = sncpy(rbuf,rlen,tmpdir) ;
	    }
	} else {
	    rs = sncpy(rbuf,rlen,tmpdir) ;
	}
	return rs ;
}
/* end subroutine (deftmpdir) */

static int chownpcs(cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	if (char *dbuf{} ; (rs = malloc_hn(&dbuf)) >= 0) {
	    cint	dlen = rs ;
	    if ((rs = getinetdomain(dbuf,dlen)) >= 0) {
		if (char *prbuf{} ; (rs = malloc_mp(&prbuf)) >= 0) {
	             cint	prlen = rs ;
	    	     cchar	*prname = PRNAME ;
	             if ((rs = mkpr(prbuf,prlen,prname,dname)) >= 0) {
	 	         if (USTAT sb ; (rs = uc_stat(prbuf,&sb)) >= 0) {
		             const uid_t	uid_pcs = sb.st_uid ;
		             const gid_t	gid_pcs = sb.st_gid ;
		             rs = uc_chown(dname,uid_pcs,gid_pcs) ;
		        } /* end if (uc_stat) */
	            } /* end if (mkpr) */
	    	    rs1 = uc_free(prbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getinetdomain) */
	    rs1 = uc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (chownpcs) */



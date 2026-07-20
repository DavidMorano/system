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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC |getinetdomain(3uc)| */
#include	<estrings.h>		/* LIBUC |sncpy{x}(3uc)| */
#include	<mkpr.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"tmpmailboxes.h"

import libutil ;
import uconstants ;

/* local defines */

#define	PRNAME		"PCS" /* Personal Communication Services */

#ifndef	TMPMAILBOXES
#define	TMPMAILBOXES	"/var/tmp/mailboxes"
#endif

#ifndef	PC_CHOWNRES
#define	PC_CHOWNRES	_PC_CHOWN_RESTRICTED
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	deftmpdir(char *,int) noex ;
local int	chownpcs(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tmpmailboxes(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) ylikely {
	    cchar	*tmpmb = TMPMAILBOXES ;
	    rbuf[0] = '\0' ;
	    if (ustat sb ; (rs = uc_stat(tmpmb,&sb)) >= 0) ylikely {
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
		        rl = lenstr(tmpmb) ;
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
} /* end subroutine (tmpmailboxes) */


/* local subroutine */

local int deftmpdir(char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	static cchar	*evp = getenv(varname.tmpdir) ;
	cchar		*tmpdir = sysword.w_tmpdir ;
	if (evp) {
	    if (ustat sb ; (rs = uc_stat(evp,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
		    cint	am = (R_OK|W_OK|X_OK) ;
		    if ((rs = perm(evp,-1,-1,nullptr,am)) >= 0) {
	                rs = sncpy(rbuf,rlen,evp) ;
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
} /* end subroutine (deftmpdir) */

local int chownpcs(cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	if (char *dbuf ; (rs = mem.hostname(&dbuf)) >= 0) ylikely {
	    cint	dlen = rs ;
	    if ((rs = getinetdomain(dbuf,dlen)) >= 0) ylikely {
		if (char *prbuf ; (rs = mem.mp(&prbuf)) >= 0) ylikely {
	             cint	prlen = rs ;
	    	     cchar	*prname = PRNAME ;
	             if ((rs = mkpr(prbuf,prlen,prname,dname)) >= 0) ylikely {
	 	         if (ustat sb ; (rs = uc_stat(prbuf,&sb)) >= 0) {
		             const uid_t	uid_pcs = sb.st_uid ;
		             const gid_t	gid_pcs = sb.st_gid ;
		             rs = uc_chown(dname,uid_pcs,gid_pcs) ;
		        } /* end if (uc_stat) */
	            } /* end if (mkpr) */
	    	    rs1 = mem.free(prbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getinetdomain) */
	    rs1 = mem.free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (chownpcs) */



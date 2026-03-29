/* getprogroot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the program root directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This subroutine is originally written.

	= 1998-03-10, David A­D­ Morano
	I added some comments.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprogroot

	Description:
	This subroutine tries to find (get) a program root directory
	from a given program name and a number of given possible
	supplied program root directories.

	Synopsis:
	int getprogroot(char *pr,mv prnames,int prlenp,char *obuf,cc *name) noex

	Arguments:
	pr		program root path
	prnames		list of program-root names
	prlenp		pointer to result variable to take resulting PR length
	obuf		buffer to receive result
	name		program to find

	Returns:
	>0		found the program path and this is the returned length
	0		program was found in present working directory
	<0		program was not found (system return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getnodename.h>
#include	<getnodedomain.h>
#include	<bufsizevar.hh>
#include	<ids.h>
#include	<storebuf.h>
#include	<dirseen.h>
#include	<nulstr.h>
#include	<strn.h>
#include	<strx.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<mkpr.h>
#include	<permx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getprogroot.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	SI		subinfo


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	ids		id ;
	dirseen		dirs ;
	int		prlen ;
	uint		f_dirs:1 ;
} ; /* end struct */


/* forward references */

local int	subinfo_start(SI *) noex ;
local int	subinfo_local(SI *,char *,cchar *,int) noex ;
local int	subinfo_pr(SI *,cchar *,char *,cchar *,int) noex ;
local int	subinfo_prs(SI *,mainv,char *,cchar *,int) noex ;
local int	subinfo_other(SI *,char *,cchar *,int) noex ;
local int	subinfo_check(SI *,cchar *,int,char *,cchar *,int) noex ;
local int	subinfo_dirstat(SI *,ustat *,cchar *,int) noex ;
local int	subinfo_record(SI *,ustat *,cchar *,int) noex ;
local int	subinfo_xfile(SI *,cchar *) noex ;
local int	subinfo_finish(SI *) noex ;

local int	mkdfname(char *,cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr cchar		*varpath = varname.path ;

static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int getprogroot(cc *pr,mainv prnames,int *prlenp,char *obuf,cc *namep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		outlen = 0 ;
	if (namep && obuf) {
	    rs = SR_INVALID ;
	    if (namep[0]) {
	        int		namel = lenstr(namep) ;
	        bool		f_changed = false ;
	        obuf[0] = '\0' ;
	        while ((namel > 0) && (namep[namel - 1] == '/')) {
	            f_changed = true ;
	            namel -= 1 ;
	        }
	        if (subinfo si ; (rs = subinfo_start(&si)) >= 0) {
	            rs = SR_NOENT ;
	            if (strnchr(namep,namel,'/') == nullptr) {
			/* check if the PCS root directory exists */
	                if ((rs < 0) && (rs != SR_NOMEM) && pr) {
	                    rs = subinfo_pr(&si,pr,obuf,namep,namel) ;
	                    outlen = rs ;
	                }
			/* check other program roots */
	                if ((rs < 0) && (rs != SR_NOMEM) && prnames) {
	                    rs = subinfo_prs(&si,prnames,obuf,namep,namel) ;
	                    outlen = rs ;
	                }
			/* search the rest of the execution path */
	                if ((rs < 0) && (rs != SR_NOMEM)) {
	                    rs = subinfo_other(&si,obuf,namep,namel) ;
	                    outlen = rs ;
	                }
	            } else {
	                rs = subinfo_local(&si,obuf,namep,namel) ;
	                outlen = (f_changed) ? namel : 0 ;
	            }
	            if (prlenp) {
	                *prlenp = si.prlen ;
	            }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (getprogroot) */


/* local subroutines */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    memclear(sip) ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->f_dirs) {
	    rs1 = dirseen_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->f_dirs = false ;
	}
	{
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_check(SI *sip,cc *d,int dlen,char *obuf,cc *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		outlen = 0 ; /* return-value */
	if (sip->f_dirs) {
	    if ((rs = dirseen_havename(&sip->dirs,d,dlen)) >= 0) {
	        rs = SR_NOENT ;
	    } else if (rs == rsn) {
		rs = SR_OK ;
	    }
	} /* end if (f_dirs) */
	if (rs >= 0) {
	    ustat sb ;
	    if ((rs = subinfo_dirstat(sip,&sb,d,dlen)) >= 0) {
	        if ((rs1 = dirseen_havedevino(&sip->dirs,&sb)) >= 0) {
	            rs = SR_NOENT ;
		} else if (rs == rsn) {
		    if ((rs = mkdfname(obuf,d,dlen,sp,sl)) >= 0) {
		        outlen = rs ;
		        if ((rs = subinfo_xfile(sip,obuf)),isNotAccess(rs)) {
	    		    rs = subinfo_record(sip,&sb,d,dlen) ;
		        }
		    } /* end if (mkdfname) */
	        }
	    }
	} /* end if (ok) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_check) */

local int subinfo_local(SI *sip,char *obuf,cc *sp,int sl) noex {
	int		rs ;
	int		outlen = 0 ;
	if ((rs = mkpath1w(obuf,sp,sl)) >= 0) {
	    outlen = rs ;
	    rs = subinfo_xfile(sip,obuf) ;
	}
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_local) */

local int subinfo_pr(SI *sip,cc *pr,char *obuf,cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		outlen = 0 ;
	if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	    if ((rs = mkpath2(dbuf,pr,"bin")) >= 0) {
	        rs = subinfo_check(sip,dbuf,-1,obuf,sp,sl) ;
	        outlen = rs ;
	    }
	    if ((rs < 0) && (rs != SR_NOMEM)) {
	        if ((rs = mkpath2(dbuf,pr,"sbin")) >= 0) {
	            rs = subinfo_check(sip,dbuf,-1,obuf,sp,sl) ;
	            outlen = rs ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        sip->prlen = lenstr(pr) ;
	    }
	    rs1 = lm_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_pr) */

local int subinfo_prs(SI *sip,mainv prnames,char *obuf,cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (char *dn ; (rs = lm_hn(&dn)) >= 0) {
	    if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	        if (char *pr ; (rs = lm_mp(&pr)) >= 0) {
		    cint	maxlen = rs ;
	            rs = SR_NOENT ;
	            for (int i = 0 ; prnames[i] ; i += 1) {
	                if ((rs1 = mkpr(pr,maxlen,prnames[i],dn)) >= 0) {
	                    rs = subinfo_pr(sip,pr,obuf,sp,sl) ;
	                }
	                if ((rs >= 0) || (rs == SR_NOMEM)) break ;
	            } /* end for */
	            rs1 = lm_free(pr) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getnodedomain) */
	    rs1 = lm_free(dn) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (subinfo_prs) */

local int subinfo_other(SI *sip,char *obuf,cc *sp,int sl) noex {
	int		rs = SR_NOENT ;
	int		outlen = 0 ; /* return-value */
	static cchar	*valp = getenv(varpath) ;
	sip->prlen = 0 ;
	if (valp) {
	    for (cc *tp ; (tp = strbrk(valp,":;")) != nullptr ; ) {
	        if (cint tl = intconv(tp - valp) ; tl > 0) {
	            rs = subinfo_check(sip,valp,tl,obuf,sp,sl) ;
	            outlen = rs ;
	        }
	        valp = (tp + 1) ;
	        if ((rs >= 0) || (rs == SR_NOMEM)) break ;
	    } /* end for */
	    if ((rs < 0) && (rs != SR_NOMEM) && (valp[0] != '\0')) {
	        rs = subinfo_check(sip,valp,-1,obuf,sp,sl) ;
	        outlen = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_other) */

local int subinfo_dirstat(SI *sip,ustat *sbp,cc *d,int dlen) noex {
	int		rs ;
	int		rs1 ;
	cchar		*dnp{} ;
	if (nulstr ns ; (rs = ns.start(d,dlen,&dnp)) >= 0) {
	    if ((rs = u_stat(dnp,sbp)) >= 0) {
	        rs = SR_NOTFOUND ;
	        if (S_ISDIR(sbp->st_mode)) {
	            rs = permid(&sip->id,sbp,X_OK) ;
		}
	    }
	    rs1 = ns.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (numstr) */
	return rs ;
}
/* end subroutine (subinfo_dirstat) */

local int subinfo_record(SI *sip,ustat *sbp,cc *d,int dlen) noex {
	int		rs = SR_OK ;
	if (! sip->f_dirs) {
	    rs = dirseen_start(&sip->dirs) ;
	    sip->f_dirs = (rs >= 0) ;
	}
	if (rs >= 0) {
	    rs = dirseen_add(&sip->dirs,d,dlen,sbp) ;
	}
	return rs ;
}
/* end subroutine (subinfo_record) */

local int subinfo_xfile(SI *sip,cc *name) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode)) {
	        rs = permid(&sip->id,&sb,X_OK) ;
	    }
	} /* end if (u_stat) */
	return rs ;
}
/* end subroutine (subinfo_xfile) */

local int mkdfname(char *rbuf,cc *dnp,int dnl,cc *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    if (storebuf buf(rbuf,rs) ; (rs = buf.strw(dnp,dnl)) >= 0) {
	        cint dl = rs ;
	        if ((rs >= 0) && (dl > 0) && (rbuf[dl - 1] != '/')) {
	            rs = buf.chr('/') ;
	        }
	        if (rs >= 0) {
	            rs = buf.strw(sp,sl) ;
	        }
	        len = buf.idx ;
	    } /* end if (storebuf) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkdfname) */



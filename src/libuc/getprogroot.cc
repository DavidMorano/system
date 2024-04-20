/* getprogroot SUPPORT */
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
	int getprogroot(char *pr,cc **prnames,int prlenp,char *obuf,cc *name)

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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<varnames.hh>
#include	<mallocxx.h>
#include	<ids.h>
#include	<storebuf.h>
#include	<dirseen.h>
#include	<nulstr.h>
#include	<strn.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getprogroot.h"


/* local defines */

#define	SUBINFO		struct subinfo
#define	SI		SUBINFO


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getnodedomain(char *,char *) noex ;
    extern int	mkpr(char *,int,cchar *,cchar *) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	ids		id ;
	dirseen		dirs ;
	uint		f_dirs:1 ;
	int		prlen ;
} ;


/* forward references */

static int	subinfo_start(subinfo *) noex ;
static int	subinfo_local(subinfo *,char *,cchar *,int) noex ;
static int	subinfo_prs(subinfo *,cchar **,char *,cchar *,int) noex ;
static int	subinfo_pr(subinfo *,cchar *,char *,cchar *,int) noex ;
static int	subinfo_other(subinfo *,char *,cchar *,int) noex ;
static int	subinfo_check(subinfo *,cchar *,int,char *,cchar *,int) noex ;
static int	subinfo_dirstat(subinfo *,USTAT *,cchar *,int) noex ;
static int	subinfo_record(subinfo *,USTAT *,cchar *,int) noex ;
static int	subinfo_xfile(subinfo *,cchar *) noex ;
static int	subinfo_finish(subinfo *) noex ;

static int	mkdfname(char *,cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr cchar		*varpath = varname.path ;

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int getprogroot(cc *pr,cc **prnames,int *prlenp,char *obuf,cc *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		outlen = 0 ;
	if (name && obuf) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        subinfo		si, *sip = &si ;
	        int		namelen = strlen(name) ;
	        bool		f_changed = false ;
	        obuf[0] = '\0' ;
	        while ((namelen > 0) && (name[namelen - 1] == '/')) {
	            f_changed = true ;
	            namelen -= 1 ;
	        }
	        if ((rs = subinfo_start(sip)) >= 0) {
	            rs = SR_NOENT ;
	            if (strnchr(name,namelen,'/') == nullptr) {
/* check if the PCS root directory exists */
	                if ((rs < 0) && (rs != SR_NOMEM) && pr) {
	                    rs = subinfo_pr(sip,pr,obuf,name,namelen) ;
	                    outlen = rs ;
	                }
/* check other program roots */
	                if ((rs < 0) && (rs != SR_NOMEM) && prnames) {
	                    rs = subinfo_prs(sip,prnames,obuf,name,namelen) ;
	                    outlen = rs ;
	                }
/* search the rest of the execution path */
	                if ((rs < 0) && (rs != SR_NOMEM)) {
	                    rs = subinfo_other(sip,obuf,name,namelen) ;
	                    outlen = rs ;
	                }
	            } else {
	                rs = subinfo_local(sip,obuf,name,namelen) ;
	                outlen = (f_changed) ? namelen : 0 ;
	            }
	            if (prlenp != nullptr) {
	                *prlenp = sip->prlen ;
	            }
	            rs1 = subinfo_finish(sip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (getprogroot) */


/* local subroutines */

static int subinfo_start(subinfo *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    memclear(sip) ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(subinfo *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->f_dirs) {
	    rs1 = dirseen_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_other(SI *sip,char *obuf,cc *np,int nl) noex {
	int		rs = SR_NOENT ;
	int		outlen = 0 ;
	cchar		*sp = getenv(varpath) ;
	sip->prlen = 0 ;
	if (sp) {
	    cchar	*tp ;
	    while ((tp = strpbrk(sp,":;")) != nullptr) {
	        {
	            rs = subinfo_check(sip,sp,(tp - sp),obuf,np,nl) ;
	            outlen = rs ;
	        }
	        sp = (tp + 1) ;
	        if ((rs >= 0) || (rs == SR_NOMEM)) break ;
	    } /* end while */
	    if ((rs < 0) && (rs != SR_NOMEM) && (sp[0] != '\0')) {
	        rs = subinfo_check(sip,sp,-1,obuf,np,nl) ;
	        outlen = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_other) */

static int subinfo_check(SI *sip,cc *d,int dlen,char *obuf,cc *np,int nl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		outlen = 0 ;
	if (sip->f_dirs) {
	    if ((rs = dirseen_havename(&sip->dirs,d,dlen)) >= 0) {
	        rs = SR_NOENT ;
	    } else if (rs == rsn) {
		rs = SR_OK ;
	    }
	} /* end if (f_dirs) */
	if (rs >= 0) {
	    USTAT	sb ;
	    if ((rs = subinfo_dirstat(sip,&sb,d,dlen)) >= 0) {
	        if ((rs1 = dirseen_havedevino(&sip->dirs,&sb)) >= 0) {
	            rs = SR_NOENT ;
		} else if (rs == rsn) {
	            rs = SR_OK ;
		}
		if ((rs = mkdfname(obuf,d,dlen,np,nl)) >= 0) {
		    outlen = rs ;
		    if ((rs = subinfo_xfile(sip,obuf)),isNotAccess(rs)) {
	    		rs = subinfo_record(sip,&sb,d,dlen) ;
		    }
		} /* end if (mkdfname) */
	    }
	} /* end if (ok) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_check) */

static int subinfo_local(SI *sip,char *obuf,cc *np,int nl) noex {
	int		rs ;
	int		outlen = 0 ;
	if ((rs = mkpath1w(obuf,np,nl)) >= 0) {
	    outlen = rs ;
	    rs = subinfo_xfile(sip,obuf) ;
	}
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_local) */

static int subinfo_prs(SI *sip,cc **prnames,char *obuf,cc *np,int nl) noex {
	int		rs ;
	int		rs1 ;
	char		*dn{} ;
	if ((rs = malloc_hn(&dn)) >= 0) {
	    if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	        char	*pr{} ;
	        if ((rs = malloc_mp(&pr)) >= 0) {
		    cint	maxlen = rs ;
	            rs = SR_NOENT ;
	            for (int i = 0 ; prnames[i] ; i += 1) {
	                if ((rs1 = mkpr(pr,maxlen,prnames[i],dn)) >= 0) {
	                    rs = subinfo_pr(sip,pr,obuf,np,nl) ;
	                }
	                if ((rs >= 0) || (rs == SR_NOMEM)) break ;
	            } /* end for */
	            rs1 = uc_free(pr) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getnodedomain) */
	    rs1 = uc_free(dn) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (subinfo_prs) */

static int subinfo_pr(SI *sip,cc *pr,char *obuf,cc *np,int nl) noex {
	int		rs ;
	int		rs1 ;
	int		outlen = 0 ;
	char		*dbuf{} ;
	if ((rs = malloc_mp(&dbuf)) >= 0) {
	    rs = mkpath2(dbuf,pr,"bin") ;
	    if (rs >= 0) {
	        rs = subinfo_check(sip,dbuf,-1,obuf,np,nl) ;
	        outlen = rs ;
	    }
	    if ((rs < 0) && (rs != SR_NOMEM)) {
	        if ((rs = mkpath2(dbuf,pr,"sbin")) >= 0) {
	            rs = subinfo_check(sip,dbuf,-1,obuf,np,nl) ;
	            outlen = rs ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        sip->prlen = strlen(pr) ;
	    }
	    rs1 = uc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (non-null) */
	return (rs >= 0) ? outlen : rs ;
}
/* end subroutine (subinfo_pr) */

static int subinfo_dirstat(SI *sip,USTAT *sbp,cc *d,int dlen) noex {
	nulstr		ns ;
	int		rs ;
	int		rs1 ;
	cchar		*dnp ;
	if ((rs = nulstr_start(&ns,d,dlen,&dnp)) >= 0) {
	    if ((rs = u_stat(dnp,sbp)) >= 0) {
	        rs = SR_NOTFOUND ;
	        if (S_ISDIR(sbp->st_mode))
	            rs = sperm(&sip->id,sbp,X_OK) ;
	    }
	    rs1 = nulstr_finish(&ns) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (numstr) */
	return rs ;
}
/* end subroutine (subinfo_dirstat) */

static int subinfo_record(SI *sip,USTAT *sbp,cc *d,int dlen) noex {
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

static int subinfo_xfile(SI *sip,cc *name) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(name,&sb)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode)) {
	        rs = sperm(&sip->id,&sb,X_OK) ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_xfile) */

static int mkdfname(char *rbuf,cc *d,int dlen,cc *np,int nl) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	rlen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,d,dlen) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (rbuf[i - 1] != '/')) {
	        rs = storebuf_chr(rbuf,rlen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,np,nl) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkdfname) */



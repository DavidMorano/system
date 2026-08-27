/* getprogroot SUPPORT (Get-Program-Root) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the program root directory */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
	int getprogroot(char *rbuf,cc *pr,mainv prns,cc *name) noex

	Arguments:
	rbuf		result buffer pointer (MAXPATHLEN)
	pr		program root path
	prns		list of program-root names
	name		program to find

	Returns:
	>0		found the program path and this is the returned length
	0		program was found in present working directory
	<0		program was not found (system return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strx.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkpathxw.h>		/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"getprogroot.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	char		*rbuf ;		/* caller supplied */
	cchar		*pr ;		/* caller supplied */
	mainv		prns ;		/* caller supplied */
	cchar		*namp ;
	ids		id ;
	dirseen		dirs ;
	int		naml ;
	uint		f_dirs:1 ;
	subinfo(char *r,cchar *p,mainv s) noex : rbuf(r), pr(p), prns(s) { 
	    namp = nullptr ;
	    naml = 0 ;
	    f_dirs = false ;
	} ; /* end ctor */
	int operator () (cchar *nap,int nal) noex ;
	int start	() noex ;
	int trypr	(cchar *,int) noex ;
	int tryprs	(cchar *,int) noex ;
	int tryother	(cchar *,int) noex ;
	int trylocal	(cchar *,int) noex ;
	int check	(cchar *,int,cchar *,int) noex ;
	int dirstat	(ustat *,cchar *,int) noex ;
	int record	(ustat *,cchar *,int) noex ;
	int xfile	(cchar *) noex ;
	int finish	() noex ;
    } ; /* end struct (subinfo) */
	typedef int (subinfo::*try_m)(cchar *,int) noex ;
} /* end namespace */


/* forward references */

local int	mkdfname	(char *,cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr try_m		tries[] = {
    	&subinfo::trypr,
    	&subinfo::tryprs,
    	&subinfo::tryother
} ; /* end array */

static bufsizevar	maxpathlen(bufsize_mp) ;
constexpr cchar		*varpath	= varname.path ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int getprogroot(char *rbuf,cc *pr,con mainv prns,cc *namep) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	DPRINTF("ent pn=%s\n",namep) ;
	if (rbuf && namep) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (int namel = rmslashes(namep) ; namel > 0) ylikely {
		if (subinfo so(rbuf,pr,prns) ; (rs = so(namep,namel)) >= 0) {
		    rl = rs ;
		} /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getprogroot) */

int subinfo::operator ()(cchar *namep,int namel) noex {
    	int		rs ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	if ((rs = start()) >= 0) ylikely {
	    if (strnchr(namep,namel,'/') == nullptr) {
		for (cauto &m : tries) {
		    rs = (this->*m)(namep,namel) ;
		    if (rs) break ;
		} /* end for */
	    } else {
	        rs = trylocal(namep,namel) ;
	    }
	    if (rs > 0) rl = rs ;
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end is (strt-finish) */
	return (rs >= 0) ? rl : rs ;
} /* end method (subinfo::operator) */


/* local subroutines */

int subinfo::start() noex {
    	int		rs ;
	if ((rs = id.load) >= 0) ylikely {
	    if ((rs = dirs.start) >= 0) ylikely {
		f_dirs = true ;
	    } /* end if (dirseen_start) */
	    if (rs < 0) {
		id.release() ;
	    }
	} /* end if (id_load) */
	return rs ;
} /* end subroutine (subinfo_start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (f_dirs) {
	    rs1 = dirs.finish ;
	    if (rs >= 0) rs = rs1 ;
	    f_dirs = false ;
	}
	{
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (subinfo_finish) */

int subinfo::check(cc *dirp,int dirl,cc *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	bool		ftry = false ;
	DPRINTF("ent fdirs=%u\n",f_dirs) ;
	if_constexpr (f_debug) {
	    {
	    strnul ds(dirp,dirl) ;
	    DPRINTF("ds=%s\n",ccp(ds)) ;
	    }
	    strnul ss(sp,sl) ;
	    DPRINTF("ss=%s\n",ccp(ss)) ;
	}
	if (f_dirs) {
	    if ((rs = dirs.havename(dirp,dirl)) >= 0) {
	    DPRINTF("dirs_havename 1 rs=%d\n",rs) ;
	        rs = SR_OK ;
	    } else if (rs == rsn) {
	    DPRINTF("dirs_havename 2 rs=%d\n",rs) ;
		rs = SR_OK ;
		ftry = true ;
	    } else {
	    DPRINTF("dirs_havename 3 rs=%d\n",rs) ;
	    }
	} else {
	    ftry = true ;
	} /* end if (f_dirs) */
	    DPRINTF("mid rs=%d ftry=%u\n",rs,ftry) ;
	if ((rs >= 0) && ftry) ylikely {
	    if (ustat sb ; (rs = dirstat(&sb,dirp,dirl)) > 0) ylikely {
		cint dl = rs ;
	        if ((rs = dirs.havedevino(&sb)) >= 0) {
	    	    DPRINTF("dirs_havedevino() rs=%d\n",rs) ;
	            rs = SR_OK ; /* <- dummy */
		} else if (rs == rsn) {
	    	    DPRINTF("dirs_havedevino() rs=%d\n",rs) ;
		    if ((rs = mkdfname(rbuf,dirp,dirl,sp,sl)) >= 0) {
	    	        DPRINTF("mkdfname rbuf=%s\n",rbuf) ;
		        if ((rs = xfile(rbuf)) > 0) {
			    rl = dl ;
			} else if (rs == 0) {
	    		    rs = record(&sb,dirp,dirl) ;
		        }
		    } /* end if (mkdfname) */
	        } /* end if */
	    } /* end if (dirstat) */
	} /* end if (ok) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_check) */

int subinfo::trylocal(cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	    cint dlen = rs ;
	    if ((rs = getpwd(dbuf,dlen)) >= 0) {
		DPRINTF("pwd=%s\n",dbuf) ;
	        if ((rs = mkpath2w(rbuf,dbuf,sp,sl)) >= 0) ylikely {
	            if (cint pl = rs ; (rs = xfile(rbuf)) > 0) {
		        cchar *dummyp{} ;
		        if (cint dl = sfdirname(dbuf,pl,&dummyp) ; dl > 0) {
		            rl = dl ;
			    rbuf[rl] = '\0' ;
		        } /* end if (sfbasename) */
	            } /* end if (xfile) */
	        } /* end if (mkpath) */
	    } /* end if (getpwd) */
	    rs1 = lm_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (non-null) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_trylocal) */

int subinfo::trypr(cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (pr) {
	    cint pl = lenstr(pr) ;
	    DPRINTF("pr=%s\n",pr) ;
	    if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	        if ((rs = mkpath(dbuf,pr,"bin")) >= 0) {
		    if_constexpr (f_debug) {
			strnul ds(sp,sl) ;
	    	        DPRINTF("-> check=%s\n",ccp(ds)) ;
		    }
	            if ((rs = check(dbuf,-1,sp,sl)) > 0) {
	                rl = pl ;
		    } else if (rs == 0) {
	                if ((rs = mkpath(dbuf,pr,"sbin")) >= 0) {
	                    if ((rs = check(dbuf,-1,sp,sl)) > 0) {
	                        rl = pl ;
			    }
	                } /* end if (mkpath) */
		    } /* end if */
		    rbuf[rl] = '\0' ;
	        } /* end if */
	        rs1 = lm_free(dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (non-null) */
	} /* end if (possible) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_trypr) */

int subinfo::tryprs(cc *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (prns) {
	    if (char *dbuf ; (rs = lm_hostname(&dbuf)) >= 0) {
	        if ((rs = getnodedomain(np,dbuf)) >= 0) {
	            if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
		        cint	plen = rs ;
	                for (int i = 0 ; (rs >= 0) && prns[i] ; i += 1) {
	                    if ((rs = mkpr(pbuf,plen,prns[i],dbuf)) >= 0) {
				pr = pbuf ;
	                        rs = trypr(sp,sl) ;
				rl = rs ;
	                    } /* end if (mkpr) */
			    if (rl > 0) break ;
	                } /* end for */
	                rs1 = lm_free(pbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (getnodedomain) */
	        rs1 = lm_free(dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (possible) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_tryprs) */

int subinfo::tryother(cc *sp,int sl) noex {
	static cchar	*valp = getenver(varpath) ;
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (valp) {
	    int dl = 0 ;
	    cchar *dummyp{} ;
	    cchar *pp = valp ;
	    DPRINTF("paths=%s\n",valp) ;
	    for (cc *tp ; (tp = strbrk(pp,":;")) != np ; ) {
		DPRINTF("tl=%d\n",intconv(tp - pp)) ;
	        if (cint tl = intconv(tp - pp) ; tl > 0) {
		    if_constexpr (f_debug) {
			strnul ps(pp,tl) ;
		        DPRINTF("path=%s\n",ccp(ps)) ;
		    }
		    if ((dl = sfdirname(pp,tl,&dummyp)) > 0) {
	                if ((rs = check(pp,tl,sp,sl)) > 0) {
	                    rl = dl ;
			}
	            }
		} /* end if (sfdirname) */
	        pp = (tp + 1) ;
	        if (rs) break ;
	    } /* end for */
	    if ((rs >= 0) && (rl == 0) && pp[0]) {
		if ((dl = sfdirname(pp,-1,&dummyp)) > 0) {
	            if ((rs = check(pp,-1,sp,sl)) > 0) {
	                rl = dl ;
		    }
		}
	    } /* end if (remainder) */
	    rbuf[rl] = '\0' ;
	} /* end if (non-null) */
	DPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_other) */

/* status-block is the output */
int subinfo::dirstat(ustat *sbp,cc *dirp,int dirl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	cchar		*dnp{} ;
	DPRINTF("ent fdirs=%u\n",f_dirs) ;
	if (nulstr ns ; (rs = ns.start(dirp,dirl,&dnp)) >= 0) ylikely {
	    cint dl = rs ;
	    if ((rs = u_stat(dnp,sbp)) >= 0) {
		DPRINTF("stat 1 rs=%d\n",rs) ;
	        if (S_ISDIR(sbp->st_mode)) {
	            if ((rs = permids(&id,sbp,X_OK)) >= 0) {
		        rl = dl ;
		    } else if (isNotAccess(rs)) {
		        rs = SR_OK ;
		    }
	        } /* end if (is-dir) */
	    } else if (isNotPresent(rs)) {
		DPRINTF("stat 2 rs=%d\n",rs) ;
		memclear(sbp) ;
		rs = SR_OK ;
	    } /* end if (u_stat) */
	    rs1 = ns.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (numstr) */
	DPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (subinfo_dirstat) */

int subinfo::xfile(cc *name) noex {
	int		rs ;
	int		fok = false ; /* return-value */
	if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        if ((rs = permids(&id,&sb,X_OK)) >= 0) {
		    fok = true ;
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (is-reg) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (u_stat) */
	return (rs >= 0) ? fok : rs ;
} /* end subroutine (subinfo_xfile) */

int subinfo::record(ustat *sbp,cc *dirp,int dirl) noex {
	int		rs = SR_OK ;
	if (! f_dirs) {
	    rs = dirs.start ;
	    f_dirs = (rs >= 0) ;
	}
	if (rs >= 0) ylikely {
	    rs = dirs.add(dirp,dirl,sbp) ;
	} /* end if (ok) */
	return rs ;
} /* end subroutine (subinfo_record) */

local int mkdfname(char *rbuf,cc *dnp,int dnl,cc *sp,int sl) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = maxpathlen) >= 0) ylikely {
	    if (storebuf buf(rbuf,rs) ; (rs = buf.strw(dnp,dnl)) >= 0) {
	        cint dl = rs ;
	        if ((rs >= 0) && (dl > 0) && (rbuf[dl - 1] != '/')) {
	            rs = buf.chr('/') ;
	        }
	        if (rs >= 0) ylikely {
	            rs = buf.strw(sp,sl) ;
	        }
	        len = buf.idx ;
	    } /* end if (storebuf) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mkdfname) */



/* proginfo1 MODULE (Module-Implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

	= 2026-10-20, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	proginfo

	Description:
	This module provides a very basic set of simple subroutines
	for basic program management.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX |gethostid(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strw.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<prognamevar.hh>	/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.base ;

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int proginfo_ctor(PI *op,Args ... args) noex {
    	cnullptr	np{} ;
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->sdp = new(nt) vecstr) != np) ylikely {
	        if ((op->idp = new(nt) ids) != np) ylikely {
	            if ((op->lfp = new(nt) logfile) != np) ylikely {
			rs = SR_OK ;
		    } /* end if (new-logfile) */
		    if (rs < 0) {
		        delete op->idp ;
		        op->idp = nullptr ;
		    } /* end if (error) */
		} /* end if (new-ids) */
		if (rs < 0) {
		    delete op->sdp ;
		    op->sdp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_ctor) */

local int proginfo_dtor(PI *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lfp) {
		delete op->lfp ;
		op->lfp = nullptr ;
	    } /* end if (delete-logfile) */
	    if (op->idp) {
		delete op->idp ;
		op->idp = nullptr ;
	    } /* end if (delete-ids) */
	    if (op->sdp) {
		delete op->sdp ;
		op->sdp = nullptr ;
	    } /* end if (delete-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_dtor) */

local int	proginfo_hargz	(PI *) noex ;		/* helper */
local int	proginfo_hver	(PI *,cchar *) noex ;	/* helper */
local int	proginfo_setdefnames(PI *) noex ;	/* helper */

#ifdef	COMMENT
local int	proginfo_setdefdn(PI *) noex ;
local int	proginfo_setdefpn(PI *) noex ;
#endif

local ccharp	char_progname	(PI *) noex ;
local ccharp	char_progename	(PI *) noex ;
local ccharp	char_progdname	(PI *) noex ;
local ccharp	char_rootdname	(PI *) noex ;

[[maybe_unused]]
local bool	hasourbad(cchar *,int) noex ;


/* local variables */

statcexpr int		vn = 10 ;
statcexpr cint		vo = (vecstrm.conserve | vecstrm.reuse | vecstrm.swap) ;
statcexpr bool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int proginfo::pstart(int ac,mainv av,mainv ev,ccp v) noex {
	int		rs ;
	(void) v ;
	DEBUGPRINTF("proginfo-mv-ent\n") ;
	if ((rs = proginfo_ctor(this)) >= 0) ylikely {
	    if (ac >= 0) argc = ac ;
	    if (av) argv = av ;
	    if (ev) envv = ev ;
	    if ((rs = sdp->start(vn,vo)) >= 0) ylikely {
	        if ((rs = idp->load) >= 0) ylikely {
	            if ((rs = proginfo_pwd(this)) >= 0) ylikely {
		        if ((rs = proginfo_hargz(this)) >= 0) ylikely {
	                    if ((rs = proginfo_setdefnames(this)) >= 0) {
			        rs = proginfo_hver(this,v) ;
	                    }
	                } /* end if (proginfo_hargz) */
	            } /* end if (pwd) */
		} /* end if (ids_start) */
	        if (rs < 0) {
	            sdp->finish() ;
	        } /* end if (error) */
	    } /* end if (vecstr-stores) */
	    if (rs < 0) {
		proginfo_dtor(this) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	DEBUGPRINTF("proginfo-mv-ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (proginfo::pstart) */

int proginfo::pfinish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if (lfp && pf.logfile) {
	    rs1 = lfp->close ;
	    if (rs >= 0) rs = rs1 ;
	    pf.logfile = false ;
	} /* end */
	DEBUGPRINTF("1 rs=%d\n",rs) ;
	if (idp) {
	    rs1 = idp->release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end */
	DEBUGPRINTF("2 rs=%d\n",rs) ;
	if (sdp) {
	    rs1 = sdp->finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end */
	DEBUGPRINTF("3 rs=%d\n",rs) ;
	{
	    rs1 = proginfo_dtor(this) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (proginfo::pfinish) */

int proginfo::pncpu() noex {
	int		rs = ncpus ;
	if (rs == 0) {
	    rs = 1 ;
	} /* end if (default value) */
	return rs ;
} /* end method (proginfo::pncpu) */

int proginfo::setent(cchar **epp,cchar *valp,int vall) noex {
	int		rs = SR_BUGCHECK ;
	int		len = 0 ;
	if (sdp) ylikely {
	    rs = SR_FAULT ;
	    if (int oi = -1 ; epp && valp) ylikely {
		rs = SR_OK ;
	        if (*epp) {
	            oi = sdp->findaddr(*epp) ;
	        }
	        if (valp) {
	            len = lenstr(valp,vall) ;
	            rs = sdp->store(valp,len,epp) ;
	        } else {
	            *epp = nullptr ;
	        } /* end */
	        if ((rs >= 0) && (oi >= 0)) {
	            (void) sdp->del(oi) ;
	        } /* end */
	    } /* end if (non-null) */
	} /* end if (bug-check) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (proginfo::setentry) */

int proginfo::setversion(ccharp ver) noex {
	int		rs = SR_FAULT ;
	if (ver) ylikely {
	    ccharpp vpp = &version ;
	    rs = setent(vpp,ver) ;
	} /* end */
	return rs ;
} /* end subroutine (proginfo::setversion) */

int proginfo::setbanner(cchar *valp) noex {
	int		rs = SR_FAULT ;
	if (valp) {
	    ccharpp vpp = &banner ;
	    rs = setent(vpp,valp,-1) ;
	} /* end */
	return rs ;
} /* end subroutine (proginfo_setbanner) */

#ifdef	COMMENT
int proginfo::setsearch(cchar *var,cchar *valp) noex {
	int		rs = SR_FAULT ;
	int		cl = -1 ;
	cchar		*cp = valp ;

	if ((cp == nullptr) && (var != nullptr)) {
	    cp = getourenv(envv,var) ;
	    if (hasourbad(cp,-1) || ((cp != nullptr) && (cp[0] == '\0'))) {
	        cp = nullptr ;
	    }
	}

	if ((cp == nullptr) && (pip->progname != nullptr)) {
	    cp = pip->progname ;
	    if (cchar *tp ; (tp = strrchr(cp,'.')) != nullptr) {
	        cl = (tp - cp) ;
	    }
	}

	if ((rs >= 0) && (cp != nullptr)) {
	    char	searchname[MAXNAMELEN+1] ;
	    int		ml = MAXNAMELEN ;
	    if (hasuc(cp,cl)) {
	        if ((cl > 0) && (cl < ml)) ml = cl ;
	        cl = strwcpylc(searchname,cp,ml) - searchname ;
	        cp = searchname ;
	    }
	    {
	        cchar	**vpp = &searchname ;
	        rs = pip->setent(vpp,cp,cl) ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (proginfo::setsearch) */
#endif /* COMMENT */

#ifdef	COMMENT
/* set program directory and program name (as might be possible) */
int proginfo_setprogname(PI *pip,cchar *ap) noex {
	int		rs = SR_OK ;
	int		al ;
	int		dl, bl ;
	cchar		*en ;
	cchar		*dn ;
	cchar		*dp, *bp ;

	if (ap == nullptr) return SR_FAULT ;

	en = pip->progename ;
	dn = pip->progdname ;
	al = lenstr(ap) ;
	while ((al > 0) && (ap[al-1] == '/')) {
	    al -= 1 ;
	}

	if ((rs >= 0) && (pip->progename == nullptr)) {
	    if (ap[0] == '/') {
	        cchar	**vpp = &pip->progename ;
	        rs = pip->setent(vpp,ap,-1) ;
	    }
	}
	bl = sfbasename(ap,al,&bp) ;
	/* set a program dirname? */
	if (rs >= 0) {
	    if (((en == nullptr) || (dn == nullptr)) && 
	        ((dl = sfdirname(ap,al,&dp)) > 0)) {
	        int	f_parent = false ;
	        int	f_pwd = false ;
	        int	f = false ;

	        if (dp[0] == '.') {
	            f_pwd = (strcmp(dp,".") == 0) ;
	            if (! f_pwd) {
	                f_parent = (strcmp(dp,"..") == 0) ;
	            }
	            f = f_pwd || f_parent ;
	        } /* end if */

	        if (f) {
	            if (pip->pwdbuf == nullptr) {
	                rs = proginfo_pwd(pip) ;
	            } /* end */
	            if (rs >= 0) {
	                if (f_pwd) {
	                    dp = pip->pwdbuf ;
	                    dl = pip->pwdlen ;
	                } else {
	                    dl = sfdirname(pip->pwdbuf,pip->pwdlen,&dp) ;
	                }
	            } /* end if (PWD or parent) */
	        } /* end if */

	        if ((rs >= 0) && (pip->progdname == nullptr)) {
	            cchar	**vpp = &pip->progdname ;
	            rs = pip->setent(vpp,dp,dl) ;
	        }

	        if ((rs >= 0) && (pip->progename == nullptr)) {
	            if ((bp != nullptr) && (bl > 0)) {
	                char	ename[MAXPATHLEN+1] ;
	                if ((rs = mkpath2w(ename,pip->progdname,bp,bl)) >= 0) {
	                    cchar	**vpp = &pip->progename ;
	                    rs = pip->setent(vpp,ename,-1) ;
	                }
	            } /* end if */
	        } /* end if */

	    } /* end if (have a dirname) */
	} /* end if (ok) */
	/* set a program basename? */
	if (rs >= 0) {
	    if ((bp != nullptr) && (bl > 0)) {
	        if ((bl = rmext(bp,bl)) == 0) {
	            bp = NOPROGNAME ;
	            bl = -1 ;
	        }
	    }
	    if ((bp != nullptr) && (bl > 0)) {
	        if (hasourbad(bp,bl)) {
	            bp = nullptr ;
	            bl = 0 ;
	        }
	    }
	    if ((bp != nullptr) && (bl > 0) && (bp[0] == '-')) {
	        pip->pf.progdash = true ;
	        bp += 1 ;
	        bl -= 1 ;
	    }
	    if ((bp != nullptr) && (bl != 0)) {
	        cchar	**vpp = &pip->progname ;
	        rs = pip->setent(vpp,bp,bl) ;
	    }
	} /* end if (basename) */
	return rs ;
} /* end subroutine (proginfo_setprogname) */
#endif /* COMMENT */

#ifdef	COMMENT
int proginfo_setprogroot(PI *pip,cchar *prp,int prl) noex {
	int		rs = SR_OK ;
	char		tbuf[MAXPATHLEN + 1] ;

	if (prp == nullptr) return SR_FAULT ;

	if (prl < 0) prl = lenstr(prp) ;

	if (prp[0] != '/') {
	    if ((rs = proginfo_pwd(pip)) >= 0) {
	        rs = mkpath2w(tbuf,pip->pwdbuf,prp,prl) ;
	        prl = rs ;
	        prp = tbuf ;
	    }
	}
	if (rs >= 0) {
	    ccharpp vpp = &pip->pr ;
	    rs = pip->setent(vpp,prp,prl) ;
	}
	return rs ;
} /* end subroutine (proginfo_setprogroot) */
#endif /* COMMENT */

#ifdef	COMMENT
int proginfo_setexecname(PI *pip,cchar *enp) noex {
	int		rs = SR_OK ;

	if (enp == nullptr) return SR_FAULT ;

	if (pip->progename == nullptr) {
	    int		enl = lenstr(enp) ;
	    while ((enl > 0) && (enp[enl-1] == '/')) enl -= 1 ;
	    if (enl > 0) {
	        cchar	**vpp = &pip->progename ;
	        rs = pip->setent(vpp,enp,enl) ;
	    }
	} else {
	    if (pip->progename != nullptr) {
	        rs = lenstr(pip->progename) ;
	    }
	}
	return rs ;
} /* end subroutine (proginfo_setexecname) */
#endif /* COMMENT */

int proginfo_pwd(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (pip->pwdbuf == nullptr) {
	    if (charp pbuf ; (rs = mem.mp(&pbuf)) >= 0) ylikely {
	        if ((rs = getpwd(pbuf,rs)) >= 0) ylikely {
	            ccharpp vpp = &pip->pwdbuf ;
	            len = rs ;
	            pip->pwdlen = rs ;
	            rs = pip->setent(vpp,pbuf,rs) ;
	        } /* end if (getpwd) */
		rs1 = mem.free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    len = pip->pwdlen ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (proginfo_pwd) */

int proginfo_progpname(PI *pip) noex {
    	int		rs ;
	int		rs1 ;
	if (pip->progname.valp == nullptr) {
	    if (charp nbuf ; (rs = mem.mn(&nbuf)) >= 0) {
	        cint	ac = pip->argc ;
	        mainv	av = pip->argv ;
	        mainv	ev = pip->envv ;
	        cint	nlen = rs ;
	        if (cchar *pn = prognamevar(ac,av,ev)) {
		    if (cchar *rp ; (rs = pip->setent(&rp,pn)) >= 0) {
		        pip->progname.valp = rp ;
		    } /* end if */
	        } else {
		    if ((rs = u_progname(nbuf,nlen)) >= 0) {
		        if (cchar *rp ; (rs = pip->setent(&rp,nbuf,rs)) >= 0) {
		            pip->progname.valp = rp ;
		        } /* end if */
		    } /* end if */
	        } /* end if */
	        rs1 = mem.free(nbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = lenstr(pip->progname.valp) ;
	} /* end */
	return rs ;
} /* end subroutine (proginfo_progpname) */

int proginfo_getpname(PI *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) {
	    rbuf[0] = eos ;
	    if ((rs = proginfo_progpname(pip)) >= 0) {
	        if (pip->progname.valp) {
	            rs = sncpy(rbuf,rlen,pip->progname.valp) ;
	        } /* end */
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_getpname) */

int proginfo_getename(PI *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) {
	    rbuf[0] = eos ;
	    if ((rs = proginfo_progename(pip)) >= 0) {
	        if (pip->progename.valp) {
	            rs = sncpy(rbuf,rlen,pip->progename.valp) ;
	        } /* end */
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_getename) */

int proginfo_getpwd(PI *pip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf) ylikely {
	    rbuf[0] = eos ;
	    if (pip->pwdbuf == nullptr) {
	        rs = proginfo_pwd(pip) ;
	    } /* end */
	    if (rs >= 0) {
	        if (rlen >= pip->pwdlen) {
	            rs = sncpy(rbuf,rlen,pip->pwdbuf) ;
	        } else {
	            rs = SR_OVERFLOW ;
	        } /* end if (error) */
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_getpwd) */

int proginfo_getenv(PI *pip,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (pip && sp) ylikely {
	    if (pip->envv) ylikely {
	        rs = getev(pip->envv,sp,sl,rpp) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proginfo_getenv) */

int proginfo_nodename(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nl = 0 ; /* return-value */
	if (pip->nodename == nullptr) {
	    if (charp nbuf ; (rs = mem.nn(&nbuf)) >= 0) {
		cint	nlen = rs ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) {
	            ccharpp vpp = &pip->nodename ;
	            nl = rs ;
	            rs = pip->setent(vpp,nbuf,nl) ;
	        } /* end */
		rs1 = mem.free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    nl = lenstr(pip->nodename) ;
	} /* end */
	return (rs >= 0) ? nl : rs ;
} /* end subroutine (proginfo_nodename) */


/* local subroutines */

#ifdef	COMMENT
local int proginfo_setdefnames(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->progname == nullptr) {
	    if ((rs = proginfo_progename(pip)) >= 0) {
	        if (pip->progename != nullptr) {
	            rs = proginfo_setprogname(pip,pip->progename) ;
	        }
	    }
	} /* end if */
	return rs ;
} /* end subroutine (proginfo_setdefnames) */
#else /* COMMENT */
local int proginfo_setdefnames(PI *pip) noex {
	int		rs = SR_OK ;
	(void) pip ;
	return rs ;
} /* end subroutine (proginfo_setdefnames) */
#endif /* COMMENT */

#ifdef	COMMENT
/* Set Default (program) Directory-Name */
local int proginfo_setdefdn(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->progdname == nullptr) {
	    int		cl ;
	    cchar	*cp ;
	    if ((rs >= 0) && (pip->progename != nullptr)) {
	        if ((cl = sfdirname(pip->progename,-1,&cp)) > 0) {
	            cchar	**vpp = &pip->progdname ;
	            rs = pip->setent(vpp,cp,cl) ;
	        }
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (proginfo_setdefdn) */
#endif /* COMMENT */

#ifdef	COMMENT
/* Set Default Program-Name */
local int proginfo_setdefpn(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->progname == nullptr) {
	    int		cl ;
	    cchar	*cp ;
	    if (pip->progename != nullptr) {
	        if ((cl = sfbasename(pip->progename,-1,&cp)) > 0) {
	            cl = rmext(cp,cl) ;
	            if (cl == 0) {
	                cp = NOPROGNAME ;
	                cl = -1 ;
	            }
	        }
	        if ((cp != nullptr) && (cl != 0)) {
	            cchar	**vpp = &pip->progdname ;
	            rs = pip->setent(vpp,cp,cl) ;
	        }
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (proginfo_setdefpn) */
#endif /* COMMENT */

proginfo_vals::proginfo_vals() noex {
	memclear(this) ;
} /* end */

void proginfo_hwser::operator () (PI *p) noex {
	op = p ;
	val = 0 ;
} /* end */

proginfo_hwser::operator uint () noex {
	if (val == 0) {
	    val = (uint) gethostid() ; /* natively returns |long| */
	} /* end if (needed) */
	return val ;
} /* end */

proginfo::proginfo(int ac,con mainv av,con mainv ev) noex {
    	DEBUGPRINTF("ent\n") ;
    	prs		= 0 ; /* previous-return-status */
	argc		= ac ;
	argv		= av ;
	envv		= ev ;
	pwdbuf		= nullptr ;
	sdp		= nullptr ;
	idp		= nullptr ;
	lfp		= nullptr ;
	now		= {} ;
	pwdlen		= 0 ;
	pm		= -1 ;
	eprint.init	(this,0) ;
	start		(this) ;
	hwserial	(this) ;
	ncpu		(this,proginfomem_ncpu) ;
	finish		(this,proginfomem_finish) ;
	rootdname	(this,proginfomem_rootdname) ;
	progname	(this,proginfomem_progname) ;
	progename	(this,proginfomem_progename) ;
	progdname	(this,proginfomem_progdname) ;
	rootdname	(this,proginfomem_rootdname) ;
    	DEBUGPRINTF("ret\n") ;
} /* end ctor (proginfo::proginfo) */

void proginfo::args(int ac,mainv av,mainv ev) noex {
	argc = ac ;
	argv = av ;
	envv = ev ;
} /* end method (proginfo::args) */

int proginfo::progmode(mainv names) noex {
    	int		rs = SR_FAULT ;
	if (names) ylikely {
	    if (ccharp pn = progname) ylikely {
	        if (int si = matstr(names,pn) ; si >= 0) ylikely {
		    pm = si ;
		    rs = si ;
		} else {
		    rs = SR_NOENT ;
		} /* end if (got match) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
    	return rs ;
} /* end method (proginfo::progmode) */

int proginfo_start::operator () (int ac,mainv av,mainv ev,ccp v) noex {
    	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("proginfo_start-args-ent\n") ;
	if (op) {
	    rs = op->pstart(ac,av,ev,v) ;
	} /* end if (non-null) */
	DEBUGPRINTF("proginfo_start-args ret rs=%d\n",rs) ;
	return rs ;
} /* end method (proginfo_start::operator) */

int proginfo_start::operator () (mainv names) noex {
    	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("proginfo_start-mv-ent\n") ;
	if (op) ylikely {
	    if ((rs = op->pstart(-1,np,np,np)) >= 0) ylikely {
		if (names) {
	            rs = op->progmode(names) ;
		}
	    }
	} /* end if (non-null) */
	DEBUGPRINTF("proginfo_start-mv-ret rs=%d\n",rs) ;
	return rs ;
} /* end method (proginfo_start::operator) */

int proginfo_co::operator () (int) noex {
	int		rs = SR_BADFMT ;
	if (op) ylikely {
	    switch (w) {
	    case proginfomem_ncpu:
		rs = op->pncpu() ;
		break ;
	    case proginfomem_finish:
		rs = op->pfinish() ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (proginfo_co::operator) */

proginfo_ccharp::operator ccharp () noex {
    	ccharp	rp = nullptr ;
	switch (w) {
	case proginfomem_progname:
	    rp = char_progname(op) ;
	    break ;
	case proginfomem_progename:
	    rp = char_progename(op) ;
	    break ;
	case proginfomem_progdname:
	    rp = char_progdname(op) ;
	    break ;
	case proginfomem_rootdname:
	    rp = char_rootdname(op) ;
	    break ;
	} /* end switch */
	return rp ;
} /* end method (proginfo_ccharp::operator) */

local int proginfo_hargz(PI *pip) noex {
    	int		rs = SR_OK ;
	if ((pip->argc > 0) && pip->argv) {
	    pip->argz = pip->argv[0] ;
	} /* end if */
	return rs ;
} /* end subroutine (proginfo_hargz) */

local int proginfo_hver(PI *pip,cchar *v) noex {
    	int		rs = SR_OK ;
	if (v) ylikely {
	    ccharpp vpp = &pip->version ;
	    rs = pip->setent(vpp,v) ;
	} /* end */
	return rs ;
} /* end subroutine (proginfo_hver) */

local ccharp char_progname(PI *pip) noex {
    	ccharp		rp ;
	if ((rp = pip->progname.valp) == nullptr) {
	    if (int rs = proginfo_progpname(pip) ; rs >= 0) {
	        rp = pip->progname.valp ;
	        pip->prs = rs ;
	    } /* end if (proginfo_progpname) */
	} /* end if (needed) */
	return rp ;
} /* end subroutine (char_progname) */

local ccharp char_progename(PI *pip) noex {
    	ccharp		rp ;
	if ((rp = pip->progename.valp) == nullptr) {
	    if (int rs = proginfo_progename(pip) ; rs >= 0) {
	        rp = pip->progename.valp ;
	        pip->prs = rs ;
	    } /* end if (proginfo_progename) */
	} /* end if (needed) */
	return rp ;
} /* end subroutine (char_progename) */

local ccharp char_progdname(PI *pip) noex {
    	ccharp		rp ;
	if ((rp = pip->progdname.valp) == nullptr) {
	    if (int rs = proginfo_progdname(pip) ; rs >= 0) {
	        rp = pip->progdname.valp ;
	        pip->prs = rs ;
	    } /* end if (proginfo_progdname) */
	} /* end if (needed) */
	return rp ;
} /* end subroutine (char_progdname) */

local ccharp char_rootdname(PI *pip) noex {
    	ccharp		rp ;
	if ((rp = pip->rootdname.valp) == nullptr) {
	    if (int rs = proginfo_rootdname(pip) ; rs >= 0) {
	        rp = pip->rootdname.valp ;
	        pip->prs = rs ;
	    } /* end if (proginfo_rootdname) */
	} /* end if (needed) */
	return rp ;
} /* end subroutine (char_rootdname) */

local bool hasourbad(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    if (! (f = hasprintbad(sp,sl))) {
	        if (sl < 0) sl = lenstr(sp) ;
	        for (int i = 0 ; i < sl ; i += 1) {
	            cint sch = mkchar(sp[i]) ;
	            f = (sch >= 128) ;
	            if (f) break ;
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hadourbad) */



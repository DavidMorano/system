/* mkpr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make program-root */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkpr

	Description:
	This subroutine finds a program-root directory given a
	software distribution name.  This subroutine is generally
	meant to be used by programs that are not themselves part
	of the facility that this subroutine is trying to get the
	program-root for.

	Synopsis:
	int mkpr(char *pbuf,int plen,cc *prname,cc *domain) noex

	Arguments:
	pbuf		supplied buffer to receive the resulting directory
	plen		supplied length of buffer
	prname		the name of the software distribution to lookup
	domain		domain-name

	Returns:
	>=0		length of resulting directory path
	<0		could not find the program-root directory

	Implementation note:
	We use the subroutine |dirsearch()| (below) to search the
	'basenames' directories since they may be indirect automount
	points and we do not want to get those stupid SYSLOG messages
	blabbing on about some certain name in the automount map
	not being available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<pwd.h>			/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getuserhome.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<fsdir.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<sncpyxc.h>		/* LIBUC */
#include	<snwcpyxc.h>		/* LIBUC */
#include	<nleadstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkpathxw.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mkpr.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	STACKBUFLEN	64

#ifndef	SWDFNAME
#define	SWDFNAME	".swd"
#endif

#define	DMODE		(X_OK | R_OK)

#define	HOMEBASEDNAME	"add-on"

#define	PRNAME		"LOCAL"

#define	SI		subinfo
#define	SI_FL		si_flags


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_stat(cchar *,ustat *) noex ;
}


/* external variables */


/* local structures */

struct si_flags {
	uint		ids:1 ;
	uint		dname:1 ;
} ; /* end struct */

struct subinfo {
	cchar		*prname ;	/* program-root-name */
	cchar		*domain ;	/* domain */
	cchar		*dname ;	/* directory-name */
	ids		id ;
	SI_FL		init, open ;
} ; /* end struct */

typedef int		(*si_f)(SI *,char *,int) noex ;

struct prmap {
	cchar		*prname ;
	cchar		*dname ;
} ; /* end struct */

struct domainbase {
	cchar		*domain ;
	cchar		*basedname ;
} ; /* end struct */


/* forward references */

local int getpr(const si_f *,char *,int,cchar *,cchar *) noex ;

local int	si_start(SI *,cchar *,cchar *) noex ;
local int	si_checkid(SI *) noex ;
local int	si_dirok(SI *,cchar *,mode_t) noex ;
local int	si_finish(SI *) noex ;

local int	si_env(SI *,char *,int) noex ;
local int	si_enver(SI *,char *,int,cc *) noex ;
local int	si_domain(SI *,char *,int) noex ;
local int	si_domainer(SI *,char *,int,cc *,int) noex ;
local int	si_user(SI *,char *,int) noex ;
local int	si_users(SI *,char *,int,cc *,cc *) noex ;
local int	si_prmap(SI *,char *,int) noex ;
local int	si_home(SI *,char *,int) noex ;
local int	si_homer(SI *,char *,int,cchar *) noex ;
local int	si_bases(SI *,char *,int) noex ;

local int	dirsearch(cchar *,cchar *) noex ;


/* local variables */

constexpr si_f		gettries[] = {
	si_domain,
	si_user,
	si_prmap,
	si_home,
	nullptr
} ; /* end array */

constexpr si_f		mktries[] = {
	si_env,
	si_domain,
	si_user,
	si_prmap,
	si_home,
	si_bases,
	nullptr
} ; /* end array */

constexpr prmap		prmaps[] = {
	{ "root", 			"/" },
	{ "extra",		 	"/usr/extra" },
	{ "usrlocal",		 	"/usr/local" },
	{ "usr",			"/usr" },
	{ "xpg4",			"/usr/xpg4" },
	{ "xpg6", 			"/usr/xpg6" },
	{ "dt",				"/usr/dt" },
	{ "ccs",			"/usr/ccs" },
	{ "openwin", 			"/usr/openwin" },
	{ "java",			"/usr/java" },
	{ "preroot",			"/usr/preroot" },
	{ "apache",			"/usr/apache" },
	{ "postfix",			"/usr/postfix" },
	{ nullptr,			nullptr }
} ; /* end array */

constexpr domainbase	domains[] = {
	{ "rightcore.com", 		"/opt" },
	{ "rightcore.org", 		"/opt" },
	{ "morano.ws", 			"/opt" },
	{ nullptr, 			nullptr }
} ; /* end array */

constexpr cpcchar	basednames[] = {
	"/opt",
	"/usr",
	"/opt",
	nullptr
} ; /* end array */

static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int getrootdname(char *rbuf,int rlen,cchar *prname,cchar *domain) noex {
    	return getpr(gettries,rbuf,rlen,prname,domain) ;
}

int mkpr(char *rbuf,int rlen,cchar *prname,cchar *domain) noex {
    	return getpr(mktries,rbuf,rlen,prname,domain) ;
}


/* local subroutines */

local int getpr(const si_f *tries,char *rb,int rl,cchar *prn,cchar *dom) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rb && prn) ylikely {
	    rb[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) ylikely {
	        if (rl < 0) rl = rs ;
	        if (prn[0] == '\0') prn = PRNAME ;
	        if (SI si ; (rs = si_start(&si,prn,dom)) >= 0) ylikely {
		    {
	                rs = SR_NOTDIR ;
	                for (int i = 0 ; tries[i] ; i += 1) {
	                    rs = (*tries[i])(&si,rb,rl) ;
	                    if (rs != 0) break ;
	                } /* end for */
	                len = rs ;
		    }
	            rs1 = si_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	        if ((rs >= 0) && (len == 0)) rs = SR_NOTFOUND ;
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getrootdname) */

local int si_start(SI *sip,cchar *prname,cchar *domain) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	memclear(sip) ;			/* <- noted */
	sip->prname = prname ;
	sip->domain = domain ;
	sip->dname = prname ;
        if (hasuc(prname,-1)) { /* while keeping stack mostly shallow */
	    if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) ylikely {
                cint	dlen = rs ;
                if ((rs = sncpylc(dbuf,dlen,prname)) >= 0) ylikely {
		    cchar	*cp{} ; 
                    if ((rs = libmem.strw(dbuf,rs,&cp)) >= 0) ylikely {
                        sip->open.dname = true ;
                        sip->dname = cp ;
                    } /* end if (memory-acquire) */
                } /* end if (sncpylc) */
	        rs1 = lm_free(dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
        } /* end if (had some upper-case) */
	return rs ;
} /* end subroutine (si_start) */
 
local int si_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->open.ids) {
	    sip->open.ids = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
        if (sip->open.dname && sip->dname) {
            sip->open.dname = false ;
	    void *vp = voidp(sip->dname) ;
            rs1 = libmem.free(vp) ;
            if (rs >= 0) rs = rs1 ;
            sip->dname = nullptr ;
        }
	return rs ;
} /* end subroutine (si_finish) */

local int si_checkid(SI *sip) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! sip->init.ids) {
	    sip->init.ids = true ;
	    if ((rs = ids_load(&sip->id)) >= 0) {
	        sip->open.ids = true ;
	    }
	    f = sip->open.ids ;
	} /* end if */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (si_checkid) */

local int si_dirok(SI *sip,cchar *dname,mode_t dm) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) ylikely {
	    if ((rs = si_checkid(sip)) >= 0) ylikely {
	        rs = SR_NOTDIR ;
	        if (S_ISDIR(sb.st_mode) && sip->open.ids) {
	            rs = permid(&sip->id,&sb,dm) ;
		}
	    }
	} /* end if (uc_stat) */
	return rs ;
} /* end subroutine (si_dirok) */

local int si_env(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cchar		*envp = sip->prname ;
	if (haslc(envp,-1)) {
	    if (char *ebuf ; (rs = lm_mp(&ebuf)) >= 0) ylikely {
		cint	elen = rs ;
	        if ((rs = sncpyuc(ebuf,elen,envp)) >= 0) ylikely {
	    	    rs = si_enver(sip,rbuf,rlen,ebuf) ;
		    len = rs ;
		}
		rs1 = lm_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = si_enver(sip,rbuf,rlen,envp) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_env) */

local int si_enver(SI *sip,char *rbuf,int rlen,cc *envp) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (envp[0] != '\0') ylikely {
	    if (cchar *cp ; (cp = getenv(envp)) != nullptr) {
		if ((rs = sncpy(rbuf,rlen,cp)) >= 0) ylikely {
		    len = rs ;
		    rs = si_dirok(sip,rbuf,DMODE) ;
		}
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_enver) */

local int si_domain(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (sip->domain == nullptr) {
	    sip->domain = getenv(varname.domain) ;
	}
	if (sip->domain && sip->domain[0]) ylikely {
	    int		dnl = lenstr(sip->domain) ;
	    cchar	*dnp = sip->domain ;
	    while ((dnl > 0) && (dnp[dnl-1] == '.')) {
		dnl -= 1 ;
	    }
	    if (dnl > 0) ylikely {
	        if (hasuc(dnp,dnl)) {
		    if (dnl <= STACKBUFLEN) {
			cint	dlen = dnl ;
			char	dbuf[dnl+1] ;
	                if ((rs = snwcpylc(dbuf,dlen,dnp,dnl)) > 0) {
		            rs = si_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                }
		    } else {
		        if (char *dbuf ; (rs = lm_hostname(&dbuf)) >= 0) {
		            cint	dlen = rs ;
	                    if ((rs = snwcpylc(dbuf,dlen,dnp,dnl)) > 0) {
		                rs = si_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                    }
		            rs1 = lm_free(dbuf) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
		    } /* end if (small-string-optimization) */
	        } else {
		    rs = si_domainer(sip,rbuf,rlen,dnp,dnl) ;
	        }
	    } /* end if (still positive) */
	} /* end if (have a domain to lookup) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_domain) */

local int si_domainer(SI *sip,char *rbuf,int rlen,cc *dnp,int dnl) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	int		i ; /* used-afterwards */
	cchar		*bnp ;
	for (i = 0 ; domains[i].domain ; i += 1) {
	    cchar	*dn = domains[i].domain ;
	    int		m ;
	    if ((m = nleadstr(dn,dnp,dnl)) > 0) {
	        if ((dn[m] == '\0') && (m == dnl)) {
	            break ;
		}
	    }
	} /* end for */
	bnp = domains[i].basedname ;
	if (domains[i].domain && bnp) ylikely {
	    if ((rs = mknpath2(rbuf,rlen,bnp,sip->dname)) >= 0) ylikely {
	        len = rs ;
	        rs = si_dirok(sip,rbuf,DMODE) ;
	    }
	} /* end if (got a domain match) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_domainer) */

local int si_user(SI *sip,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	    cint	pwlen = rs ;
	    cchar	*dn = sip->dname ;
	    if (ucentpwx pw ; (rs = pw.nam(pwbuf,pwlen,dn)) >= 0) ylikely {
	        if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
		    cchar	*d = pw.pw_dir ;
		    cchar	swd[] = SWDFNAME ;
	            if ((rs = mkpath(tbuf,d,swd)) >= 0) ylikely {
			rs = si_users(sip,rbuf,rlen,tbuf,d) ;
			len = rs ;
		    }
		    rs1 = lm_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getpwx_name) */
	    rs1 = lm_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_user) */

local int si_users(SI *sip,char *rbuf,int rlen,cc *sym,cc *d) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (ustat sb ; (rs = u_lstat(sym,&sb)) >= 0) ylikely {
            if (S_ISLNK(sb.st_mode)) ylikely {
                if ((rs = u_readlink(sym,rbuf,rlen)) >= 0) ylikely {
                    cint     bl = rs ;
                    len = rs ;
                    rbuf[bl] = '\0' ;
                    if ((bl == 1) && (rbuf[0] == '.')) {
                        rs = mknpath1(rbuf,rlen,d) ;
                        len = rs ;
                    } else if ((bl > 0) && (rbuf[0] != '/')) {
                        if (char *tmore ; (rs = lm_mp(&tmore)) >= 0) {
                            if ((rs = mkpath1(tmore,rbuf)) >= 0) {
                                rs = mknpath2(rbuf,rlen,d,tmore) ;
                                len = rs ;
			    }
			    rs1 = lm_free(tmore) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
                    } /* end if */
                    if ((rs >= 0) && (len > 0)) ylikely {
                        rs = si_dirok(sip,rbuf,DMODE) ;
                    }
                } /* end if (read link) */
            } /* end if (symbolic link) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
        } /* end if (have SWD directory entry) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_users) */

local int si_prmap(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* used-afterwards */
	int		len = 0 ; /* return-value */
	for (i = 0 ; prmaps[i].prname ; i += 1) {
	    cchar	*pn = prmaps[i].prname ;
	    if (int m ; (m = nleadstr(pn,sip->dname,-1)) > 0) {
	        if ((pn[m] == '\0') && (sip->dname[m] == '\0')) {
	            break ;
		}
	    }
	} /* end for */
	if (prmaps[i].prname) ylikely {
	    if ((rs = sncpy1(rbuf,rlen,prmaps[i].dname)) >= 0) {
	        len = rs ;
	        rs = si_dirok(sip,rbuf,DMODE) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_prmap) */

local int si_home(SI *sip,char *rbuf,int rlen) noex {
    	static cchar	*vhome = getenv(varname.home) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (vhome) ylikely {
	    rs = si_homer(sip,rbuf,rlen,vhome) ;
	    len = rs ;
	} else {
	    if (char *hbuf ; (rs = lm_mp(&hbuf)) >= 0) ylikely {
		cint	hlen = rs ;
	        if ((rs = getuserhome(hbuf,hlen,"-")) >= 0) ylikely {
	    	    rs = si_homer(sip,rbuf,rlen,hbuf) ;
	    	    len = rs ;
	        } /* end if (getuserhome) */
		rs1 = lm_free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_home) */

local int si_homer(SI *sip,char *rbuf,int rlen,cchar *hn) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if (hn && hn[0]) ylikely {
	    cchar	*dn = sip->dname ;
	    cchar	*hdname = HOMEBASEDNAME ;
	    if ((rs = mknpath3(rbuf,rlen,hn,hdname,dn)) >= 0) ylikely {
	        len = rs ;
	        rs = si_dirok(sip,rbuf,DMODE) ;
	    }
	} /* end if (got a HOME directory) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_homer) */

local int si_bases(SI *sip,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (char *tmpfname ; (rs = lm_mp(&tmpfname)) >= 0) ylikely {
	    for (int i = 0 ; basednames[i] ; i += 1) {
	        cchar	*bn = basednames[i] ;
	        if ((rs = dirsearch(bn,sip->dname)) > 0) ylikely {
	            if ((rs = mkpath2(tmpfname,bn,sip->dname)) >= 0) ylikely {
		        cint	dm = DMODE ;
		        if ((rs = si_dirok(sip,tmpfname,dm)) >= 0) {
	                    rs = sncpy1(rbuf,rlen,tmpfname) ;
			    len = rs ;
		        }
	            }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	        if (rs > 0) break ;
	    } /* end for */
	    rs1 = lm_free(tmpfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_bases) */

local int dirsearch(cchar *basedname,cchar *username) noex {
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
	    cint	nlen = rs ;
	    if (fsdir dir ; (rs = fsdir_open(&dir,basedname)) >= 0) ylikely {
	        fsdir_ent	ds ;
	        while ((rs = fsdir_read(&dir,&ds,nbuf,nlen)) > 0) {
		    cchar	*fnp = ds.name ;
		    if (fnp[0] != '.') {
		        f_found = (strcmp(fnp,username) == 0) ;
		        if (f_found) break ;
		    }
	        } /* end while */
	        rs1 = fsdir_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f_found : rs ;
} /* end subroutine (dirsearch) */



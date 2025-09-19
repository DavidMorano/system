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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<pwd.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getpwx.h>
#include	<getuserhome.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<ids.h>
#include	<fsdir.h>
#include	<sncpyx.h>
#include	<sncpyxc.h>
#include	<snwcpyxc.h>
#include	<nleadstr.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<permx.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mkpr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	STACKBUFLEN	64

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"		/* program-root */
#endif

#ifndef	VARPREXTRA
#define	VARPREXTRA	"EXTRA"		/* program-root */
#endif

#ifndef	SWDFNAME
#define	SWDFNAME	".swd"
#endif

#define	DMODE		(X_OK | R_OK)

#define	HOMEBASEDNAME	"add-on"

#define	PRNAME		"LOCAL"

#define	SI		struct subinfo
#define	SI_FL		struct subinfo_flags


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		ids:1 ;
	uint		dname:1 ;
} ;

struct subinfo {
	ids		id ;
	cchar		*prname ;	/* program-root-name */
	cchar		*domain ;	/* domain */
	cchar		*dname ;	/* directory-name */
	SI_FL		init, open ;
} ;

typedef int		(*subinfo_f)(SI *,char *,int) ;

struct prmap {
	cchar		*prname ;
	cchar		*dname ;
} ;

struct domainbase {
	cchar		*domain ;
	cchar		*basedname ;
} ;


/* forward references */

static int	subinfo_start(SI *,cchar *,cchar *) noex ;
static int	subinfo_checkid(SI *) noex ;
static int	subinfo_dirok(SI *,cchar *,mode_t) noex ;
static int	subinfo_finish(SI *) noex ;

static int	subinfo_env(SI *,char *,int) noex ;
static int	subinfo_enver(SI *,char *,int,cc *) noex ;
static int	subinfo_domain(SI *,char *,int) noex ;
static int	subinfo_domainer(SI *,char *,int,cc *,int) noex ;
static int	subinfo_user(SI *,char *,int) noex ;
static int	subinfo_users(SI *,char *,int,cc *,cc *) noex ;
static int	subinfo_prmap(SI *,char *,int) noex ;
static int	subinfo_home(SI *,char *,int) noex ;
static int	subinfo_homer(SI *,char *,int,cchar *) noex ;
static int	subinfo_bases(SI *,char *,int) noex ;

static int	dirsearch(const char *,const char *) noex ;


/* local variables */

constexpr subinfo_f		gettries[] = {
	subinfo_domain,
	subinfo_user,
	subinfo_prmap,
	subinfo_home,
	nullptr
} ;

constexpr subinfo_f		mktries[] = {
	subinfo_env,
	subinfo_domain,
	subinfo_user,
	subinfo_prmap,
	subinfo_home,
	subinfo_bases,
	nullptr
} ;

constexpr prmap			prmaps[] = {
	{ "root", "/" },
	{ "extra", "/usr/extra" },
	{ "usrlocal", "/usr/local" },
	{ "usr", "/usr" },
	{ "xpg4", "/usr/xpg4" },
	{ "xpg6", "/usr/xpg6" },
	{ "dt", "/usr/dt" },
	{ "ccs", "/usr/ccs" },
	{ "openwin", "/usr/openwin" },
	{ "java", "/usr/java" },
	{ "preroot", "/usr/preroot" },
	{ "apache", "/usr/apache" },
	{ "postfix", "/usr/postfix" },
	{ nullptr, nullptr }
} ;

constexpr domainbase		domains[] = {
	{ "rightcore.com", "/usr/add-on" },
	{ "rightcore.org", "/usr/add-on" },
	{ "ece.neu.edu", "/home/student/dmorano/add-on" },
	{ "clearresearch.org", "/usr/add-on" },
	{ "christian-update.org", "/usr/add-on" },
	{ "morano.ws", "/usr/add-on" },
	{ "custombibles.online", "/usr/add-on" },
	{ "bespokebibles.com", "/usr/add-on" },
	{ nullptr, nullptr }
} ;

constexpr cpcchar		basednames[] = {
	"/usr/add-on",
	"/usr",
	"/opt",
	nullptr
} ;

static bufsizevar		maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int getrootdname(char *rbuf,int rlen,cchar *prname,cchar *domain) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && prname) ylikely {
	    rbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) ylikely {
	        SI	si ;
	        if (rlen < 0) rlen = rs ;
	        if (prname[0] == '\0') prname = PRNAME ;
	        if ((rs = subinfo_start(&si,prname,domain)) >= 0) ylikely {
	            rs = SR_NOTDIR ;
	            for (int i = 0 ; gettries[i] ; i += 1) {
	                rs = (*gettries[i])(&si,rbuf,rlen) ;
	                rl = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            if (rs == 0) rs = SR_NOTFOUND ;
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (getrootdname) */

int mkpr(char *pbuf,int plen,cchar *prname,cchar *domain) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pbuf && prname) ylikely {
	    rs = SR_INVALID ;
	    pbuf[0] = '\0' ;
	    if (prname[0]) ylikely {
	        if (SI ti ; (rs = subinfo_start(&ti,prname,domain)) >= 0) {
	            rs = SR_NOTFOUND ;
	            for (int i = 0 ; mktries[i] ; i += 1) {
	                rs = (*mktries[i])(&ti,pbuf,plen) ;
		        rl = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            if (rs == 0) rs = SR_NOTFOUND ;
	            rs1 = subinfo_finish(&ti) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkpr) */


/* local subroutines */

static int subinfo_start(SI *sip,cchar *prname,cchar *domain) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	memclear(sip) ;			/* <- noted */
	sip->prname = prname ;
	sip->domain = domain ;
	sip->dname = prname ;
        if (hasuc(prname,-1)) { /* while keeping stack mostly shallow */
	    if (char *dbuf ; (rs = malloc_mp(&dbuf)) >= 0) ylikely {
                cint	dlen = rs ;
                if ((rs = sncpylc(dbuf,dlen,prname)) >= 0) ylikely {
		    cchar	*cp{} ; 
                    if ((rs = libmem.strw(dbuf,rs,&cp)) >= 0) ylikely {
                        sip->open.dname = true ;
                        sip->dname = cp ;
                    } /* end if (memory-allocation) */
                } /* end if (sncpylc) */
	        rs1 = malloc_free(dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
        } /* end if (had some upper-case) */
	return rs ;
}
/* end subroutine (subinfo_start) */
 
static int subinfo_finish(SI *sip) noex {
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
}
/* end subroutine (subinfo_finish) */

static int subinfo_checkid(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->init.ids) {
	    sip->init.ids = true ;
	    if ((rs = ids_load(&sip->id)) >= 0) {
	        sip->open.ids = true ;
	    }
	}
	return (rs >= 0) ? sip->open.ids : rs ;
}
/* end subroutine (subinfo_checkid) */

static int subinfo_dirok(SI *sip,cchar *dname,mode_t dm) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) ylikely {
	    if ((rs = subinfo_checkid(sip)) >= 0) ylikely {
	        rs = SR_NOTDIR ;
	        if (S_ISDIR(sb.st_mode) && sip->open.ids) {
	            rs = permid(&sip->id,&sb,dm) ;
		}
	    }
	} /* end if (uc_stat) */
	return rs ;
}
/* end subroutine (subinfo_dirok) */

static int subinfo_env(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cchar		*envp = sip->prname ;
	if (haslc(envp,-1)) {
	    if (char *ebuf ; (rs = malloc_mp(&ebuf)) >= 0) ylikely {
		cint	elen = rs ;
	        if ((rs = sncpyuc(ebuf,elen,envp)) >= 0) ylikely {
	    	    rs = subinfo_enver(sip,rbuf,rlen,ebuf) ;
		    len = rs ;
		}
		rs1 = malloc_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = subinfo_enver(sip,rbuf,rlen,envp) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_env) */

static int subinfo_enver(SI *sip,char *rbuf,int rlen,cc *envp) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (envp[0] != '\0') ylikely {
	    if (cchar *cp ; (cp = getenv(envp)) != nullptr) {
		if ((rs = sncpy(rbuf,rlen,cp)) >= 0) ylikely {
		    len = rs ;
		    rs = subinfo_dirok(sip,rbuf,DMODE) ;
		}
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_enver) */

static int subinfo_domain(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (sip->domain == nullptr) {
	    cchar	*vn = varname.domain ;
	    sip->domain = getenv(vn) ;
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
		            rs = subinfo_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                }
		    } else {
		        if (char *dbuf ; (rs = malloc_hn(&dbuf)) >= 0) {
		            cint	dlen = rs ;
	                    if ((rs = snwcpylc(dbuf,dlen,dnp,dnl)) > 0) {
		                rs = subinfo_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                    }
		            rs1 = malloc_free(dbuf) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
		    } /* end if (small-string-optimization) */
	        } else {
		    rs = subinfo_domainer(sip,rbuf,rlen,dnp,dnl) ;
	        }
	    } /* end if (still positive) */
	} /* end if (have a domain to lookup) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_domain) */

static int subinfo_domainer(SI *sip,char *rbuf,int rlen,cc *dnp,int dnl) noex {
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
	        rs = subinfo_dirok(sip,rbuf,DMODE) ;
	    }
	} /* end if (got a domain match) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_domainer) */

static int subinfo_user(SI *sip,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) ylikely {
	    cint	pwlen = rs ;
	    cchar	*dn = sip->dname ;
	    if (ucentpwx pw ; (rs = pw.nam(pwbuf,pwlen,dn)) >= 0) ylikely {
	        if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) ylikely {
		    cchar	*d = pw.pw_dir ;
		    cchar	swd[] = SWDFNAME ;
	            if ((rs = mkpath(tbuf,d,swd)) >= 0) ylikely {
			rs = subinfo_users(sip,rbuf,rlen,tbuf,d) ;
			len = rs ;
		    }
		    rs1 = malloc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getpwx_name) */
	    rs1 = malloc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_user) */

static int subinfo_users(SI *sip,char *rbuf,int rlen,cc *sym,cc *d) noex {
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
                        if (char *tmore ; (rs = malloc_mp(&tmore)) >= 0) {
                            if ((rs = mkpath1(tmore,rbuf)) >= 0) {
                                rs = mknpath2(rbuf,rlen,d,tmore) ;
                                len = rs ;
			    }
			    rs1 = malloc_free(tmore) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
                    } /* end if */
                    if ((rs >= 0) && (len > 0)) ylikely {
                        rs = subinfo_dirok(sip,rbuf,DMODE) ;
                    }
                } /* end if (read link) */
            } /* end if (symbolic link) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
        } /* end if (have SWD directory entry) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_users) */

static int subinfo_prmap(SI *sip,char *rbuf,int rlen) noex {
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
	        rs = subinfo_dirok(sip,rbuf,DMODE) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_prmap) */

static int subinfo_home(SI *sip,char *rbuf,int rlen) noex {
    	static cchar	*hn = getenv(VARHOME) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (hn) ylikely {
	    rs = subinfo_homer(sip,rbuf,rlen,hn) ;
	    len = rs ;
	} else {
	    if (char *hbuf ; (rs = malloc_mp(&hbuf)) >= 0) ylikely {
		cint	hlen = rs ;
	        if ((rs = getuserhome(hbuf,hlen,"-")) >= 0) ylikely {
	    	    rs = subinfo_homer(sip,rbuf,rlen,hbuf) ;
	    	    len = rs ;
	        }
		rs1 = malloc_free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_home) */

static int subinfo_homer(SI *sip,char *rbuf,int rlen,cchar *hn) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (hn && hn[0]) ylikely {
	    cchar	*dn = sip->dname ;
	    cchar	*hdname = HOMEBASEDNAME ;
	    if ((rs = mknpath3(rbuf,rlen,hn,hdname,dn)) >= 0) ylikely {
	        len = rs ;
	        rs = subinfo_dirok(sip,rbuf,DMODE) ;
	    }
	} /* end if (got a HOME directory) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_homer) */

static int subinfo_bases(SI *sip,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *tmpfname ; (rs = malloc_mp(&tmpfname)) >= 0) ylikely {
	    for (int i = 0 ; basednames[i] ; i += 1) {
	        cchar	*bn = basednames[i] ;
	        if ((rs = dirsearch(bn,sip->dname)) > 0) ylikely {
	            if ((rs = mkpath2(tmpfname,bn,sip->dname)) >= 0) ylikely {
		        cint	dm = DMODE ;
		        if ((rs = subinfo_dirok(sip,tmpfname,dm)) >= 0) {
	                    rs = sncpy1(rbuf,rlen,tmpfname) ;
			    len = rs ;
		        }
	            }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	        if (rs > 0) break ;
	    } /* end for */
	    rs1 = malloc_free(tmpfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_bases) */

static int dirsearch(cchar *basedname,cchar *username) noex {
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) ylikely {
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
	    rs1 = malloc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (dirsearch) */



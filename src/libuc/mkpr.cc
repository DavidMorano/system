/* mkpr SUPPORT */
/* lang=C++20 */

/* make program-root */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	0		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.  

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
	points and we don't want to get those stupid SYSLOG messages
	blabbing on about some certain name in the automount map
	not being available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<pwd.h>
#include	<usystem.h>
#include	<varnames.hh>
#include	<getbufsize.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<ids.h>
#include	<getax.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getusername.h>
#include	<fsdir.h>
#include	<localmisc.h>
#include	<sncpyx.h>
#include	<sncpyxc.h>
#include	<snwcpyxc.h>
#include	<nleadstr.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<hasx.h>


/* local defines */

#define	STACKBUFLEN	64

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */

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


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	sperm(IDS *,USTAT *,int) noex ;
    extern int	getuserhome(char *,int,cchar *) noex ;
    extern int	isNotPresent(int) noex ;
}


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

struct prmap {
	cchar		*prname ;
	cchar		*dname ;
} ;

struct domainbase {
	cchar		*domain ;
	cchar		*basedname ;
} ;


/* forward references */

extern "C" {
    int mkpr(char *,int,cchar *,cchar *) noex ;
    int getrootdname(char *,int,cchar *,cchar *) noex ;
}

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

static constexpr int	(*gettries[])(SI *,char *,int) = {
	subinfo_domain,
	subinfo_user,
	subinfo_prmap,
	subinfo_home,
	nullptr
} ;

static constexpr int	(*mktries[])(SI *,char *,int) = {
	subinfo_env,
	subinfo_domain,
	subinfo_user,
	subinfo_prmap,
	subinfo_home,
	subinfo_bases,
	nullptr
} ;

static constexpr struct prmap	prmaps[] = {
	{ "root", "/" },
	{ "usr", "/usr" },
	{ "xpg4", "/usr/xpg4" },
	{ "xpg6", "/usr/xpg6" },
	{ "dt", "/usr/dt" },
	{ "ccs", "/usr/ccs" },
	{ "openwin", "/usr/openwin" },
	{ "java", "/usr/java" },
	{ "extra", "/usr/extra" },
	{ "preroot", "/usr/preroot" },
	{ "apache", "/usr/apache" },
	{ "postfix", "/usr/postfix" },
	{ nullptr, nullptr }
} ;

static constexpr struct domainbase	domains[] = {
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

static constexpr cchar		*basednames[] = {
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
	if (rbuf && prname) {
	    rbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) {
	        SI	si ;
	        if (rlen < 0) rlen = rs ;
	        if (prname[0] == '\0') prname = PRNAME ;
	        if ((rs = subinfo_start(&si,prname,domain)) >= 0) {
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
	if (pbuf && prname) {
	    rs = SR_INVALID ;
	    pbuf[0] = '\0' ;
	    if (prname[0]) {
	        SI	ti ;
	        if ((rs = subinfo_start(&ti,prname,domain)) >= 0) {
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
	    char	*dbuf{} ;
	    if ((rs = malloc_mp(&dbuf)) >= 0) {
                cint	dlen = rs ;
                if ((rs = sncpylc(dbuf,dlen,prname)) >= 0) {
		    cchar	*cp{} ; 
                    if ((rs = uc_mallocstrw(dbuf,rs,&cp)) >= 0) {
                        sip->open.dname = true ;
                        sip->dname = cp ;
                    } /* end if (m-a) */
                } /* end if (sncpylc) */
	        rs1 = uc_free(dbuf) ;
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
            rs1 = uc_free(sip->dname) ;
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
	USTAT		sb ;
	int		rs ;
	if ((rs = uc_stat(dname,&sb)) >= 0) {
	    if ((rs = subinfo_checkid(sip)) >= 0) {
	        rs = SR_NOTDIR ;
	        if (S_ISDIR(sb.st_mode) && sip->open.ids) {
	            rs = sperm(&sip->id,&sb,dm) ;
		}
	    }
	} /* end if (uc_stat) */
	return rs ;
}
/* end subroutine (subinfo_dirok) */

static int subinfo_env(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*envp = sip->prname ;
	if (haslc(envp,-1)) {
	    char	*ebuf{} ;
	    if ((rs = malloc_mp(&ebuf)) >= 0) {
		cint	elen = rs ;
	        if ((rs = sncpyuc(ebuf,elen,envp)) >= 0) {
	    	    rs = subinfo_enver(sip,rbuf,rlen,ebuf) ;
		    len = rs ;
		}
		rs1 = uc_free(ebuf) ;
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
	if (envp[0] != '\0') {
	    if (cchar *cp ; (cp = getenv(envp)) != nullptr) {
		if ((rs = sncpy1(rbuf,rlen,cp)) >= 0) {
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
	if (sip->domain && sip->domain[0]) {
	    int		dnl = strlen(sip->domain) ;
	    cchar	*dnp = sip->domain ;
	    while ((dnl > 0) && (dnp[dnl-1] == '.')) dnl -= 1 ;
	    if (dnl > 0) {
	        if (hasuc(dnp,dnl)) {
		    if (dnl <= STACKBUFLEN) {
			cint	dlen = dnl ;
			char	dbuf[dnl+1] ;
	                if ((rs = snwcpylc(dbuf,dlen,dnp,dnl)) > 0) {
		            rs = subinfo_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                }
		    } else {
		        char	*dbuf{} ;
		        if ((rs = malloc_hn(&dbuf)) >= 0) {
		            cint	dlen = rs ;
	                    if ((rs = snwcpylc(dbuf,dlen,dnp,dnl)) > 0) {
		                rs = subinfo_domainer(sip,rbuf,rlen,dbuf,rs) ;
	                    }
		            rs1 = uc_free(dbuf) ;
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
	int		len = 0 ;
	int		i ;
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
	if (domains[i].domain && bnp) {
	    if ((rs = mknpath2(rbuf,rlen,bnp,sip->dname)) >= 0) {
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
	char		*pwbuf{} ;
	if ((rs = malloc_pw(&pwbuf)) >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = rs ;
	    if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,sip->dname)) >= 0) {
	        char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
		    cchar	*d = pw.pw_dir ;
		    cchar	*swd = SWDFNAME ;
	            if ((rs = mkpath2(tbuf,d,swd)) >= 0) {
			rs = subinfo_users(sip,rbuf,rlen,tbuf,d) ;
			len = rs ;
		    }
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (GETPW_NAME) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_user) */

static int subinfo_users(SI *sip,char *rbuf,int rlen,cc *sym,cc *d) noex {
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
        if ((rs = u_lstat(sym,&sb)) >= 0) {
            if (S_ISLNK(sb.st_mode)) {
                if ((rs = u_readlink(sym,rbuf,rlen)) >= 0) {
                    cint     bl = rs ;
                    len = rs ;
                    rbuf[bl] = '\0' ;
                    if ((bl == 1) && (rbuf[0] == '.')) {
                        rs = mknpath1(rbuf,rlen,d) ;
                        len = rs ;
                    } else if ((bl > 0) && (rbuf[0] != '/')) {
                        char        *tmore{} ;
			if ((rs = malloc_mp(&tmore)) >= 0) {
                            if ((rs = mkpath1(tmore,rbuf)) >= 0) {
                                rs = mknpath2(rbuf,rlen,d,tmore) ;
                                len = rs ;
			    }
			    rs1 = uc_free(tmore) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
                    } /* end if */
                    if ((rs >= 0) && (len > 0)) {
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
	int		i = 0 ;		/* used afterwards */
	int		m ;
	int		len = 0 ;
	for (i = 0 ; prmaps[i].prname ; i += 1) {
	    cchar	*pn = prmaps[i].prname ;
	    if ((m = nleadstr(pn,sip->dname,-1)) > 0) {
	        if ((pn[m] == '\0') && (sip->dname[m] == '\0')) {
	            break ;
		}
	    }
	} /* end for */
	if (prmaps[i].prname) {
	    if ((rs = sncpy1(rbuf,rlen,prmaps[i].dname)) >= 0) {
	        len = rs ;
	        rs = subinfo_dirok(sip,rbuf,DMODE) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_prmap) */

static int subinfo_home(SI *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*hn ;
	if ((hn = getenv(VARHOME)) != nullptr) {
	    rs = subinfo_homer(sip,rbuf,rlen,hn) ;
	    len = rs ;
	} else {
	    char	*hbuf{} ;
	    if ((rs = malloc_mp(&hbuf)) >= 0) {
		cint	hlen = rs ;
	        if ((rs = getuserhome(hbuf,hlen,"-")) >= 0) {
	    	    rs = subinfo_homer(sip,rbuf,rlen,hbuf) ;
	    	    len = rs ;
	        }
		rs1 = uc_free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_home) */

static int subinfo_homer(SI *sip,char *rbuf,int rlen,cchar *hn) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (hn && hn[0]) {
	    cchar	*dn = sip->dname ;
	    cchar	*hdname = HOMEBASEDNAME ;
	    if ((rs = mknpath3(rbuf,rlen,hn,hdname,dn)) >= 0) {
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
	char		*tmpfname{} ;
	if ((rs = malloc_mp(&tmpfname)) >= 0) {
	    for (int i = 0 ; basednames[i] ; i += 1) {
	        cchar	*bn = basednames[i] ;
	        if ((rs = dirsearch(bn,sip->dname)) > 0) {
	            if ((rs = mkpath2(tmpfname,bn,sip->dname)) >= 0) {
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
	    rs1 = uc_free(tmpfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_bases) */

static int dirsearch(cchar *basedname,cchar *username) noex {
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	char		*nbuf{} ;
	if ((rs = malloc_mn(&nbuf)) >= 0) {
	    fsdir	dir ;
	    fsdir_ent	ds ;
	    cint	nlen = rs ;
	    if ((rs = fsdir_open(&dir,basedname)) >= 0) {
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
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (dirsearch) */



/* pcsgetnames SUPPORT */
/* lang=C++20 */

/* get various information elements related to the PCS environment */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	1		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsgetnames

	Description:
	This subroutine retrieves the real name of a user according
	to what the PCS thinks it might be.  This subroutine is
	generally called by PCS programs.

	Synopsis:
	int pcsname(cchar *pr,char *rbuf,int rlen,cchar *username) noex

	Arguments:
	pr		PCS system program root (if available)
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	username	username to check

	Returns:
	>=0		OK
	<0		some error (system-error)

	Implementation note:

	Is "home-searching" faster than just asking the system for
	the home directory of a user?  It probably is not!  In
	reality, it is a matter of interacting with the Automount
	server probably much more than what would have happened if
	we just first interacted with the Name-Server-Cache and
	then the Automount server once after that.

	Note that we NEVER |stat(2)| a file directly within any of
	the possible base directories for home-directories.  This
	is done to avoid those stupid SYSLOG entries saying that a
	file could not be found within an indirect auto-mount point
	-- what the base directories usually are now-a-days!

	On second thought, avoiding asking the "system" may be a
	good course of action since even though we are going through
	the name-server cache, it more often than not has to go out
	to some wirdo-only-knows network NIS+ (whatever) server to
	get the PASSWD information.  This is not unusual in a large
	organization.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<project.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<getax.h>
#include	<ucpwcache.h>
#include	<filereadln.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<cfdec.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<snwcpyx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsgetnames.h"

import uconstants ;

/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */

#ifndef	nullptrFNAME
#define	nullptrFNAME	"/dev/null"
#endif

#ifndef	NSYSPIDS
#define	NSYSPIDS	100
#endif

#ifndef	VARNAME
#define	VARNAME		"NAME"
#endif

#ifndef	VARFULLNAME
#define	VARFULLNAME	"FULLNAME"
#endif

#ifndef	DEFPROJNAME
#define	DEFPROJNAME	"default"
#endif

#ifndef	NAMEFNAME
#define	NAMEFNAME	".name"
#endif

#ifndef	FULLNAMEFNAME
#define	FULLNAMEFNAME	".fullname"
#endif

#ifndef	PROJECTFNAME
#define	PROJECTFNAME	".project"
#endif

#ifndef	PCSDPIFNAME /* PCS Default-Project-Info file */
#define	PCSDPIFNAME	"etc/projectinfo"
#endif

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int	pcsprojectinfo(cchar *,char *,int,cchar *) noex ;
}


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		uid:1 ;
	uint		pw:1 ;
} ;

struct subinfo {
	cchar		*pr ;
	cchar		*un ;
	char		*rbuf ;		/* user supplied buffer */
	char		*pwbuf ;
	PASSWD		pw ;
	SUBINFO_FL	init, f ;
	uid_t		uid ;
	int		rlen ;
	int		pwlen ;
} ;

struct pcsnametype {
	cchar		*var ;
	cchar		*fname ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,char *,int,cchar *) noex ;
static int	subinfo_getuid(SUBINFO *,uid_t *) noex ;
static int	subinfo_getpw(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;

static int	getname(SUBINFO *,int) noex ;
static int	getname_var(SUBINFO *,int) noex ;
static int	getname_userhome(SUBINFO *,int) noex ;
static int	getname_again(SUBINFO *,int) noex ;
static int	getname_sysdb(SUBINFO *,int) noex ;

static int	getprojinfo_userhome(SUBINFO *) noex ;
static int	getprojinfo_sysdb(SUBINFO *) noex ;
static int	getprojinfo_pcsdef(SUBINFO *) noex ;


/* local variables */

typedef int (*projinfo_f)(SUBINFO *) noex ;

static constexpr projinfo_f	getprojinfos[] = {
	getprojinfo_userhome,
	getprojinfo_sysdb,
	getprojinfo_pcsdef,
	nullptr
} ;

static constexpr struct pcsnametype	pcsnametypes[] = {
	{ VARNAME, NAMEFNAME },
	{ VARFULLNAME, FULLNAMEFNAME },
	{ nullptr, nullptr }
} ;

typedef int (*nameinfo_f)(SUBINFO *,int) noex ;

static constexpr nameinfo_f	getnames[] = {
	getname_var,
	getname_userhome,
	getname_again,
	getname_sysdb,
	nullptr
} ;

static bufsizevar		maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int pcsgetname(cchar *pr,char *rbuf,int rlen,cchar *username) noex {
	cint	nt = pcsnametype_name ;
	return pcsgetnames(pr,rbuf,rlen,username,nt) ;
}
/* end subroutine (pcsgetname) */

int pcsgetfullname(cc *pr,char *rbuf,int rlen,cc *username) noex {
	cint	nt = pcsnametype_fullname ;
	return pcsgetnames(pr,rbuf,rlen,username,nt) ;
}
/* end subroutine (pcsgetfullname) */

int pcsname(cc *pr,char *rbuf,int rlen,cc *username) noex {
	cint	nt = pcsnametype_name ;
	return pcsgetnames(pr,rbuf,rlen,username,nt) ;
}
/* end subroutine (pcsname) */

int pcsfullname(cc *pr,char *rbuf,int rlen,cc *username) noex {
	cint	nt = pcsnametype_fullname ;
	return pcsgetnames(pr,rbuf,rlen,username,nt) ;
}
/* end subroutine (pcsfullname) */

int pcsnames(cc *pr,char *rbuf,int rlen,cc *un,int nt) noex {
	return pcsgetnames(pr,rbuf,rlen,un,nt) ;
}
/* end subroutine (pcsnames) */

int pcsgetnames(cc *pr,char *rbuf,int rlen,cc *un,int nt) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pr && rbuf && un) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0] && un[0] && (nt < pcsnametype_overlast)) {
	        SUBINFO		si ;
	        if ((rs = subinfo_start(&si,pr,rbuf,rlen,un)) >= 0) {
	            {
	                rs = getname(&si,nt) ;
		        rl = rs ;
	            }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pcsgetnames) */

int pcsgetprojinfo(cc *pr,char *rbuf,int rlen,cc *username) noex {
	return pcsprojectinfo(pr,rbuf,rlen,username) ;
}
/* end subroutine (pcsgetprojinfo) */

int pcsprojectinfo(cc *pr,char *rbuf,int rlen,cc *username) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pr && rbuf && username) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (username[0]) {
	        SUBINFO		mi ;
	        if ((rs = subinfo_start(&mi,pr,rbuf,rlen,username)) >= 0) {
	            for (int i = 0 ; getprojinfos[i] ; i += 1) {
			projinfo_f	fn = getprojinfos[i] ;
	                rs = fn(&mi) ;
			rl = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            rs1 = subinfo_finish(&mi) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pcsprojectinfo) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cc *pr,char *rbuf,int rlen,cc *un) noex {
	int		rs ;
	memclear(sip) ; /* dangerous */
	sip->pr = pr ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	sip->un = un ;
	if ((rs = maxpathlen) >= 0) {
	    char	*pwbuf{} ;
	    if ((rs = malloc_pw(&pwbuf)) >= 0) {
	        sip->pwbuf = pwbuf ;
	        sip->pwlen = rs ;
	    } /* end if (memory-allocation) */
	} /* end if (maxpathlen) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->pwbuf) {
	    rs1 = uc_free(sip->pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->pwbuf = nullptr ;
	}
	sip->pr = nullptr ;
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_getuid(SUBINFO *sip,uid_t *uidp) noex {
	int		rs = SR_OK ;
	cchar		*vn = varname.username ;
	if (! sip->init.uid) {
	    static cchar	*cp = getenv(vn) ;
	    sip->init.uid = true ;
	    if ((cp != nullptr) && (strcmp(cp,sip->un) == 0)) {
	        sip->fl.uid = true ;
	        sip->uid = getuid() ;
	    } else {
	        rs = subinfo_getpw(sip) ;
	        if ((rs >= 0) && (! sip->fl.uid)) {
	            sip->fl.uid = true ;
	            sip->uid = sip->pw.pw_uid ;
	        }
	    } /* end if */
	} /* end if (initializing UID) */
	if (uidp != nullptr) {
	    *uidp = sip->uid ;
	}
	if ((rs >= 0) && (! sip->fl.uid)) {
	    rs = SR_NOTFOUND ;
	}
	return rs ;
}
/* end subroutine (subinfo_getuid) */

static int subinfo_getpw(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	cchar		*un = sip->un ;
	if (! sip->init.pw) {
	    cint	pwlen = sip->pwlen ;
	    char	*pwbuf = sip->pwbuf ;
	    sip->init.pw = true ;
	    if ((un != nullptr) && (un[0] != '\0') && (un[0] != '-')) {
	        if (hasalldig(un,-1)) {
	            uint	uv ;
	            if ((rs = cfdecui(un,-1,&uv)) >= 0) {
	                const uid_t	uid = uv ;
	                rs = getpwusername(&sip->pw,pwbuf,pwlen,uid) ;
	            }
	        } else {
	            rs = GETPW_NAME(&sip->pw,pwbuf,pwlen,un) ;
		}
	    } else {
	        rs = getpwusername(&sip->pw,pwbuf,pwlen,-1) ;
	    }
	    if (rs >= 0) {
	        sip->fl.uid = true ;
	        sip->uid = sip->pw.pw_uid ;
	    }
	} /* end if (was not already initialized) */
	return rs ;
}
/* end subroutine (subinfo_getpw) */

static int getname(SUBINFO *sip,int nt) noex {
	int		rs = SR_BUGCHECK ;
	int		rl = 0 ;
	if (nt < pcsnametype_overlast) {
	    sip->rbuf[0] = '\0' ;
	    for (int i = 0 ; getnames[i] ; i += 1) {
	        nameinfo_f	fn = getnames[i] ;
	        rs = fn(sip,nt) ;
		rl = rs ;
	        if (rs != 0) break ;
	    } /* end for */
	} /* end if (ok) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (getname) */

static int getname_var(SUBINFO *sip,int nt) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*vn = varname.username ;
	cchar		*un = sip->un ;
	bool		f ;
	f = (un[0] == '-') ;
	if (! f) {
	    static cchar	*vun = getenv(vn) ;
	    if ((vun != nullptr) && (vun[0] != '\0'))
	        f = (strcmp(vun,un) == 0) ;
	}
	if (f) {
	    static cchar	*cp = getenv(pcsnametypes[nt].var) ;
	    if ((cp != nullptr) && (cp[0] != '\0')) {
	        rs = sncpy1(sip->rbuf,sip->rlen,cp) ;
		len = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_var) */

static int getname_userhome(SUBINFO *sip ,int nt) noex {
        cint            sz = (2 * (maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	int		na = 0 ;
	char		*a{} ;
        if ((rs = uc_malloc(sz,&a)) >= 0) {
            cint        hlen = maxpathlen ; 
	    cchar	*un = sip->un ;
            char        *hbuf = (a + (na++ * (maxpathlen + 1))) ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	        cchar	*fn = pcsnametypes[nt].fname ;
                char    *tbuf = (a + (na++ * (maxpathlen + 1))) ;
	        if ((rs = mkpath2(tbuf,hbuf,fn)) >= 0) {
	            rs = filereadln(tbuf,sip->rbuf,sip->rlen) ;
		    len = rs ;
		    if (isNotPresent(rs)) rs = SR_OK ;
	        }
	    } /* end if (getuserhome) */
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_userhome) */

static int getname_again(SUBINFO *sip,int nt) noex {
	int		rs = SR_OK ;
	if (nt == pcsnametype_fullname) {
	    nt = pcsnametype_name ;
	    rs = getname(sip,nt) ;
	}
	return rs ;
}
/* end subroutine (getname_again) */

static int getname_sysdb(SUBINFO *sip,int) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (! sip->init.pw) {
	    rs = subinfo_getpw(sip) ;
	}
	if (rs >= 0) {
	    cint	nlen = (strlen(sip->pw.pw_gecos)+10) ;
	    cchar	*gecos = sip->pw.pw_gecos ;
	    char	*nbuf{} ;
	    if ((rs = uc_malloc((nlen+1),&nbuf)) >= 0) {
	        if ((rs = mkgecosname(nbuf,nlen,gecos)) > 0) {
	            rs = mkrealname(sip->rbuf,sip->rlen,nbuf,rs) ;
	            len = rs ;
	        }
	        rs1 = uc_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_sysdb) */

static int getprojinfo_userhome(SUBINFO *sip) noex {
	cint		sz = (2 * (maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	int		na = 0 ;
	char		*a{} ;
	if ((rs = uc_malloc(sz,&a)) >= 0) {
	    cint	hlen = maxpathlen ;
	    cchar	*un = sip->un ;
	    cchar	*fname = PROJECTFNAME ;
	    char	*hbuf = (a + (na++ * (maxpathlen + 1))) ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	        char	*tbuf = (a + (na++ * (maxpathlen + 1))) ;
	        if ((rs = mkpath2(tbuf,hbuf,fname)) >= 0) {
		    cint	rlen = sip->rlen ;
		    char	*rbuf = sip->rbuf ;
	            if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
		        len = rs ;
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
		    }
	        } /* end if (mkpath) */
	    } /* end if (gethome) */
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_userhome) */

static int getprojinfo_sysdb(SUBINFO *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*pjbuf{} ;
	if ((rs = malloc_pj(&pjbuf)) >= 0) {
	    ucentpj	pj ;
	    cint	pjlen = rs ;
	    if ((rs = uc_getpjdef(&pj,pjbuf,pjlen,sip->un)) >= 0) {
	        bool	f = (strcmp(pj.pj_name,DEFPROJNAME) != 0) ;
	        if (f) {
	            uid_t	uid ;
	            if ((rs = subinfo_getuid(sip,&uid)) >= 0) {
		        f = (uid >= NSYSPIDS) ;
		    }
	        }
	        if ((rs >= 0) && f) {
	            rs = sncpy1(sip->rbuf,sip->rlen,pj.pj_comment) ;
		    len = rs ;
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	    rs1 = uc_free(pjbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_sysdb) */

static int getprojinfo_pcsdef(SUBINFO *sip) noex {
	uid_t		uid ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = subinfo_getuid(sip,&uid)) >= 0) {
	    if (uid >= NSYSPIDS) {
	        cchar	*fname = PCSDPIFNAME ;
	        char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = mkpath2(tbuf,sip->pr,fname)) >= 0) {
		        cint	rlen = sip->rlen ;
		        char	*rbuf = sip->rbuf ;
	                if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
	                    len = rs ;
		        } else if (isNotPresent(rs)) {
			    rs = SR_OK ;
		        }
	            } /* end if (mkpath) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (system UID) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_pcsdef) */



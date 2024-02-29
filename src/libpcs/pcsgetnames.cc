/* pcsgetnames SUPPORT */
/* lang=C++20 */

/* get various information elements related to the PCS environment */
/* version %I% last-modified %G% */

#define	CF_DEFPCS	1		/* try a default PCS program-root */
#define	CF_UGETPW	1		/* use |ugetpw_xxx(3uc)| */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	<pwd.h>
#include	<project.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<char.h>
#include	<bfile.h>
#include	<sbuf.h>
#include	<fsdir.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<filereadln.h>
#include	<getusername.h>
#include	<localmisc.h>

#include	"pcsgetnames.h"


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif

#ifndef	NSYSPIDS
#define	NSYSPIDS	100
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	BUFLEN		(MAXPATHLEN + MAXHOSTNAMELEN + LINEBUFLEN)

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
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


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		uid:1 ;
	uint		pw:1 ;
} ;

struct subinfo {
	cchar		*pr ;
	cchar		*varusername ;
	cchar		*un ;
	char		*rbuf ;		/* user supplied buffer */
	SUBINFO_FL	init, f ;
	PASSWD		pw ;
	uid_t		uid ;
	int		rlen ;
	int		pwlen ;
	char		*pwbuf ;
} ;

struct pcsnametype {
	cchar		*var ;
	cchar		*fname ;
} ;


/* forward references */

static int	pcsprojectinfo(cchar *,char *,int,cchar *) noex ;

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

static constexpr int	(*getprojinfos[])(SUBINFO *) = {
	getprojinfo_userhome,
	getprojinfo_sysdb,
	getprojinfo_pcsdef,
	NULL
} ;

enum pcsnametypes {
	pcsnametype_name,
	pcsnametype_fullname,
	pcsnametype_overlast
} ;

static constexpr struct pcsnametype	pcsnametypes[] = {
	{ VARNAME, NAMEFNAME },
	{ VARFULLNAME, FULLNAMEFNAME },
	{ NULL, NULL }
} ;

static constexpr int	(*getnames[])(SUBINFO *,int) = {
	getname_var,
	getname_userhome,
	getname_again,
	getname_sysdb,
	NULL
} ;


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
	SUBINFO		si ;
	int		rs ;

#if	CF_DEFPCS
	if (pr == NULL)
	    pr = getenv(VARPRPCS) ;
#endif

	if (pr == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;
	if (un == NULL) return SR_FAULT ;

	if (un[0] == '\0') return SR_INVALID ;

	if (nt >= pcsnametype_overlast) return SR_INVALID ;

	rbuf[0] = '\0' ;
	if ((rs = subinfo_start(&si,pr,rbuf,rlen,un)) >= 0) {

	    rs = getname(&si,nt) ;

	    subinfo_finish(&si) ;
	} /* end if */

	return rs ;
}
/* end subroutine (pcsgetnames) */

int pcsgetprojinfo(cc *pr,char *rbuf,int rlen,cc *username) noex {
	return pcsprojectinfo(pr,rbuf,rlen,username) ;
}
/* end subroutine (pcsgetprojinfo) */

int pcsprojectinfo(cc *pr,char *rbuf,int rlen,cc *username) noex {
	SUBINFO		mi ;
	int		rs ;

	if (pr == NULL)
	    pr = getenv(VARPRPCS) ;

	if (pr == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;
	if (username == NULL) return SR_FAULT ;

	if (username[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	if ((rs = subinfo_start(&mi,pr,rbuf,rlen,username)) >= 0) {
	    int	i ;

	    for (i = 0 ; getprojinfos[i] != NULL ; i += 1) {
	        rs = (*getprojinfos[i])(&mi) ;
	        if (rs != 0) break ;
	    } /* end for */

	    subinfo_finish(&mi) ;
	} /* end if */

	return rs ;
}
/* end subroutine (pcsprojectinfo) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cc *pr,char *rbuf,int rlen,cc *un) noex {
	cint		pwlen = getbufsize(getbufsize_pw) ;
	int		rs ;
	char		*pwbuf ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->pr = pr ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	sip->un = un ;
	sip->varusername = VARUSERNAME ;

	if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	    sip->pwbuf = pwbuf ;
	    sip->pwlen = pwlen ;
	}

	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->pwbuf != NULL) {
	    rs1 = uc_free(sip->pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->pwbuf = NULL ;
	}

	sip->pr = NULL ;
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_getuid(SUBINFO *sip,uid_t *uidp) noex {
	int		rs = SR_OK ;

	if (! sip->init.uid) {
	    cchar	*cp ;

	    sip->init.uid = TRUE ;
	    cp = getenv(sip->varusername) ;

	    if ((cp != NULL) && (strcmp(cp,sip->un) == 0)) {
	        sip->f.uid = TRUE ;
	        sip->uid = getuid() ;
	    } else {
	        rs = subinfo_getpw(sip) ;
	        if ((rs >= 0) && (! sip->f.uid)) {
	            sip->f.uid = TRUE ;
	            sip->uid = sip->pw.pw_uid ;
	        }
	    } /* end if */

	} /* end if (initializing UID) */

	if (uidp != NULL)
	    *uidp = sip->uid ;

	if ((rs >= 0) && (! sip->f.uid))
	    rs = SR_NOTFOUND ;

	return rs ;
}
/* end subroutine (subinfo_getuid) */

static int subinfo_getpw(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	cchar	*un = sip->un ;

	if (! sip->init.pw) {
	    cint	pwlen = sip->pwlen ;
	    char	*pwbuf = sip->pwbuf ;
	    sip->init.pw = TRUE ;
	    if ((un != NULL) && (un[0] != '\0') && (un[0] != '-')) {
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
	        sip->f.uid = TRUE ;
	        sip->uid = sip->pw.pw_uid ;
	    }
	} /* end if (was not already initialized) */

	return rs ;
}
/* end subroutine (subinfo_getpw) */

static int getname(SUBINFO *sip,int nt) noex {
	int		rs = SR_BUGCHECK ;
	if (nt < pcsnametype_overlast) {
	    sip->rbuf[0] = '\0' ;
	    for (int i = 0 ; getnames[i] != NULL ; i += 1) {
	        rs = (*getnames[i])(sip,nt) ;
	        if (rs != 0) break ;
	    } /* end for */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (getname) */

static int getname_var(SUBINFO *sip,int nt) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	int		f ;
	cchar	*un = sip->un ;

	f = (un[0] == '-') ;
	if (! f) {
	    cchar	*vun = getenv(VARUSERNAME) ;
	    if ((vun != NULL) && (vun[0] != '\0'))
	        f = (strcmp(vun,un) == 0) ;
	}
	if (f) {
	    cchar	*cp = getenv(pcsnametypes[nt].var) ;
	    if ((cp != NULL) && (cp[0] != '\0')) {
	        rs = sncpy1(sip->rbuf,sip->rlen,cp) ;
		len = rs ;
	    }
	} /* end if */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_var) */

static int getname_userhome(SUBINFO *sip ,int nt) noex {
	cint	hlen = MAXPATHLEN ;
	int		rs ;
	cchar		*un = sip->un ;
	char		hbuf[MAXPATHLEN + 1] ;

	if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	    cchar	*fn = pcsnametypes[nt].fname ;
	    char	tbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tbuf,hbuf,fn)) >= 0) {
	        rs = filereadln(tbuf,sip->rbuf,sip->rlen) ;
		if (isNotPresent(rs)) rs = SR_OK ;
	    }
	} /* end if (getuserhome) */

	return rs ;
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

static int getname_sysdb(SUBINFO *sip,int nt) noex {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (! sip->init.pw) {
	    rs = subinfo_getpw(sip) ;
	}

#ifdef	COMMENT /* we have two implementations */
	if (rs >= 0) {
	    cint	nlen = (strlen(sip->pw.pw_gecos)+10) ;
	    cchar	*gecos = sip->pw.pw_gecos ;
	    char	*nbuf ;
	    if ((rs = uc_malloc((nlen+1),&nbuf)) >= 0) {
	        if ((rs = mkgecosname(nbuf,nlen,gecos)) > 0) {
	            rs = mkrealname(sip->rbuf,sip->rlen,nbuf,rs) ;
	            len = rs ;
	        }
	        uc_free(nbuf) ;
	    } /* end if (memory-allocation) */
	} /* end if */
#else /* COMMENT */
	if (rs >= 0) {
	    int		gl ;
	    cchar	*gecos = sip->pw.pw_gecos ;
	    cchar	*gp ;
	    if ((gl = getgecosname(gecos,-1,&gp)) > 0) {
	        cint	nlen = (gl+10) ;
	        char		*nbuf ;
	        if ((rs = uc_malloc((nlen+1),&nbuf)) >= 0) {
	            if ((rs = snwcpyhyphen(nbuf,nlen,gp,gl)) > 0) {
	                rs = mkrealname(sip->rbuf,sip->rlen,nbuf,rs) ;
	                len = rs ;
	            }
	            uc_free(nbuf) ;
	        } /* end if (memory-allocation) */
	    } /* end if (getgecosname) */
	} /* end if */
#endif /* COMMENT */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_sysdb) */

static int getprojinfo_userhome(SUBINFO *sip) noex {
	cint	hlen = MAXPATHLEN ;
	int		rs ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	cchar	*fname = PROJECTFNAME ;
	char		hbuf[MAXPATHLEN + 1] ;

	hbuf[0] = '\0' ;
	if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	    char	tbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tbuf,hbuf,fname)) >= 0) {
		cint	rlen = sip->rlen ;
		char		*rbuf = sip->rbuf ;
	        if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
		    len = rs ;
		} else if (isNotPresent(rs)) 
		    rs = SR_OK ;
	    }
	} /* end if (gethome) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_userhome) */

static int getprojinfo_sysdb(SUBINFO *sip) noex {
	PROJECT		pj ;
	cint	pjlen = getbufsize(getbufsize_pj) ;
	int		rs ;
	int		len = 0 ;
	char		*pjbuf ;

	if ((rs = uc_malloc((pjlen+1),&pjbuf)) >= 0) {
	    if ((rs = uc_getdefaultproj(sip->un,&pj,pjbuf,pjlen)) >= 0) {
	        int	f = (strcmp(pj.pj_name,DEFPROJNAME) != 0) ;
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
	    uc_free(pjbuf) ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_sysdb) */

static int getprojinfo_pcsdef(SUBINFO *sip) noex {
	uid_t		uid ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if ((rs = subinfo_getuid(sip,&uid)) >= 0) {
	    if (uid >= NSYSPIDS) {
	        cchar	*fname = PCSDPIFNAME ;
	        char	tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mkpath2(tbuf,sip->pr,fname)) >= 0) {
		    cint	rlen = sip->rlen ;
		    char	*rbuf = sip->rbuf ;
	            if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
	                len = rs ;
		    } else if (isNotPresent(rs)) 
			rs = SR_OK ;
	        }
	    } /* end if (system UID) */
	} /* end if */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getprojinfo_pcsdef) */



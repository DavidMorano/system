/* pwi SUPPORT */
/* lang=C++20 */

/* PassWord Index manager */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a small hack for use by the USERINFO built-in command
	that is part of the Korn Shell (KSH).

	This object provides some front-end glue for using the
	IPASSWD object on an IPASSWD database.

	Notes:

	= Searching for a PWI DB file:

	If a PWI DB name is passed to us, we only search for that
	DB.  If no PWI DB is passwed, we search first for a DB with
	the same name as our cluster name (if we have one); otherwise
	failing that we search for a DB with our node name.

	If no DB is present then we either make (a-fresh) the DB
	given to us by name, or we make a DB using our cluster name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<endianstr.h>
#include	<mkpath.h>
#include	<mkfnamesuf.h>
#include	<char.h>
#include	<realname.h>
#include	<ipasswd.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<ucpwcache.h>
#include	<prgetclustername.h>
#include	<localmisc.h>

#include	"pwi.h"


/* local defines */

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#define	GETPW_UID	ucpwcache_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_PWCACHE */

#define	DBDNAME		"var/pwi"

#ifndef	PASSWDFNAME
#define	PASSWDFNAME	"/etc/passwd"
#endif

#ifndef	realnameLEN
#define	realnameLEN	100
#endif

#define	TO_FILEMOD	(24 * 3600)

#define	PROG_MKPWI	"mkpwi"

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKPWI_DBNAME"

#undef	VARPRPWI
#define	VARPRPWI	"MKPWI_PROGRAMROOT"

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	PWDESC		struct pwdesc


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy4(char *,int, cchar *,cchar *,cchar *,cchar *) ;
extern int	sncpylc(char *,int,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	getnodename(char *,int) ;
extern int	getgecosname(cchar *,int,cchar **) ;
extern int	hasuc(cchar *,int) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpyuc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;


/* local structures */

struct subinfo_flags {
	uint		dbname:1 ;
} ;

struct subinfo {
	cchar		*pr ;
	cchar		*dbname ;
	cchar		*midname ;
	SUBINFO_FL	alloc ;
} ;

struct pwdesc {
	PASSWD		*pwp ;
	char		*pwbuf ;
	int		pwlen ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,cchar *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;
static int	subinfo_midname(SUBINFO *) noex ;
static int	subinfo_mkpwi(SUBINFO *) noex ;

static int	realname_isextra(realname *,PWDESC *,cchar *) noex ;


/* local variables */

static constexpr cchar	*exports[] = {
	VARHZ,
	VARNODE,
	VARHOMEDNAME,
	VARUSERNAME,
	VARLOGNAME,
	VARTZ,
	VARPWD,
	nullptr
} ;

/* use fixed locations for security reasons (like we care!) */
static constexpr cchar	*prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;

static constexpr cchar	*extras = "¹²³" ;


/* exported variables */


/* exported subroutines */

int pwi_open(PWI *op,cchar *pr,cchar *dbname) noex {
	SUBINFO		si, *sip = &si ;
	YSTAT		sb, *sbp = &sb ;
	const time_t	daytime = time(nullptr) ;
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if ((rs = subinfo_start(sip,pr,dbname)) >= 0) {
	    if ((rs = subinfo_midname(sip)) >= 0) {
	        time_t	ti_pwi ;
	        cint	to = TO_FILEMOD ;
	        cchar	*suf = IPASSWD_SUF ;
	        cchar	*endstr = ENDIANSTR ;
	        cchar	*midname = sip->midname ;
	        char	fname[MAXPATHLEN+1] ;

	        if ((rs = mkfnamesuf2(fname,midname,suf,endstr)) >= 0) {
	            rs1 = u_stat(fname,sbp) ;

	            ti_pwi = sbp->st_mtime ;
	            if ((rs1 >= 0) && (ti_pwi == 0)) {
	                rs1 = SR_NOTFOUND ;
	            }

	            if ((rs1 >= 0) && ((daytime - ti_pwi) >= to)) {
	                rs1 = SR_NOTFOUND ;
	            }

	            if (rs1 >= 0) {

	                rs1 = u_stat(PASSWDFNAME,&sb) ;

	                if ((rs1 >= 0) && (sb.st_mtime > ti_pwi)) {
	                    rs1 = SR_NOTFOUND ;
	                }

	            } /* end if (checking against system PASSWD file) */

	            if ((rs1 == SR_NOTFOUND) || (rs1 == SR_ACCESS)) {
	                rs = subinfo_mkpwi(sip) ;
	            } else {
	                rs = rs1 ;
		    }

	        } /* end if (mkfnamesuf) */

	        if (rs >= 0) {
	            if ((rs = ipasswd_open(&op->db,midname)) >= 0) {
	                op->magic = PWI_MAGIC ;
	            }
	        }
	    } /* end if (subinfo_midname) */
	    rs1 = subinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */

	return rs ;
}
/* end subroutine (pwi_open) */

int pwi_close(PWI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PWI_MAGIC) return SR_NOTOPEN ;

	rs1 = ipasswd_close(&op->db) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pwi_close) */

int pwi_lookup(PWI *op,char *rbuf,int rlen,cchar *name) noex {
	IPASSWD_CUR	cur ;
	realname	rn ;
	cint		nlen = realnameLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl ;
	int		c = 0 ;
	int		fopts = 0 ;
	int		ul = 0 ;
	cchar		*sp ;
	char		nbuf[realnameLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magic != PWI_MAGIC) return SR_NOTOPEN ;

	if (name[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	if ((rlen >= 0) && (rlen < USERNAMELEN))
	    return SR_OVERFLOW ;

/* conditionally convert to lower case as needed */

	sp = name ;
	sl = -1 ;
	if (hasuc(name,-1)) {
	    sp = nbuf ;
	    rs = sncpylc(nbuf,nlen,name) ;
	    sl = rs ;
	}

/* load "name" into realname object for lookup */

	if (rs >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = getbufsize(getbufsize_pw) ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
		PWDESC	pd ;
		pd.pwp = &pw ;
		pd.pwbuf = pwbuf ;
		pd.pwlen = pwlen ;
	        if ((rs = realname_start(&rn,sp,sl)) >= 0) {

	            if ((rs = ipasswd_curbegin(&op->db,&cur)) >= 0) {
		        char	un[USERNAMELEN + 1] ;

	                while (rs >= 0) {
	                    rs1 = ipasswd_fetch(&op->db,&rn,&cur,fopts,un) ;
	                    if (rs1 == SR_NOTFOUND) break ;
			    rs = rs1 ;

			    if (rs >= 0) {
			        if ((rs = realname_isextra(&rn,&pd,un)) == 0) {
	                            ul = rs1 ; /* this must be HERE! */
	                            c += 1 ;
	                            rs = sncpy1(rbuf,rlen,un) ;
			        }
			    }

	                } /* end while */

	                rs1 = ipasswd_curend(&op->db,&cur) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */

	            rs1 = realname_finish(&rn) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (realname) */
		rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (ok) */

/* if there was more than one match to the name, punt and issue error */

	if (rs >= 0) {
	    if (c <= 0) {
	        rs = SR_NOTFOUND ;
	    } else if (c > 1) {
	        rs = SR_NOTUNIQ ;
	    }
	} /* end if */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (pwi_lookup) */


/* private subroutines */

static int subinfo_start(SUBINFO *sip,cchar *pr,cchar *dbname) noex {
	memclear(sip) ;
	sip->pr = pr ;
	sip->dbname = dbname ;
	return SR_OK ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->midname != nullptr) {
	    rs1 = uc_free(sip->midname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->midname = nullptr ;
	}
	if (sip->alloc.dbname && (sip->dbname != nullptr)) {
	    rs1 = uc_free(sip->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dbname = nullptr ;
	}
	sip->pr = nullptr ;
	return rs ;
}
/* end subroutine (subinfo_finish) */

/* find the inverse-passwd database file */
static int subinfo_midname(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		dblen = -1 ;
	cchar		*dbname = sip->dbname ;
	char		namebuf[MAXNAMELEN + 1] ;

	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    cint	nlen = NODENAMELEN ;
	    char	nbuf[NODENAMELEN + 1] ;
	    char	cbuf[NODENAMELEN + 1] ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        cchar	*nn ;
	        if ((rs = prgetclustername(sip->pr,cbuf,nlen,nbuf)) >= 0) {
	            nn = cbuf ;
		} else if (rs == rsn) {
		    rs = SR_OK ;
		    nn = nbuf ;
		}
		if (rs >= 0) {
	            rs = mkpath3(namebuf,sip->pr,DBDNAME,nn) ;
	            dbname = namebuf ;
	            dblen = rs ;
		}
	    }
	} /* end if (empty specification) */

	if (rs >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(dbname,dblen,&cp)) >= 0) {
	        sip->midname = cp ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (subinfo_midname) */

/* make the inverse-passwd database file */
static int subinfo_mkpwi(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		i ;
	cchar		*pr = sip->pr ;
	cchar		*dbname = sip->midname ;
	cchar		*progmkpwi = PROG_MKPWI ;
	char		progfname[MAXPATHLEN + 1] ;

	if (dbname == nullptr)
	    return SR_FAULT ;

	if (dbname[0] == '\0')
	    return SR_INVALID ;

	for (i = 0 ; prbins[i] != nullptr ; i += 1) {
	    if ((rs = mkpath3(progfname,pr,prbins[i],progmkpwi)) >= 0) {
	        rs = perm(progfname,-1,-1,nullptr,X_OK) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

	if (rs >= 0) {
	    spawnproc	ps{} ;
	    vecstr	envs ;
	    pid_t	cpid ;
	    cint	vo = VECSTR_OCOMPACT ;
	    int		cstat ;
	    int		cex ;
	    if ((rs = vecstr_start(&envs,10,vo)) >= 0) {
		cint	alen = MAXNAMELEN ;
		int	cl ;
	        cchar	*av[10] ;
		cchar	*cp ;
		cchar	*argz = progmkpwi ;
		char	abuf[MAXNAMELEN+1] ;
	        i = 0 ;

		if ((cl = sfbasename(progmkpwi,-1,&cp)) > 0) {
		    argz = abuf ;
		    strwcpyuc(abuf,cp,MIN(cl,alen)) ;
		}

/* setup arguments */

	        av[i++] = argz ;
	        av[i++] = nullptr ;

/* setup environment */

	        vecstr_envadd(&envs,VARPRPWI,sip->pr,-1) ;

		if (sip->dbname != nullptr)
	            vecstr_envadd(&envs,VARDBNAME,sip->dbname,-1) ;

	        for (i = 0 ; exports[i] != nullptr ; i += 1) {
	            if ((cp = getenv(exports[i])) != nullptr) {
	                rs = vecstr_envadd(&envs,exports[i],cp,-1) ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
/* go */
	        if (rs >= 0) {
	            mainv	ev ;
	            if ((rs = vecstr_getvec(&envs,&ev)) >= 0) {

	            memclear(&ps) ;
	            ps.opts |= SPAWNPROC_OIGNINTR ;
	            ps.opts |= SPAWNPROC_OSETPGRP ;
	            for (i = 0 ; i < 3 ; i += 1) {
			if (i != 2) {
	                    ps.disp[i] = SPAWNPROC_DCLOSE ;
			} else {
	                    ps.disp[i] = SPAWNPROC_DINHERIT ;
			}
	            } /* end for */

	            rs = spawnproc(&ps,progfname,av,ev) ;
	            cpid = rs ;

		    } /* end if (vecstr_getvec) */
	        } /* end if (ok) */
	        vecstr_finish(&envs) ;
	    } /* end if (vecstr) */

	    if (rs >= 0) {

	        cstat = 0 ;
	        rs = 0 ;
	        while (rs == 0) {
	            rs = u_waitpid(cpid,&cstat,0) ;
	            if (rs == SR_INTR) rs = 0 ;
	        } /* end while */

	        if (rs >= 0) {
	            cex = 0 ;
	            if (WIFSIGNALED(cstat)) {
	                rs = SR_UNATCH ;	/* protocol not attached */
		    }
	            if ((rs >= 0) && WIFEXITED(cstat)) {

	                cex = WEXITSTATUS(cstat) ;

	                if (cex != 0)
	                    rs = SR_LIBBAD ;

	            } /* end if (wait-exited) */

	        } /* end if (process finished) */

	    } /* end if */

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (subinfo_mkpwi) */

static int realname_isextra(realname *op,PWDESC *pdp,cchar *un) noex {
	PASSWD		*pwp = pdp->pwp ;
	cint		pwlen = pdp->pwlen ;
	int		rs ;
	int		f = false ;
	cchar		*ln ;
	char		*pwbuf = pdp->pwbuf ;
	if ((rs = realname_getlast(op,&ln)) >= 0) {
	    cint	ll = rs ;
	    if (strnpbrk(ln,ll,extras) == nullptr) {
		if ((rs = GETPW_NAME(pwp,pwbuf,pwlen,un)) > 0) {
		    cchar	*sp ;
		    if ((rs = getgecosname(pwp->pw_gecos,-1,&sp)) > 0) {
			f = (strnpbrk(sp,rs,extras) != nullptr) ;
		    }
		} else if (rs == SR_NOTFOUND) {
		    rs = SR_OK ;
		}
	    } /* end if (query does not have special extras) */
	} /* end if (realname_getlast) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (realname_isextra) */



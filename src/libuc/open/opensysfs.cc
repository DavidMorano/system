/* opensysfs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* open a channel (file-descriptor) to some system file or directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opensysfs

	Description:
	We open a file or a directory to some system resource.

	Synopsis:
	int opensysfs(int w,int of,int ttl) noex

	Arguments:
	w		what resource
	of		open-flags
	ttl		time-to-live for the resource

	Returns:
	>=0		FD of dir-cache
	<0		error

	Notes:
	- open flags:
		O_NOTTY
		O_EXCL		
		O_SYNC
		O_NDELAY
		O_NONBLOCK
		O_CLOEXEC

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<bit>
#include	<usystem.h>
#include	<ids.h>
#include	<vecpstr.h>
#include	<dirseen.h>
#include	<envhelp.h>
#include	<ctdec.h>
#include	<stdfnames.h>
#include	<spawnproc.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"opensysfs.h"


/* local defines */

#define	OPENSYSFS_PROGSYSFS	"sysfs"
#define	OPENSYSFS_PROGMKPWI	"mkpwi"

#define	SUFNAMELEN	USERNAMELEN
#define	ABUFLEN		USERNAMELEN

#define	REALNAMESUF	"pwi"

#define	MINPERMS	0664		/* minimum permissions on files */

#define	PASSWDFNAME	"/etc/passwd"
#define	GROUPFNAME	"/etc/group"
#define	PROJECTFNAME	"/etc/project"
#define	SHADOWFNAME	"/etc/shadow"
#define	USERATTRFNAME	"/etc/user_attr"
#define	SHELLSFNAME	"/etc/shells"

#ifndef	MAXNAMELEN
#ifdef	MAXNAMLEN
#define	MAXNAMELEN	MAXNAMLEN
#else
#define	MAXNAMELEN	256
#endif
#endif /* MAXNAMELEN */


/* imported namespaces */

using std::endian ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	snsd(char *,int,cchar *,uint) noex ;
    extern int	snsds(char *,int,cchar *,cchar *) noex ;
    extern int	snwcpy(char *,int,cchar *,int) noex ;
    extern int	sncpyuc(char *,int,cchar *) noex ;
    extern int	sncpy1(char *,int,cchar *) noex ;
    extern int	sncpy2(char *,int,cchar *,cchar *) noex ;
    extern int	mkpath2(char *,cchar *,cchar *) noex ;
    extern int	mkpath3(char *,cchar *,cchar *,cchar *) noex ;
    extern int	mkpath2w(char *,cchar *,cchar *,int) noex ;
    extern int	sfshrink(cchar *,int,cchar **) noex ;
    extern int	sfbasename(cchar *,int,cchar **) noex ;
    extern int	sfprogroot(cchar *,int,cchar **) noex ;
    extern int	matostr(cchar **,int,cchar *,int) noex ;
    extern int	cfdeci(cchar *,int,int *) noex ;
    extern int	ctdeci(char *,int,int) noex ;
    extern int	strwcmp(cchar *,cchar *,int) noex ;
    extern int	mktmpfile(char *,mode_t,cchar *) noex ;
    extern int	sperm(ids *,USTAT *,int) noex ;
    extern int	dirseen_notseen(dirseen *,USTAT *,cchar *,int) noex ;
    extern int	dirseen_notadd(dirseen *,USTAT *,cchar *,int) noex ;
}

extern "C" {
    extern char	*strwcpy(char *,cchar *,int) noex ;
    extern char	*strwcpylc(char *,cchar *,int) noex ;
    extern char	*strwcpyuc(char *,cchar *,int) noex ;
    extern char	*strcpylc(char *,cchar *) noex ;
    extern char	*strcpyuc(char *,cchar *) noex ;
    extern char	*timestr_log(time_t,char *) noex ;
}


/* external variables */

extern cchar	**environ ;


/* local structures */


/* forward references */

static int	mkrealpath(char *,int,cchar *,cchar *) noex ;
static int	opencfile(int,int,int) noex ;
static int	checkperms(cchar *,USTAT *,mode_t) noex ;

static int	findprog(ids *,char *,cchar *) noex ;
static int	findprogbin(ids *,dirseen *,char *,cchar *,cchar *) noex ;

static int	runmkpwi(int,cchar *,int) noex ;
static int	runsysfs(int) noex ;


/* local variables */

constexpr cpcchar	dbfnames[] = { /* source database files */
	PASSWDFNAME,
	PASSWDFNAME,
	GROUPFNAME,
	PROJECTFNAME,
	PASSWDFNAME,
	GROUPFNAME,
	PROJECTFNAME,
	PASSWDFNAME,
	SHELLSFNAME,
	SHADOWFNAME,
	USERATTRFNAME,
	nullptr
} ;

constexpr cpcchar	cfnames[] = {
	OPENSYSFS_FUSERHOMES,
	OPENSYSFS_FUSERNAMES,
	OPENSYSFS_FGROUPNAMES,
	OPENSYSFS_FPROJECTNAMES,
	OPENSYSFS_FPASSWD,
	OPENSYSFS_FGROUP,
	OPENSYSFS_FPROJECT,
	OPENSYSFS_FREALNAME,
	OPENSYSFS_FSHELLS,
	OPENSYSFS_FSHADOW,
	OPENSYSFS_FUSERATTR,
	nullptr
} ;

constexpr cpcchar	prvars[] = {
	"EXTRA",
	"PREROOT",
	nullptr
} ;

constexpr cpcchar	prdirs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ;

constexpr cpcchar	prbins[] = {
	"sbin",
	"bin",
	nullptr
} ;

constexpr cpcchar	envbads[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"RANDOM",
	"SECONDS",
	nullptr
} ;

constexpr bool		f_endian = (endian::big == endian::native) ;

constexpr cchar		*endianstr = ((f_endian) ? "1" : "0") ;


/* exported variables */


/* exported subroutines */

int opensysfs(int w,int of,int ttl) noex {
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    rs = SR_BADF ;
	    if (ttl < 0) ttl = OPENSYSFS_DEFTTL ;
	    if ((of & O_ACCMODE) == O_RDONLY) {
	        switch (w) {
	        case OPENSYSFS_WUSERHOMES:
	        case OPENSYSFS_WUSERNAMES:
	        case OPENSYSFS_WPASSWD:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WGROUPNAMES:
	        case OPENSYSFS_WGROUP:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WPROJECTNAMES:
	        case OPENSYSFS_WPROJECT:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WREALNAME:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WSHELLS:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WSHADOW:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        case OPENSYSFS_WUSERATTR:
	            rs = opencfile(w,of,ttl) ;
	            break ;
	        default:
	            rs = SR_INVALID ;
	            break ;
	        } /* end switch */
	    } /* end if (ok) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (opensysfs) */


/* local subroutines */

static int opencfile(int w,int of,int ttl) noex {
	int		rs ;
	int		fd = -1 ;
	cchar		*sdname = OPENSYSFS_SYSDNAME ;
	cchar		*gcname = cfnames[w] ;
	char		gfname[MAXPATHLEN+1] ;
	if ((rs = mkrealpath(gfname,w,sdname,gcname)) > 0) {
	    USTAT	sb ;
	    time_t	dt = 0 ;
	    if ((rs = u_stat(gfname,&sb)) >= 0) {
		mode_t	mm = MINPERMS ;
	        time_t	mt = sb.st_mtime ;
		switch (w) {
	        case OPENSYSFS_WSHADOW:
		    mm &= (~7) ;
		/* FALLTHROUGH */
		default:
	            rs = checkperms(gfname,&sb,mm) ;
		    break ;
		} /* end switch */
	        if ((rs >= 0) && (ttl >= 0)) {
	            switch (w) {
	            case OPENSYSFS_WUSERNAMES:
	            case OPENSYSFS_WGROUPNAMES:
	            case OPENSYSFS_WPROJECTNAMES:
	            case OPENSYSFS_WPASSWD:
	            case OPENSYSFS_WGROUP:
	            case OPENSYSFS_WPROJECT:
	            case OPENSYSFS_WREALNAME:
	            case OPENSYSFS_WSHELLS:
	            case OPENSYSFS_WSHADOW:
	            case OPENSYSFS_WUSERATTR:
	                if (dt == 0) dt = time(nullptr) ;
	                if ((dt-mt) >= ttl) rs = SR_STALE ;
	                break ;
	            } /* end switch */
	        } /* end if (ttl) */
	        if ((rs >= 0) && (ttl >= 0)) {
	            switch (w) {
	            case OPENSYSFS_WUSERHOMES:
	                {
	                    cint	aw = OPENSYSFS_WUSERNAMES ;
	                    cchar	*an ;
	                    char	tfname[MAXPATHLEN+1] ;
	                    an = cfnames[aw] ;
	                    if ((rs = mkpath2(tfname,sdname,an)) >= 0) {
	                        if ((rs = u_stat(tfname,&sb)) >= 0) {
	                            if (dt == 0) dt = time(nullptr) ;
	                            if ((dt-sb.st_mtime) >= ttl) rs = SR_STALE ;
	                        } /* end if (stat) */
	                    } /* end if (mkpath) */
	                } /* end block */
	                break ;
	            } /* end switch */
	        } /* end if (alternate test) */

	        if (rs >= 0) {
	            if (strcmp(dbfnames[w],STDFNNULL) != 0) {
	                if ((rs = u_stat(dbfnames[w],&sb)) >= 0) {
	                    if (dt == 0) dt = time(nullptr) ;
	                    if (sb.st_mtime > mt) rs = SR_STALE ;
	                }
	            } /* end if (not std-null) */
	        } /* end if (DB stat) */
	    } /* end if (stat) */
	    if ((rs == SR_NOENT) || (rs == SR_STALE)) {
	        cchar	*tp ;
	        switch (w) {
	        case OPENSYSFS_WREALNAME:
	            if ((tp = strrchr(gfname,'.')) != nullptr) {
	                rs = runmkpwi(w,gfname,(tp-gfname)) ;
	            }
	            break ;
	        default:
	            rs = runsysfs(w) ;
	            break ;
	        } /* end switch */
	    } /* end (not-found or stale) */
	    if (rs >= 0) {
	        if ((rs = u_open(gfname,of,0666)) >= 0) {
	            fd = rs ;
	            if (of & O_CLOEXEC) {
	                rs = uc_closeonexec(fd,true) ;
		    }
	            if (rs < 0) {
	                u_close(fd) ;
		    }
	        } /* end if (file-open) */
	    } /* end if (ok) */
	} /* end if (mkrealpath) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opencfile) */

static int mkrealpath(char *gfname,int w,cchar *sdname,cchar *gcname) noex {
	int		rs = SR_OK ;
	switch (w) {
	case OPENSYSFS_WREALNAME:
	    {
	        cint	slen = SUFNAMELEN ;
	        cchar	*suf = REALNAMESUF ;
	        char	sbuf[SUFNAMELEN+1] ;
	        char	cbuf[MAXNAMELEN+1] ;
	        if ((rs = sncpy2(sbuf,slen,suf,endianstr)) >= 0) {
	            if ((rs = snsds(cbuf,MAXNAMELEN,gcname,sbuf)) >= 0) {
	                rs = mkpath2w(gfname,sdname,cbuf,rs) ;
	            }
	        }
	    }
	    break ;
	default:
	    rs = mkpath2(gfname,sdname,gcname) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (mkrealpath) */

static int checkperms(cchar *gfname,USTAT *sbp,mode_t mm) noex {
	int		rs = SR_OK ;
	if ((sbp->st_mode & mm) != mm) {
	    uid_t	uid = getuid() ;
	    if (sbp->st_uid == uid) {
	        mode_t	newm = (sbp->st_mode | mm) ;
	        rs = u_chmod(gfname,newm) ;
	    } else {
	        uc_unlink(gfname) ;
	        rs = SR_NOENT ;
	    }
	} /* end if (problem) */
	return rs ;
}
/* end subroutine (checkperms) */

static int runmkpwi(int w,cchar *dbp,int dbl) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGMKPWI ;
	if ((rs = ids_load(&id)) >= 0) {
	    char	pfname[MAXPATHLEN+1] ;
	    if ((rs = findprog(&id,pfname,pn)) > 0) {
		cint	zlen = MAXNAMELEN ;
	        int	cs = 0 ;
	        cchar	*av[3] ;
	        char	zbuf[MAXNAMELEN+1] ;
	        if ((rs = sncpyuc(zbuf,zlen,pn)) >= 0) {
	            envhelp	env ;
	            if ((rs = envhelp_start(&env,envbads,environ)) >= 0) {
	                mainv	ev = nullptr ;
	                if (rs >= 0) {
	                    cchar	*cp ;
	                    int		cl ;
	                    cchar	*evar = "MKPWI_PROGRAMROOT" ;
	                    if ((cl = sfprogroot(pfname,-1,&cp)) > 0) {
	                        rs = envhelp_envset(&env,evar,cp,cl) ;
	                    }
	                }
	                if (rs >= 0) {
	                    cchar	*evar = "MKPWI_DBNAME" ;
	                    rs = envhelp_envset(&env,evar,dbp,dbl) ;
	                }
	                if (rs >= 0) {
	                    cchar	*evar = "MKPWI_W" ;
			    char	dbuf[DIGBUFLEN+1] ;
			    cint	dlen = DIGBUFLEN ;
			    if ((rs = ctdec(dbuf,dlen,w)) >= 0) {
	                        rs = envhelp_envset(&env,evar,dbuf,rs) ;
			    }
	                }
	                if ((rs = envhelp_getvec(&env,&ev)) >= 0) {
	                    spawnproc_con	ps{} ;
	                    av[0] = zbuf ;
	                    av[1] = nullptr ;
	                    ps.disp[0] = SPAWNPROC_DCLOSE ;
	                    ps.disp[1] = SPAWNPROC_DCLOSE ;
	                    ps.disp[2] = SPAWNPROC_DNULL ;
	                    ps.opts |= SPAWNPROC_OIGNINTR ;
	                    ps.opts |= SPAWNPROC_OSETSID ;
	                    if ((rs = spawnproc(&ps,pfname,av,ev)) >= 0) {
	                        const pid_t	pid = rs ;
	                        rs = u_waitpid(pid,&cs,0) ;
	                    } /* end if (spawned and waited for) */
	                } /* end if (envhelp-get) */
	                rs1 = envhelp_finish(&env) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (envhelp) */
	        } /* end if (sncpyuc) */
	    } /* end if (findprog) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
}
/* end subroutine (runmkpwi) */

static int runsysfs(int w) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGSYSFS ;
	if ((rs = ids_load(&id)) >= 0) {
	    char	pfname[MAXPATHLEN+1] ;
	    if ((rs = findprog(&id,pfname,pn)) > 0) {
	        spawnproc_con	ps{} ;
	        cint		alen = ABUFLEN ;
	        int		cs = 0 ;
	        cchar		*av[3] ;
	        char		zbuf[MAXNAMELEN+1] ;
	        char		abuf[ABUFLEN+1] ;
	        if ((rs = sncpyuc(zbuf,MAXNAMELEN,pn)) >= 0) {
	            cchar	**ev = nullptr ;
	            if ((rs = ctdeci(abuf,alen,w)) >= 0) {
	                av[0] = zbuf ;
	                av[1] = abuf ;
	                av[2] = nullptr ;
	                ps.disp[0] = SPAWNPROC_DCLOSE ;
	                ps.disp[1] = SPAWNPROC_DCLOSE ;
	                ps.disp[2] = SPAWNPROC_DNULL ;
	                ps.opts |= SPAWNPROC_OIGNINTR ;
	                ps.opts |= SPAWNPROC_OSETSID ;
	                if ((rs = spawnproc(&ps,pfname,av,ev)) >= 0) {
	                    const pid_t	pid = rs ;
	                    rs = u_waitpid(pid,&cs,0) ;
	                } /* end if (spawned and waited for) */
	            } /* end if (argument-preparation) */
	        } /* end if (sncpyuc) */
	    } /* end if (findprog) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
}
/* end subroutine (runsysfs) */

static int findprog(ids *idp,char *pfname,cchar *pn) noex {
	dirseen		bhist, *blp = &bhist ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	pfname[0] = '\0' ;
	if ((rs = dirseen_start(blp)) >= 0) {
	    vecpstr	dhist ;
	    if ((rs = vecpstr_start(&dhist,4,0,0)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        int	f = false ;
	        cchar	*pr ;
	        for (int i = 0 ; (rs >= 0) && prvars[i] ; i += 1) {
	            if ((pr = getenv(prvars[i])) != nullptr) {
	                if ((rs = vecpstr_already(&dhist,pr,-1)) == rsn) {
	                    if ((rs = findprogbin(idp,blp,pfname,pr,pn)) > 0) {
	                        pl = rs ;
	                        f = true ;
	                    }
	                    if ((rs >= 0) && (! f)) {
	                        rs = vecpstr_add(&dhist,pr,-1) ;
	                    }
	                }
	            } /* end if */
	            if (f) break ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && (! f)) {
	            for (int i = 0 ; (rs >= 0) && prdirs[i] ; i += 1) {
	                pr = prdirs[i] ;
	                if ((rs = vecpstr_already(&dhist,pr,-1)) == rsn) {
	                    if ((rs = findprogbin(idp,blp,pfname,pr,pn)) > 0) {
	                        pl = rs ;
	                        f = true ;
	                    }
	                    if ((rs >= 0) && (! f))
	                        vecpstr_add(&dhist,pr,-1) ;
	                } /* end if (not already) */
	                if (rs > 0) break ;
	            } /* end for */
	        } /* end if */
	        rs1 = vecpstr_finish(&dhist) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecpstr) */
	    rs1 = dirseen_finish(blp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprog) */

static int findprogbin(ids *idp,dirseen *dsp,char *pfname,cc *pr,cc *pn) noex {
	USTAT		sb ;
	int		rs ;
	int		pl = 0 ;
	int		f = false ;
	if ((rs = u_stat(pr,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        int	dl ;
	        char	bindname[MAXPATHLEN+1] ;
	        for (int i = 0 ; (rs >= 0) && prbins[i] ; i += 1) {
	            if ((rs = mkpath2(bindname,pr,prbins[i])) >= 0) {
	                dl = rs ;
	                if ((rs = dirseen_notseen(dsp,&sb,bindname,rs)) > 0) {
	                    if ((rs = mkpath2(pfname,bindname,pn)) >= 0) {
	                        USTAT	psb ;
	                        pl = rs ;
	                        if ((rs = u_stat(pfname,&psb)) >= 0) {
	                            if ((rs = sperm(idp,&psb,X_OK)) >= 0) {
	                                f = S_ISREG(psb.st_mode) ;
	                            }
	                        }
	                        if ((rs < 0) && isNotPresent(rs)) rs = SR_OK ;
	                        if ((rs >= 0) && (! f)) {
	                            rs = dirseen_notadd(dsp,&sb,bindname,dl) ;
	                        }
	                    } /* end if (mkpath) */
	                } /* end if (dirseen-notseen) */
	            } /* end if (mkpath) */
	        } /* end for */
	    } /* end if (was a directory) */
	} /* end if (stat) */
	if ((rs >= 0) && (! f)) pl = 0 ;
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprogbin) */



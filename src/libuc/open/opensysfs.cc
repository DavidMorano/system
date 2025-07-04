/* opensysfs SUPPORT */
/* charset=ISO8859-1 */
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
	int opensysfs(opensysdbs w,int of,int ttl) noex

	Arguments:
	w		what resource
	of		open-flags
	ttl		time-to-live for the resource

	Returns:
	>=0		FD of dir-cache
	<0		error (system-return)

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
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>		/* |ENDIANSTR(3u)| */
#include	<ids.h>
#include	<vecpstr.h>
#include	<dirseen.h>
#include	<envhelp.h>
#include	<ctdec.h>
#include	<stdfnames.h>
#include	<sysdbfiles.h>
#include	<spawnproc.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<opensysdbs.h>
#include	<localmisc.h>

#include	"opensysfs.h"

import unixfnames ;

/* local defines */

#define	OPENSYSFS_PROGSYSFS	"sysfs"
#define	OPENSYSFS_PROGMKPWI	"mkpwi"

#define	REALNAMESUF	"pwi"

#define	MINPERMS	0664		/* minimum permissions on files */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		usernamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	mkrealpath(char *,int,cchar *,cchar *) noex ;
static int	opencfile(int,int,int) noex ;
static int	checkperms(cchar *,USTAT *,mode_t) noex ;

static int	findprog(ids *,char *,cchar *) noex ;
static int	findprogbin(ids *,dirseen *,char *,cchar *,cchar *) noex ;

static int	runmkpwi(int,cchar *,int) noex ;
static int	runsysfs(int) noex ;


/* local variables */

static vars		var ;

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


/* exported variables */


/* exported subroutines */

namespace libuc {
    int opensysfs(opensysdbs w,int of,int ttl) noex {
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < opensysdb_overlast)) {
	    rs = SR_BADF ;
	    if (ttl < 0) ttl = OPENSYSFS_DEFTTL ;
	    if ((of & O_ACCMODE) == O_RDONLY) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            switch (w) {
	            case opensysdb_userhomes:
	            case opensysdb_usernames:
	            case opensysdb_passwd:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_groupnames:
	            case opensysdb_group:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_projectnames:
	            case opensysdb_project:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_realname:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_shells:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_shadow:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            case opensysdb_userattr:
	                rs = opencfile(w,of,ttl) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
		} /* end if (vars) */
	    } /* end if (ok) */
	} /* end if (valid) */
	return rs ;
    } /* end subroutine (opensysfs) */
} /* end namespace (libuc) */


/* local subroutines */

static int opencfile(int w,int of,int ttl) noex {
    	cint		sz = ((var.maxpathlen + 1) * 2) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		fd = -1 ; /* return-value */
	int		ai = 0 ;
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    cchar	*sdname = OPENSYSFS_SYSDNAME ;
	    cchar	*gcname = opensysdb[w] ;
	    char	*gfname = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = mkrealpath(gfname,w,sdname,gcname)) > 0) {
	        time_t	dt = 0 ;
	        if (USTAT sb ; (rs = u_stat(gfname,&sb)) >= 0) {
		    mode_t	mm = MINPERMS ;
	            time_t	mt = sb.st_mtime ;
		    cchar	*ufn = nullptr ;
		    switch (w) {
	            case opensysdb_shadow:
		        ufn = unixfname.shadow ;
		        mm &= (~7) ;
		        fallthrough ;
		        /* FALLTHROUGH */
		    default:
	                rs = checkperms(gfname,&sb,mm) ;
		        break ;
		    } /* end switch */
	            if ((rs >= 0) && (ttl >= 0)) {
	                switch (w) {
	                case opensysdb_usernames:
	                case opensysdb_groupnames:
	                case opensysdb_projectnames:
	                case opensysdb_passwd:
	                case opensysdb_group:
	                case opensysdb_project:
	                case opensysdb_realname:
	                case opensysdb_shells:
	                case opensysdb_shadow:
	                case opensysdb_userattr:
	                    if (dt == 0) dt = time(nullptr) ;
	                    if ((dt-mt) >= ttl) rs = SR_STALE ;
	                    break ;
	                } /* end switch */
	            } /* end if (ttl) */
		    (void) ufn ;
	            if ((rs >= 0) && (ttl >= 0)) {
	                switch (w) {
	                case opensysdb_userhomes:
	                    {
	                        cint	aw = opensysdb_usernames ;
	                        cchar	*an ;
	                        char	*tfname = (a + ((maxpath + 1) * ai++)) ;
	                        an = opensysdb[aw] ;
	                        if ((rs = mkpath2(tfname,sdname,an)) >= 0) {
	                            if ((rs = u_stat(tfname,&sb)) >= 0) {
	                                if (dt == 0) dt = time(nullptr) ;
	                                if ((dt-sb.st_mtime) >= ttl) {
					    rs = SR_STALE ;
				        }
	                            } /* end if (stat) */
	                        } /* end if (mkpath) */
	                    } /* end block */
	                    break ;
	                } /* end switch */
	            } /* end if (alternate test) */
#ifdef	COMMENT
	            if (rs >= 0) {
	                if (strcmp(dbfnames[w],STDFNNULL) != 0) {
	                    if ((rs = u_stat(dbfnames[w],&sb)) >= 0) {
	                        if (dt == 0) dt = time(nullptr) ;
	                        if (sb.st_mtime > mt) rs = SR_STALE ;
	                    }
	                } /* end if (not std-null) */
	            } /* end if (DB stat) */
#endif /* COMMENT */
	        } /* end if (stat) */
	        if ((rs == SR_NOENT) || (rs == SR_STALE)) {
	            switch (w) {
	            case opensysdb_realname:
	                if (cchar *tp ; (tp = strrchr(gfname,'.')) != nullptr) {
			    cint	tl = intconv(tp - gfname) ;
	                    rs = runmkpwi(w,gfname,tl) ;
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
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opencfile) */

static int mkrealpath(char *gfname,int w,cchar *sdname,cchar *gcname) noex {
	int		rs ;
	if (char *cbuf ; (rs = malloc_mp(&cbuf)) >= 0) {
	    cint	slen = szof(REALNAMESUF) + 10 ;
	    cint	clen = rs ;
	    cchar	*suf = REALNAMESUF ;
	    switch (w) {
	    case opensysdb_realname:
	        {
		    cchar	*es = ENDIANSTR ;
	            char	sbuf[slen + 1] ;
	            if ((rs = sncpy(sbuf,slen,suf,es)) >= 0) {
	                if ((rs = snsds(cbuf,clen,gcname,sbuf)) >= 0) {
	                    rs = mkpath2w(gfname,sdname,cbuf,rs) ;
	                }
	            }
	        } /* end block */
	        break ;
	    default:
	        rs = mkpath2(gfname,sdname,gcname) ;
	        break ;
	    } /* end switch */
	    rs = rsfree(rs,cbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (mkrealpath) */

static int checkperms(cchar *gfname,USTAT *sbp,mode_t mm) noex {
	int		rs = SR_OK ;
	if ((sbp->st_mode & mm) != mm) {
	    const uid_t		uid = getuid() ;
	    if (sbp->st_uid == uid) {
	        cmode		newm = (sbp->st_mode | mm) ;
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
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGMKPWI ;
	if (ids id ; (rs = ids_load(&id)) >= 0) {
	    cint	sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
	    cint	maxpath = var.maxpathlen ;
	    int		ai = 0 ;
	    if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
		cint	zlen = maxpath ;
	        char	*pfname = (a + ((maxpath + 1) * ai++)) ;
	        char	*zbuf = (a + ((maxpath + 1) * ai++)) ;
	        if ((rs = findprog(&id,pfname,pn)) > 0) {
	            int		cs = 0 ;
	            cchar	*av[3] ;
	            if ((rs = sncpyuc(zbuf,zlen,pn)) >= 0) {
	                envhelp	env ;
	                if ((rs = envhelp_start(&env,envbads,np)) >= 0) {
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
	        rs = rsfree(rs,a) ;
	    } /* end if (m-a-f) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
}
/* end subroutine (runmkpwi) */

static int runsysfs(int w) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGSYSFS ;
	if (ids id ; (rs = ids_load(&id)) >= 0) {
	    cint	maxpath = var.maxpathlen ;
	    cint	maxname = var.maxnamelen ;
	    int		ai = 0 ;
	    int		sz = 0 ;
	    sz += (var.maxpathlen + 1) ;
	    sz += (var.maxnamelen + 1) ;
	    sz += (var.usernamelen + 1) ;
	    if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
		cint	zlen = maxpath ;
		cint	alen = maxname ;
	        char	*pfname = (a + ((maxpath + 1) * ai++)) ;
	        char	*zbuf = (a + ((maxpath + 1) * ai++)) ;
	        char	*abuf = (a + ((maxname + 1) * ai++)) ;
	        if ((rs = findprog(&id,pfname,pn)) > 0) {
	            int		cs = 0 ;
	            cchar	*av[3] ;
	            if ((rs = sncpyuc(zbuf,zlen,pn)) >= 0) {
	                spawnproc_con	ps{} ;
	                mainv		ev = nullptr ;
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
	                        const pid_t		pid = rs ;
	                        rs = u_waitpid(pid,&cs,0) ;
	                    } /* end if (spawned and waited for) */
	                } /* end if (argument-preparation) */
	            } /* end if (sncpyuc) */
	        } /* end if (findprog) */
	        rs = rsfree(rs,a) ;
	    } /* end if (m-a-f) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
}
/* end subroutine (runsysfs) */

static int findprog(ids *idp,char *pfname,cchar *pn) noex {
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	pfname[0] = '\0' ;
	if (dirseen dirs ; (rs = dirs.start) >= 0) {
	    if (vecpstr dhist ; (rs = dhist.start(4,0,0)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        bool	f = false ;
	        cchar	*pr ;
	        for (int i = 0 ; (rs >= 0) && prvars[i] ; i += 1) {
	            if ((pr = getenv(prvars[i])) != nullptr) {
	                if ((rs = dhist.already(pr,-1)) == rsn) {
			    dirseen	*dsp = &dirs ;
	                    if ((rs = findprogbin(idp,dsp,pfname,pr,pn)) > 0) {
	                        pl = rs ;
	                        f = true ;
	                    }
	                    if ((rs >= 0) && (! f)) {
	                        rs = dhist.add(pr,-1) ;
	                    }
	                }
	            } /* end if */
	            if (f) break ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && (! f)) {
	            for (int i = 0 ; (rs >= 0) && prdirs[i] ; i += 1) {
	                pr = prdirs[i] ;
	                if ((rs = dhist.already(pr,-1)) == rsn) {
			    dirseen	*dsp = &dirs ;
	                    if ((rs = findprogbin(idp,dsp,pfname,pr,pn)) > 0) {
	                        pl = rs ;
	                        f = true ;
	                    }
	                    if ((rs >= 0) && (! f)) {
	                        rs = dhist.add(pr,-1) ;
			    }
	                } /* end if (not already) */
	                if (rs > 0) break ;
	            } /* end for */
	        } /* end if */
	        rs1 = dhist.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecpstr) */
	    rs1 = dirs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprog) */

static int findprogbin(ids *idp,dirseen *dsp,char *pfname,cc *pr,cc *pn) noex {
	int		rs ;
	int		pl = 0 ; /* return-value */
	bool		f = false ;
	if (USTAT sb ; (rs = u_stat(pr,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	            for (int i = 0 ; (rs >= 0) && prbins[i] ; i += 1) {
	                if ((rs = mkpath2(tbuf,pr,prbins[i])) >= 0) {
	                    cint	dl = rs ;
	                    if ((rs = dsp->notseen(&sb,tbuf,rs)) > 0) {
	                        if ((rs = mkpath2(pfname,tbuf,pn)) >= 0) {
	                            USTAT	psb ;
	                            pl = rs ;
	                            if ((rs = u_stat(pfname,&psb)) >= 0) {
	                                if ((rs = permid(idp,&psb,X_OK)) >= 0) {
	                                    f = S_ISREG(psb.st_mode) ;
	                                } else if (isNotPresent(rs)) {
					    rs = SR_OK ;
				        }
	                            } else if (isNotPresent(rs)) {
				        rs = SR_OK ;
				    }
	                            if ((rs >= 0) && (! f)) {
	                                rs = dsp->notadd(&sb,tbuf,dl) ;
	                            }
	                        } /* end if (mkpath) */
	                    } /* end if (dirseen-notseen) */
	                } /* end if (mkpath) */
	            } /* end for */
		} /* end if (m-a-f) */
	    } /* end if (was a directory) */
	} /* end if (stat) */
	if ((rs >= 0) && (! f)) pl = 0 ;
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprogbin) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	        if ((rs = getbufsize(getbufsize_un)) >= 0) {
		    usernamelen = rs ;
	        }
	    }
	}
    	return rs ;
}
/* end method (vars::operator) */



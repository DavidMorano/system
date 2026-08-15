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
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU |ENDIANSTR(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecpstr.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<envhelp.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<stdfnames.h>		/* LIBUC */
#include	<sysdbfiles.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<opensysdbs.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"opensysfs.hh"

#pragma		GCC dependency		"mod/unixfnames.ccm"

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


/* local structures */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		usernamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	mkrealpath(char *,int,cchar *,cchar *) noex ;
local int	opencfile(int,int,int) noex ;
local int	checkperms(cchar *,ustat *,mode_t) noex ;

local int	findprog(ids *,char *,cchar *) noex ;
local int	findprogbin(ids *,dirseen *,char *,cchar *,cchar *) noex ;

local int	runmkpwi(int,cchar *,int) noex ;
local int	runsysfs(int) noex ;


/* local variables */

static vars		var ;

constexpr cpcchar	prvars[] = {
	"EXTRA",
	"PREROOT",
	nullptr
} ; /* end array */

constexpr cpcchar	prdirs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ; /* end array */

constexpr cpcchar	prbins[] = {
	"sbin",
	"bin",
	nullptr
} ; /* end array */

constexpr cpcchar	envbads[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"RANDOM",
	"SECONDS",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

namespace libuc {
    int opensysfs(opensysdbs w,int of,int ttl) noex {
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < opensysdb_overlast)) ylikely {
	    rs = SR_BADF ;
	    if (ttl < 0) ttl = OPENSYSFS_DEFTTL ;
	    if ((of & O_ACCMODE) == O_RDONLY) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
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

local int opencfile(int w,int of,int ttl) noex {
    	cint		sz = ((var.maxpathlen + 1) * 2) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	int		ai = 0 ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	    cchar	*sdname = OPENSYSFS_SYSDNAME ;
	    cchar	*gcname = opensysdb[w] ;
	    char	*gfname = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = mkrealpath(gfname,w,sdname,gcname)) > 0) ylikely {
	        time_t	dt = 0 ;
	        if (ustat sb ; (rs = u_stat(gfname,&sb)) >= 0) ylikely {
		    mode_t	mm = MINPERMS ;
	            time_t	mt = sb.st_mtime ;
		    cchar	*ufn = nullptr ;
		    switch (w) {
	            case opensysdb_shadow:
		        ufn = unixfname.shadow ;
		        mm &= (~7) ;
		        falldown ;
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
	            if ((rs >= 0) && (ttl >= 0)) ylikely {
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
	            if (rs >= 0) ylikely {
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
	            if ((rs = u_open(gfname,of,0666)) >= 0) ylikely {
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
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opencfile) */

local int mkrealpath(char *gfname,int w,cchar *sdname,cchar *gcname) noex {
	int		rs ;
	int		rs1 ;
	if (char *cbuf ; (rs = lm_mp(&cbuf)) >= 0) ylikely {
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
	    rs1 = lm_free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (mkrealpath) */

local int checkperms(cchar *gfname,ustat *sbp,mode_t mm) noex {
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
} /* end subroutine (checkperms) */

local int runmkpwi(int w,cchar *dbp,int dbl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGMKPWI ;
	if (ids id ; (rs = ids_load(&id)) >= 0) ylikely {
	    cint	sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
	    cint	maxpath = var.maxpathlen ;
	    int		ai = 0 ;
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
		cint	zlen = maxpath ;
	        char	*pfname	= (a + ((maxpath + 1) * ai++)) ;
	        char	*zbuf	= (a + ((maxpath + 1) * ai++)) ;
	        if ((rs = findprog(&id,pfname,pn)) > 0) ylikely {
	            int		cs = 0 ;
	            cchar	*av[3] ;
	            if ((rs = sncpyuc(zbuf,zlen,pn)) >= 0) {
	                envhelp	env ;
	                if ((rs = envhelp_start(&env,envbads,np)) >= 0) {
	                    mainv	ev = nullptr ;
	                    if (rs >= 0) {
	                        cchar	*cp ;
	                        int	cl ;
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
	        rs1 = lm_free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
} /* end subroutine (runmkpwi) */

local int runsysfs(int w) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pn = OPENSYSFS_PROGSYSFS ;
	if (ids id ; (rs = ids_load(&id)) >= 0) ylikely {
	    cint	maxpath = var.maxpathlen ;
	    cint	maxname = var.maxnamelen ;
	    int		ai = 0 ;
	    int		sz = 0 ;
	    sz += (var.maxpathlen + 1) ;
	    sz += (var.maxnamelen + 1) ;
	    sz += (var.usernamelen + 1) ;
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
		cint	zlen = maxpath ;
		cint	alen = maxname ;
	        char	*pfname	= (a + ((maxpath + 1) * ai++)) ;
	        char	*zbuf	= (a + ((maxpath + 1) * ai++)) ;
	        char	*abuf	= (a + ((maxname + 1) * ai++)) ;
	        if ((rs = findprog(&id,pfname,pn)) > 0) ylikely {
	            int		cs = 0 ;
	            cchar	*av[3] ;
	            if ((rs = sncpyuc(zbuf,zlen,pn)) >= 0) ylikely {
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
	        rs1 = lm_free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
} /* end subroutine (runsysfs) */

local int findprog(ids *idp,char *pfname,cchar *pn) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	pfname[0] = '\0' ;
	if (dirseen dirs ; (rs = dirs.start) >= 0) ylikely {
	    if (vecpstr dhist ; (rs = dhist.start(4,0,0)) >= 0) ylikely {
	        bool	f = false ;
	        cchar	*pr ; /* used-multiple */
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
} /* end subroutine (findprog) */

local int findprogbin(ids *idp,dirseen *dsp,char *pfname,cc *pr,cc *pn) noex {
	int		rs ;
	int		pl = 0 ; /* return-value */
	bool		f = false ;
	if (ustat sb ; (rs = u_stat(pr,&sb)) >= 0) ylikely {
	    if (S_ISDIR(sb.st_mode)) ylikely {
		if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	            for (int i = 0 ; (rs >= 0) && prbins[i] ; i += 1) {
	                if ((rs = mkpath2(tbuf,pr,prbins[i])) >= 0) {
	                    cint	dl = rs ;
	                    if ((rs = dsp->notseen(&sb,tbuf,rs)) > 0) {
	                        if ((rs = mkpath2(pfname,tbuf,pn)) >= 0) {
	                            ustat psb ;
	                            pl = rs ;
	                            if ((rs = u_stat(pfname,&psb)) >= 0) {
	                                if ((rs = permids(idp,&psb,X_OK)) >= 0) {
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
} /* end subroutine (findprogbin) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	        maxpathlen = rs ;
	        if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		    usernamelen = rs ;
	        }
	    }
	}
    	return rs ;
} /* end method (vars::operator) */



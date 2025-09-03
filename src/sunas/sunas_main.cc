/* main SUPPORT (SUNAS) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* (another) program to switch programs */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory allocations */
#define	CF_DEBUGARGS	1		/* debug argument handling */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This program cleanes up the arguments to the SUN AS progran.

	Synopsis:
	$ <name> <arg(s)>

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<ids.h>
#include	<envhelp.h>
#include	<vecpstr.h>
#include	<six.h>
#include	<hasx.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"sunas_conf.h"
#include	"defs.h"


/* local defines */

#ifndef	LOGNAMELEN
#define	LOGNAMELEN	32
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	PI		proginfo
#define	VS		vecpstr


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) noex ;

#if	CF_DEBUGS
extern int	debugopen(cchar *) noex ;
extern int	debugprintf(cchar *,...) noex ;
extern int	debugclose() noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* external variables */


/* local structures */


/* forward references */

static int	proc_devino(PI *) noex ;
static int	proc_idsbegin(PI *) noex ;
static int	proc_idsend(PI *) noex ;
static int	proc_find(PI *,envhelp *,cchar *,cchar **) noex ;
static int	proc_progok(PI *,cchar *) noex ;
static int	proc_exec(PI *,cchar *,cchar **,cchar **) noex ;
static int	proc_args(PI *,VS *,cchar **) noex ;
static int	proc_savecopy(PI *,cchar *) noex ;

static int	mkcfname(char *,cchar *,cchar *) noex ;

#if	CF_DEBUGS && CF_DEBUGARGS
static int	debugargs(cchar **) noex ;
#endif


/* local variables */

constexpr pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

constexpr mapex		mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ 0, 0 }
} ;

constexpr cpcchar	sysdirs[] = {
	"/usr/xpg4/bin",
	"/usr/ccs/bin",
	"/usr/bin",
	"/usr/SUNWspro/bin",
	"/usr/extra/bin",
	"/usr/extra/sbin",
	"/usr/extra/libexec",
	"/usr/preroot/bin",
	"/usr/preroot/sbin",
	"/usr/preroot/libexec",
	"/usr/sbin",
	nullptr
} ;

constexpr cpcchar	envbads[] = {
	"LD_PRELOAD",
	"RANDOM",
	"A__z",
	nullptr
} ;

constexpr cpcchar	argdels[] = {
	"-v",
	nullptr
} ;

constexpr cpcchar	argchecks[] = {
	"-I",
	nullptr
} ;

constexpr cpcchar	assexts[] = {
	"s",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	proginfo	pi, *pip = &pi ;
	bfile		errfile ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		rs = SR_OK ;
	int		rs1 ;
	int		ex = EX_INFO ;
	cchar		*pr = nullptr ;
	cchar		*cp ;

#if	CF_DEBUGS
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

#if	CF_DEBUGS && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

	if ((cp = getenv(VARBANNER)) == nullptr) cp = BANNER ;
	proginfo_setbanner(pip,cp) ;

	if ((cp = getenv(VARERRORFNAME)) == nullptr) cp = BFILE_STDERR ;
	if ((rs1 = bopen(&errfile,cp,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->fl.errfile = true ;
	    bcontrol(&errfile,BC_LINEBUF,0) ;
	} else if (! isNotPresent(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

/* initialize */

	pip->verboselevel = 1 ;

/* get the program root */

	if (rs >= 0) {
	    if ((rs = proginfo_setpiv(pip,pr,&initvars)) >= 0) {
	        rs = proginfo_setsearchname(pip,VARSEARCHNAME,nullptr) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("main: pr=%s\n",pip->pr) ;
	debugprintf("main: sn=%s\n",pip->searchname) ;
	debugprintf("main: pn=%s\n",pip->progname) ;
#endif

/* go */

	if (rs >= 0) {
	    if ((rs = proc_devino(pip)) >= 0) {
	        envhelp	env ;
	        if ((rs = envhelp_start(&env,envbads,pip->envv)) >= 0) {
	            if ((rs = proc_idsbegin(pip)) >= 0) {
			{
			    cchar	*pn = pip->progname ;
	                    rs = proc_find(pip,&env,pn,argv) ;
			}
	                rs1 = proc_idsend(pip) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (proc-ids) */
	            rs1 = envhelp_finish(&env) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (envhelp_finish) */
	    } /* end if (proc_devino) */
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    ex = EX_OSERR ;
	    fmt = "%s: resource failure (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	} /* end if (ok) */

	if ((rs < 0) && (ex == EX_OK)) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    ex = EX_NOPROG ;
	    fmt = "%s: inaccessible software components (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_NOENT:
	        ex = EX_NOPROG ;
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        break ;
	    } /* end switch */
	} /* end if (bad) */

/* return */
	if (pip->efp != nullptr) {
	    pip->fl.errfile = false ;
	    bclose(pip->efp) ;
	    pip->efp = nullptr ;
	}

	proginfo_finish(pip) ;

badprogstart:

#if	CF_DEBUGS && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("main: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif

#if	CF_DEBUGS
	debugclose() ;
#endif

	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

static int proc_devino(PI *pip) noex {
	cint		tlen = MAXPATHLEN ;
	int		rs ;
	char		tbuf[MAXPATHLEN+1] ;

#if	CF_DEBUGS
	debugprintf("main/proc_devino: ent\n") ;
#endif

	if ((rs = proginfo_getename(pip,tbuf,tlen)) >= 0) {
#if	CF_DEBUGS
	    debugprintf("main/proc_devino: ent tbuf=%s\n",tbuf) ;
#endif
	    if (ustat sb ; (rs = u_stat(tbuf,&sb)) >= 0) {
	        pip->dev = sb.st_dev ;
	        pip->ino = sb.st_ino ;
#if	CF_DEBUGS
	        debugprintf("main/proc_devino: dev=\\x%08x ino=%llu\n",
	            pip->dev,pip->ino) ;
#endif
	    }
	} /* end if (proginfo_getename) */

	return rs ;
}
/* end subroutine (proc_devino) */

static int proc_idsbegin(PI *pip) noex {
	return ids_load(&pip->id) ;
}
/* end subroutine (proc_idsbegin) */

static int proc_idsend(PI *pip) noex {
	return ids_release(&pip->id) ;
}
/* end subroutine (proc_idsend) */

static int proc_find(PI *pip,envhelp *elp,cchar *bn,mainv argv) noex {
	cint		clen = MAXNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		f_found = false ;
	cchar		*prefix = PROG_PREFIX ;
	char		rbuf[MAXPATHLEN+1] ;
	char		cbuf[MAXNAMELEN+1] ;
	for (i = 0 ; sysdirs[i] != nullptr ; i += 1) {
	    cchar	*sd = sysdirs[i] ;
	    if ((rs = sncpy2(cbuf,clen,prefix,bn)) >= 0) {
	        if ((rs = mkpath2(rbuf,sd,cbuf)) >= 0) {
	            if ((rs = proc_progok(pip,rbuf)) > 0) {
	                mainv	ev ;
	                if ((rs = envhelp_getvec(elp,&ev)) >= 0) {
			    f_found = true ;
	                    rs = proc_exec(pip,rbuf,argv,ev) ;
	                }
	            }
	        } /* end if (mkpath) */
	    } /* end if (sncpy2) */
	    if (f_found) break ;
	    if ((rs < 0) && (! isNotPresent(rs))) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (proc_find) */

static int proc_progok(PI *pip,cchar *progfname) noex {
	int		rs ;
	int		f = false ;
#if	CF_DEBUGS
	debugprintf("main/proc_progok: ent progfname=%s\n",progfname) ;
#endif
	if (ustat sb ; (rs = uc_stat(progfname,&sb)) >= 0) {
#if	CF_DEBUGS
	    {
	        cint	mlen = TIMEBUFLEN ;
	        char		mbuf[TIMEBUFLEN+1] ;
	        snfilemode(mbuf,mlen,sb.st_mode) ;
	        debugprintf("main/proc_progok: mode=%s (%07o)\n",
	            mbuf,sb.st_mode) ;
	        debugprintf("main/proc_progok: mode=\\x%08x\n",sb.st_mode) ;
	    }
#endif /* CF_DEBUGS */
	    if (S_ISREG(sb.st_mode)) {
	        if ((rs = permid(&pip->id,&sb,X_OK)) >= 0) {
	            if ((sb.st_dev != pip->dev) || (sb.st_ino != pip->ino)) {
	                f = true ;
	            }
	        } else if (isNotPresent(rs)) {
#if	CF_DEBUGS
	            debugprintf("main/proc_progok: permid() rs=%d\n",rs) ;
#endif
	            rs = SR_OK ;
	        }
	    } /* end if (regular-file) */
	} else if (isNotPresent(rs)) {
#if	CF_DEBUGS
	    debugprintf("main/proc_progok: u_stat() rs=%d\n",rs) ;
#endif
	    rs = SR_OK ;
	}
#if	CF_DEBUGS
	debugprintf("main/proc_progok: rs=%d f=%u\n",rs,f) ;
#endif
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (proc_progok) */

static int proc_exec(PI *pip,cc *rbuf,cc **argv,cc **penvv) noex {
	VS		a ;
	int		rs ;
	int		rs1 ;
	bflush(pip->efp) ;
#if	CF_DEBUGS
	debugprintf("main/proc_exec: rbuf=%s\n",rbuf) ;
#endif
	if ((rs = vecpstr_start(&a,10,1024,0)) >= 0) {
	    if ((rs = proc_args(pip,&a,argv)) >= 0) {
		cchar	**va ;
		if ((rs = vecpstr_getvec(&a,&va)) >= 0) {
#if	CF_DEBUGS && CF_DEBUGARGS
		    debugargs(va) ;
#else /* CF_DEBUGARGS */
#ifdef	COMMENT
		    cint	rsn = SR_NOTENT ;
	            if ((rs = uc_execve(rbuf,va,penvv)) == rsn) {
	                rs = uc_isaexecve(rbuf,va,penvv) ;
	            }
#else /* COMMENT */
	            rs = uc_isaexecve(rbuf,va,penvv) ;
#endif /* COMMENT */
#endif /* CF_DEBUGARGS */
		} /* end if (vecpstr_getvec) */
	    } /* end if (proc_args) */
	    rs1 = vecpstr_finish(&a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecpstr) */
#if	CF_DEBUGS
	debugprintf("main/proc_exec: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (proc_exec) */

static int proc_args(PI *pip,VS *alp,cchar **argv) noex {
	int		rs ;
	if ((rs = vecpstr_add(alp,argv[0],-1)) >= 0) {
	    bool	f_del = false ;
	    for (int i = 1 ; argv[i] != nullptr ; i += 1) {
	        cchar	*a = argv[i] ;
	        if (matstr(argdels,a,-1) < 0) {
		    if (matstr(argchecks,a,-1) >= 0) {
		        f_del = true ;
		    } else {
		        if (! f_del) {
			    if ((a[0] != '-') && (sifext(a,-1,assexts) >= 0)) {
				rs = proc_savecopy(pip,a) ;
			    }
			    if (rs >= 0) {
			        rs = vecpstr_add(alp,a,-1) ;
			    }
		        }
		        f_del = false ;
		    }
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (vecpstr_add) */
	return rs ;
}
/* end subroutine (proc_args) */

static int proc_savecopy(PI *pip,cchar *a) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	cchar		*dn = COPYDNAME ;
	if ustat sb ; (rs = uc_stat(dn,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		cint	am = (X_OK|W_OK) ;
		if ((rs = permid(&pip->id,&sb,am)) >= 0) {
	            char	cbuf[MAXPATHLEN+1] ;
	            if ((rs = mkcfname(cbuf,dn,a)) >= 0) {
			bfile	cfile, *cfp = &cfile ;
	                if ((rs = bopen(cfp,cbuf,"wct",0666)) >= 0) {
		            {
				f = true ;
	                        rs = bwritefile(cfp,a) ;
    		            }
	                    rs1 = bclose(cfp) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (file-copy) */
	            } /* end if (mkcfname) */
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ; /* fail silently! */
		} /* end if (permid) */
	    } /* end if (is-dir) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ; /* fail silently! */
	} /* end if (stat) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (proc_savecopy) */

static int mkcfname(char *cbuf,cchar *dn,cchar *a) noex {
	int		rs = SR_OK ;
	cchar		*bp ;
	if (int bl ; (bl = sfbasename(a,-1,&bp)) > 0) {
	    rs = mkpath2w(cbuf,dn,bp,bl) ;
	} else {
	    rs = SR_INVALID ;
	} /* end if (base-name) */
	return rs ;
}
/* end subroutine (mkcfname) */

#if	CF_DEBUGS && CF_DEBUGARGS
static int debugargs(cchar **va) noex {
	int	i ;
	for (i = 0 ; va[i] != nullptr ; i += 1) {
	   debugprintf("main: a[%02u]=%s\n",i,va[i]) ;
	}
	return i ;
}
/* end subroutine (debugargs) */
#endif /* CF_DEBUGARGS */



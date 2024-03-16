/* spawnproc SUPPORT */
/* lang=C++20 */

/* spawn a local program */
/* version %I% last-modified %G% */

#define	CF_ISAEXEC	0		/* use Solaris® |isaexec(3c)| */
#define	CF_ENVSORT	0		/* sort the environment? */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	spawnproc

	Description:
	Spawn a process while specifying some options for its start-up.

	Synopsis:
	int spawnproc(SP *psap,cchar *fname,cchar **argv,cchar **envv) noex

	Arguments:
	psap		pointer to optional file descriptor dispositions
	fname		program to execute
	argv		arguments to program
	envv		environment to program

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

	Implementation notes:  
	Remember that the |pipe(2)| system call creates two pipe
	file descriptors.  Both of these file descriptors are open
	for reading and writing on System V UNIX®.  However, on BSD
	systems (or older BSD systems assuming that they have not
	yet upgraded to the correct System V behavior) the first
	file descriptor, the one in the zeroth array element, is
	only open for reading.  The second file descriptor, the one
	in the oneth array element, is only open for writing.  We
	will follow the BSD semantics for portability but hopefully,
	someday, the BSD version of UNIX® will get up to speed with
	the rest of the world!

	Also, note that since we are (very) likely to be running
	in a (hotly) mutli-threaded environment, we have to be quite
	sure that we try to only call async-signal-safe (really
	fork-safe) subroutines after the |fork(2)| and before any
	|exit(2)|.  The thing to avoid that might be used by
	"accident" is a hidden |malloc(3c)| (or friends) after the
	|fork(2)| someplace.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<ucgetpid.h>
#include	<envhelp.h>
#include	<vecstr.h>
#include	<ids.h>
#include	<sigign.h>
#include	<mkpathx.h>
#include	<snx.h>
#include	<sfx.h>
#include	<xperm.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"spawnproc.h"


/* local defines */

#ifndef	NOFILE
#define	NOFILE		20
#endif

#define	NENV		100

#ifndef	ENVBUFLEN
#define	ENVBUFLEN	(MAXPATHLEN + 40)
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif

#define	DEFPATH		"/usr/xpg4/bin:/usr/bin:/usr/extra/bin"


/* imported namespaces */


/* local typedefs */

typedef spawnproc_con	scon ;


/* external subroutines */

extern "C" {
    extern int	matkeystr(cchar **,cchar *,int) noex ;
    extern int	getprogpath(ids *,vecstr *,char *,cchar *,int) noex ;
    extern int	getpwd(char *,int) noex ;
    extern int	dupup(int,int) noex ;
    extern int	sigdefaults(cint *) noex ;
    extern int	sigignores(cint *) noex ;
    extern int	vecstr_addpath(vecstr *,cchar *,int) noex ;
    extern int	vecstr_addcspath(vecstr *) noex ;
}


/* external variables */


/* local structures */

struct vars {
	int		maxpathlen ;
} ;


/* forward reference */

static int	spawnproc_pipes(scon *,cchar *,mainv,mainv) noex ;
static int	spawnproc_parfin(scon *,int,int *,int (*)[2]) noex ;

static void	spawnproc_child(scon *,cchar *,mainv,mainv,
			int,int *,int (*)[2]) noex ;

static int	envhelp_load(envhelp *,char *,cchar *,mainv) noex ;

static int	findprog(char *,char *,cchar *) noex ;
static int	findxfile(ids *,char *,cchar *) noex ;
static int	ourfork() noex ;
static int	opendevnull(int *,int) noex ;

static int	mkvars() noex ;


/* local variables */

static constexpr cpcchar	envbads[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"RANDOM",
	"SECONDS",
	nullptr
} ;

static constexpr cint	sigigns[] = {
	SIGTERM,
	SIGINT,
	SIGHUP,
	SIGPIPE,
#ifdef SIGPOLL
	SIGPOLL,
#endif
#if	defined(SIGXFSZ)
	SIGXFSZ,
#endif
	0
} ;

static constexpr cint	sigdefs[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
#ifdef SIGPOLL
	SIGPOLL,
#endif
	0
} ;

static constexpr cint	sigouts[] = {
	SIGTTOU,
	0
} ;

static vars		var ;

static constexpr cchar	nullfname[] = NULLFNAME ;


/* exported variables */


/* exported subroutines */

int spawnproc(scon *psap,cchar *fname,cchar **argv,cchar **envv) noex {
	static int	rsv = mkvars() ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		pid = 0 ;
	cchar		*efname = fname ;
	char		pwd[MAXPATHLEN + 1] = { 0 } ;
	char		pbuf[MAXPATHLEN + 1] ;

	if (psap == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;
	if (fname[0] == '\0') return SR_INVALID ;


	if ((rs = rsv) >= 0) {
	if (fname[0] != '/') {
	    if ((rs = findprog(pwd,pbuf,fname)) > 0) {
	        efname = pbuf ;
	    }
	} else {
	    rs = perm(fname,-1,-1,nullptr,X_OK) ;
	}
	if (rs >= 0) {
	    envhelp	e, *ehp = &e ;
	    if ((rs = envhelp_start(ehp,envbads,envv)) >= 0) {
	        if ((rs = envhelp_load(ehp,pwd,efname,argv)) >= 0) {
	            mainv	ev{} ;
	            if ((rs = envhelp_getvec(ehp,&ev)) >= 0) {
	                rs = spawnproc_pipes(psap,efname,argv,ev) ;
	                pid = rs ;
	            }
	        } /* end if (envhelp_load) */
	        rs1 = envhelp_finish(ehp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (envhelp_start) */
	} /* end if (ok) */
	} /* end if (mkvars) */

	return (rs >= 0) ? pid : rs ;
}
/* end subroutine (spawnproc) */


/* local subroutines */

static int spawnproc_pipes(scon *psap,cchar *fname,
		mainv argv,mainv ev) noex {
	int		rs ;
	int		pid = 0 ;
	int		con[2] ;
	int		dupes[3] ;
	int		pipes[3][2] ;

	for (int i = 0 ; i < 3 ; i += 1) {
	    pipes[i][0] = -1 ;
	    pipes[i][1] = -1 ;
	    dupes[i] = -1 ;
	} /* end for */

/* process the file descriptors as specified */

	if ((rs = uc_piper(con,3)) >= 0) {
	    cint	pfd = con[0] ;
	    cint	cfd = con[1] ;
	    if ((rs = uc_closeonexec(cfd,true)) >= 0) {

	        for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	            switch (psap->disp[i]) {
	            case SPAWNPROC_DINHERIT:
	                break ;
	            case SPAWNPROC_DCREATE:
	                rs = u_pipe(pipes[i]) ;
	                if (rs < 0) {
	                    pipes[i][0] = -1 ;
	                    pipes[i][1] = -1 ;
	                }
	                break ;
	            case SPAWNPROC_DDUP:
	                if ((rs = dupup(psap->fd[i],3)) >= 0) {
	                    dupes[i] = rs ;
	                }
	                break ;
	            } /* end switch */
	        } /* end for */

	        if (rs >= 0) {
	            if ((rs = ourfork()) == 0) { /* child */
	                u_close(pfd) ;
	                spawnproc_child(psap,fname,argv,ev,cfd,dupes,pipes) ;
	            } else if (rs > 0) { /* parent */
	                pid = rs ;
	                if ((rs = u_close(cfd)) >= 0) {
	                    rs = spawnproc_parfin(psap,pfd,dupes,pipes) ;
	                }
	            } /* end if (ourfork) */
	        } /* end if (ok) */

	        if (rs < 0) { /* error */
	            for (int i = 0 ; i < 3 ; i += 1) {
	                for (int j = 0 ; j < 2 ; j += 1) {
	                    if (pipes[i][j] >= 0) u_close(pipes[i][j]) ;
	                }
	                if (dupes[i] >= 0) {
			    u_close(dupes[i]) ;
			}
	                psap->fd[i] = -1 ;
	            } /* end for */
	        } /* end if (error) */

	    } /* end if uc_closeonexec) */
	    u_close(pfd) ; /* parent-file-descriptor */
	} /* end if (uc_pipes) */

	return (rs >= 0) ? pid : rs ;
}
/* end subroutine (spawnproc_pipes) */

static int spawnproc_parfin(scon *psap,int pfd,int *dupes,
		int (*pipes)[2]) noex {
	int		rs = SR_OK ;
	int		i ;
	int		w ;
	for (i = 0 ; i < 3 ; i += 1) {
	    switch (psap->disp[i]) {
	    case SPAWNPROC_DCREATE:
	        w = (i != 0) ? 1 : 0 ;
	        u_close(pipes[i][w]) ;
	        pipes[i][w] = -1 ;
	        w = (i == 0) ? 1 : 0 ;
	        psap->fd[i] = pipes[i][w] ;
	        break ;
	    default:
	        psap->fd[i] = -1 ;
	        break ;
	    } /* end switch */
	} /* end for */
	for (i = 0 ; i < 3 ; i += 1) {
	    if (dupes[i] >= 0) {
	        u_close(dupes[i]) ;
	        dupes[i] = -1 ;
	    }
	} /* end for */
	if (rs >= 0) {
	    int	res ;
	    if ((rs = u_read(pfd,&res,sizeof(int))) > 0) {
	        rs = res ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (spawnproc_parfin) */

static void spawnproc_child(scon *psap,cchar *fname,
	mainv argv,mainv ev,int cfd,int *dupes,int (*pipes)[2]) noex {
	int		rs = SR_OK ;
	int		opens[3] ;
	mainv		av ;
	cchar		*arg[2] ;

	for (int i = 0 ; i < 3 ; i += 1) {
	    opens[i] = -1 ;
	}

	if ((rs >= 0) && (psap->opts & SPAWNPROC_OIGNINTR)) {
	    sigignores(sigigns) ;
	}

	if (rs >= 0) {
	    if (psap->opts & SPAWNPROC_OSETSID) {
	        setsid() ;
	    } else if (psap->opts & SPAWNPROC_OSETPGRP) {
	        rs = u_setpgid(0,psap->pgrp) ;
	    }
	}

	if ((rs >= 0) && (psap->opts & SPAWNPROC_OSETCTTY)) {
	    SIGIGN	si ;
	    pid_t	pgrp = getpgrp() ;
	    if ((rs = sigign_start(&si,sigouts)) >= 0) {
	        rs = uc_tcsetpgrp(psap->fd_ctty,pgrp) ;
	        sigign_finish(&si) ;
	    } /* end if (sigign) */
	} /* end if (set PGID for controlling terminal) */

	if ((rs >= 0) && (psap->opts & SPAWNPROC_OSIGDEFS)) {
	    sigdefaults(sigdefs) ;
	}

	if (rs >= 0) {
	    int		w ;
	    for (int i = 0 ; i < 3 ; i += 1) {
	        switch (psap->disp[i]) {
	        case SPAWNPROC_DINHERIT:
	            rs = opendevnull(opens,i) ;
	            break ;
	        case SPAWNPROC_DDUP:
		    u_close(i) ;
	            u_dup2(dupes[i],i) ;
	            u_close(dupes[i]) ;
	            break ;
	        case SPAWNPROC_DCREATE:
	            {
	                w = (i == 0) ? 1 : 0 ;
	                u_close(pipes[i][w]) ;
	            }
	            {
	                w = (i != 0) ? 1 : 0 ;
			u_close(i) ;
	                u_dup2(pipes[i][w],i) ;
	                u_close(pipes[i][w]) ;
	            }
	            break ;
	        case SPAWNPROC_DCLOSE:
	        default:
	            break ;
	        case SPAWNPROC_DNULL:
	            u_close(i) ; /* may fail (already closed) */
	            rs = opendevnull(opens,i) ;
	            break ;
	        } /* end switch */
	        if (rs < 0) break ;
	    } /* end for */
	    for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	        switch (psap->disp[i]) {
	        case SPAWNPROC_DCLOSE:
	            u_close(i) ; /* may fail (already closed) */
	            break ;
	        } /* end switch */
	    } /* end for */
	} /* end if (disposition) */
	av = argv ;
	if ((rs >= 0) && (argv == nullptr)) {
	    cchar	*cp ;
	    if (sfbasename(fname,-1,&cp) > 0) {
	        arg[0] = cp ;
	        arg[1] = nullptr ;
	        av = arg ;
	    } else {
	        rs = SR_NOENT ;
	    }
	} /* end if (argument check) */

/* do the exec */

	if ((rs >= 0) && (psap->nice > 0)) {
	    rs = u_nice(psap->nice) ;
	}

	if (rs >= 0) {

#if	CF_ISAEXEC && defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
	    rs = uc_isaexecve(fname,av,ev) ;
#else
	    rs = uc_execve(fname,av,ev) ;
#endif

	    u_write(cfd,&rs,sizeof(int)) ;
	} /* end if (exec) */

	uc_exit(EX_NOEXEC) ;
}
/* end subroutine (spawnproc_child) */

static int envhelp_load(envhelp *ehp,char *pwd,cchar *efname,
		mainv argv) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;

	if ((rs = envhelp_envset(ehp,"_EF",efname,-1)) >= 0) {
	    int		al = -1 ;
	    cchar	*ap = nullptr ;
	    if (argv != nullptr) ap = argv[0] ;
	    if (ap == nullptr) al = sfbasename(efname,-1,&ap) ;
	    if ((rs = envhelp_envset(ehp,"_A0",ap,al)) >= 0) {
		cint	sulen = (strlen(efname)+22) ;
		char		*subuf ;
		if ((rs = uc_malloc((sulen+1),&subuf)) >= 0) {
	    	    const pid_t		pid = uc_getpid() ;
	    	    if ((rs = snshellunder(subuf,sulen,pid,efname)) > 0) {
	       		rs = envhelp_envset(ehp,"_",subuf,rs) ;
	    	    }
	    	    uc_free(subuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (envhelp_envset) */
	} /* end if (envhelp_envset) */

	if (rs >= 0) {
	    static cchar	*var = VARPWD ;
	    if ((rs = envhelp_present(ehp,var,-1,nullptr)) == rsn) {
	        int	pwdl = -1 ;
	        rs = SR_OK ;
	        if (pwd[0] == '\0') {
	            rs = getpwd(pwd,MAXPATHLEN) ;
	            pwdl = rs ;
	        }
	        if (rs >= 0) {
	            rs = envhelp_envset(ehp,var,pwd,pwdl) ;
	        }
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    cchar	*var = VARPATH ;
	    if ((rs = envhelp_present(ehp,var,-1,nullptr)) == rsn) {
	        cint	plen = (2*MAXPATHLEN) ;
	        char	*pbuf ;
	        if ((rs = uc_malloc((plen+1),&pbuf)) >= 0) {
		    cint	cs = _CS_PATH ;
	            if ((rs = uc_confstr(pbuf,plen,cs)) >= 0) {
	                rs = envhelp_envset(ehp,var,pbuf,rs) ;
	            } /* end if */
	            uc_free(pbuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (envhelp_present) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (envhelp_load) */

static int findprog(char *pwd,char *pbuf,cchar *fname) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if ((rs = ids_load(&id)) >= 0) {
	    if (strchr(fname,'/') != nullptr) {
	        if (pwd[0] == '\0') {
	            rs = getpwd(pwd,MAXPATHLEN) ;
	        }
	        if (rs >= 0) {
	            if ((rs = mkpath2(pbuf,pwd,fname)) >= 0) {
	                USTAT	sb ;
	                pl = rs ;
	                if ((rs = uc_stat(pbuf,&sb)) >= 0) {
	                    cint	am = X_OK ;
	                    rs = sperm(&id,&sb,am) ;
	                }
	            } /* end if (mkpath) */
	        } /* end if (ok) */
	    } else {
	        rs = findxfile(&id,pbuf,fname) ;
	        pl = rs ;
	    } /* end if */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprog) */

static int findxfile(ids *idp,char *rbuf,cchar *pn) noex {
	vecstr		plist ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	rbuf[0] = '\0' ;
	if ((rs = vecstr_start(&plist,40,0)) >= 0) {
	    cchar	*path = getenv(VARPATH) ;
	    if ((path != nullptr) && (path[0] != '\0')) {
	        rs = vecstr_addpath(&plist,path,-1) ;
	    } else {
	        rs = vecstr_addcspath(&plist) ;
	    }
	    if (rs >= 0) {
	        rs = getprogpath(idp,&plist,rbuf,pn,-1) ;
	        pl = rs ;
	    }
	    rs1 = vecstr_finish(&plist) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findxfile) */

static int ourfork() noex {
	return uc_fork() ;
}
/* end subroutine (ourfork) */

static int opendevnull(int *opens,int i) noex {
	USTAT		sb ;
	cint		rsbad = SR_BADF ;
	int		rs ;
	if ((rs = u_fstat(i,&sb)) == rsbad) {
	    cint	of = (i == 0) ? O_RDONLY : O_WRONLY ;
	    cmode	om = 0666 ;
	    if ((rs = u_open(nullfname,of,om)) >= 0) {
	        cint	fd = rs ;
	        if (fd != i) {
	            if ((rs = u_dup2(fd,i)) >= 0) {
	                opens[i] = rs ;
	                u_close(fd) ;
	            } /* end if (dup2) */
	        } else {
	            opens[i] = i ;
	        }
	        if (rs < 0) {
	            u_close(fd) ;
	        }
	    } /* end if (open) */
	} /* end if (stat) */
	return rs ;
}
/* end subroutine (opendevnull) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	}
	return rs ;
}
/* end subroutine (mkvars) */



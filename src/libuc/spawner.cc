/* spawner SUPPORT */
/* lang=C++20 */

/* spawn a local program */
/* version %I% last-modified %G% */

#define	CF_ENVSORT	0		/* sort the environment? */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	spawner

	Description:
	Here we spawn a process while specifying options for its
	start-up.

	Synopsis:
	int spawner_start(spawner *op,cchar *fname,mainv argv,mainv envv) noex

	Arguments:
	op		pointer to optional file descriptor dispositions
	fname		program to execute
	argv		arguments to program
	envv		environment to program

	Returns:
	>=0		OK
	<0		error (system-return)

	Implementation notes:  

	Remember that the |pipe(2)| system call creates two pipe
	file descriptors. Both of these file descriptors are open
	for reading and writing on System V UNIX®. However, on BSD
	systems (or older BSD systems assuming that they have not
	yet upgraded to the correct System V behavior) the first
	file descriptor, the one in the zeroth array element, is
	only open for reading. The second file descriptor, the one
	in the oneth array element, is only open for writing. We
	will follow the BSD semantics for portability but hopefully,
	someday, the BSD version of UNIX® will get up to speed with
	the rest of the world!

	Also, note that since we are (very) likely to be running
	in a (hotly) mutli-threaded environment, we have to be quite
	sure that we try to only call async-signal-safe (really
	fork-safe) subroutines after the |fork(2)| and before any
	|exit(2)|. The thing to avoid that might be used by "accident"
	is a hidden |malloc(3c)| (or friends) after the |fork(2)|
	someplace.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucgetpid.h>		/* |uc_getpid(3uc)| */
#include	<ids.h>
#include	<sigign.h>
#include	<xperm.h>
#include	<findxfile.h>
#include	<xfile.h>
#include	<getpwd.h>
#include	<snx.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>
#include	<mapex.h>
#include	<exitcodes.h>

#include	"spawner.h"


/* local defines */

#define	SCMD		struct spawner_cmd

#ifndef	NOFILE
#define	NOFILE		20
#endif

#define	NENV		100

#ifndef	ENVBUFLEN
#define	ENVBUFLEN	(MAXPATHLEN + 40)
#endif

#ifndef	PATHBUFLEN
#define	PATHBUFLEN	(8 * MAXPATHLEN)
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


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		spawner_fddup2(spawner *,int,int) noex ;
}

extern "C" {
    extern int	sigignores(cint *) noex ;
    extern int	sigdefaults(cint *) noex ;
}


/* external variables */


/* local structures */

enum cmds {
	cmd_setctty,
	cmd_seteuid,
	cmd_setegid,
	cmd_sigign,
	cmd_sigdfl,
	cmd_sighold,
	cmd_sigrlse,
	cmd_fdclose,
	cmd_fdnull,
	cmd_fddup,
	cmd_fddup2,
	cmd_fdcreate,
	cmd_overlast
} ;

struct spawner_cmd {
	uid_t		euid ;
	gid_t		egid ;
	int		cmd ;
	int		pfdend ;
	int		pfd ;		/* "parent" FD */
	int		cfd ;		/* "child" FD */
} ;

typedef SCMD		scmd ;
typedef SCMD *		scmdp ;
typedef SCMD **		scmdpp ;


/* forward references */

static int	child(spawner *,scmd **,mainv,mainv) noex ;

static int	procparent(scmd **) noex ;
static int	defaultfds(scmd **) noex ;
static int	closefds(scmd **) noex ;

static int	envhelp_load(envhelp *,char *,cchar *,mainv) noex ;

static int	findprog(char *,char *,cchar *) noex ;
static bool	isUsed(scmd **,int) noex ;
static bool	isChildFD(int) noex ;


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

static constexpr int	sigigns[] = {
	SIGTERM,
	SIGINT,
	SIGHUP,
	SIGPIPE,
	SIGPOLL,
#if	defined(SIGXFSZ)
	SIGXFSZ,
#endif
	0
} ;

static constexpr int	sigdefs[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
	SIGPOLL,
	0
} ;

static constexpr int	sigouts[] = {
	SIGTTOU,
	0
} ;

static constexpr MAPEX	mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ SR_OVERFLOW, EX_SOFTWARE },
	{ SR_DOM, EX_SOFTWARE },
	{ SR_RANGE, EX_SOFTWARE },
	{ SR_NOANODE, EX_SOFTWARE },
	{ SR_NOMEM, EX_OSERR },
	{ SR_PROTO, EX_PROTOCOL },
	{ SR_INTR, EX_INTR },
	{ SR_EXIT, EX_TERM },
	{ 0, 0 }
} ;


/* exported variables */


/* exported subroutines */

int spawner_start(spawner *op,cchar *fname,mainv argv,mainv envv) noex {
	int		rs = SR_OK ;
	cchar		*efname = fname ;
	char		pwd[MAXPATHLEN + 1] = { 0 } ;
	char		pbuf[MAXPATHLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;

/* check for bad input */

	if (fname[0] == '\0') return SR_INVAL ;

	memclear(op) ;
	op->argv = argv ;
	op->pid = -1 ;

/* find the program */

	if (fname[0] != '/') {
	    if ((rs = findprog(pwd,pbuf,fname)) > 0) {
	        efname = pbuf ;
	    }
	} else {
	    rs = perm(fname,-1,-1,nullptr,X_OK) ;
	}

	if (rs >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(efname,-1,&cp)) >= 0) {
	        cint		sz = sizeof(scmd) ;
	        op->execfname = cp ;
	        if ((rs = vecobj_start(&op->cmds,sz,2,0)) >= 0) {
	            envhelp	*ehp = &op->env ;
	            if ((rs = envhelp_start(ehp,envbads,envv)) >= 0) {
	                rs = envhelp_load(ehp,pwd,efname,argv) ;
	                if (rs < 0) {
	                    envhelp_finish(ehp) ;
			}
	            } /* end if (envhelp_start) */
	            if (rs < 0) {
	                vecobj_finish(&op->cmds) ;
		    }
	        } /* end if (vecobj_start) */
	        if (rs < 0) {
	            uc_free(op->execfname) ;
	            op->execfname = nullptr ;
	        }
	    } /* end if (uc_mallocstrw) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (spawner_start) */

int spawner_finish(spawner *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    void	**vpp{} ;
	    if ((rs1 = vecobj_getvec(&op->cmds,&vpp)) >= 0) {
	        scmd	**cv = scmdpp(vpp) ;
	        for (int i = 0 ; cv[i] != nullptr ; i += 1) {
	            scmd	*cmdp = cv[i] ;
	            switch (cmdp->cmd) {
	            case cmd_fdcreate:
	                if (cmdp->pfd >= 0) {
			    u_close(cmdp->pfd) ;
			}
	                break ;
	            } /* end switch */
	        } /* end for */
	    }
	    if (rs >= 0) rs = rs1 ;
	    if (op->execfname != nullptr) {
	        rs1 = uc_free(op->execfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->execfname = nullptr ;
	    }
	    {
	        rs1 = envhelp_finish(&op->env) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(&op->cmds) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (spawner_finish) */

int spawner_setsid(spawner *op) noex {
	if (op == nullptr) return SR_FAULT ;
	if (op->opts & SPAWNER_OSETPGRP) return SR_INVALID ;
	op->opts |= SPAWNER_OSETSID ;
	return SR_OK ;
}
/* end subroutine (spawner_setsid) */

int spawner_setpgrp(spawner *op,pid_t pgrp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (pgrp < 0) return SR_INVALID ;
	if (op->opts & SPAWNER_OSETSID) return SR_INVALID ;
	op->opts |= SPAWNER_OSETPGRP ;
	op->pgrp = pgrp ;
	return SR_OK ;
}
/* end subroutine (spawner_setpgrp) */

int spawner_setctty(spawner *op,int fdterm,pid_t pgrp) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pgrp < 0) return SR_INVALID ;

	op->pgrp = pgrp ;
	if ((rs = dupup(fdterm,3)) >= 0) {
	    scmd	sc{} ;
	    cint	nfd = rs ;
	    sc.cmd = cmd_setctty ;
	    sc.pfd = nfd ;
	    sc.cfd = -1 ;
	    rs = vecobj_add(&op->cmds,&sc) ;
	    if (rs < 0) {
	        u_close(nfd) ;
	    }
	} /* end if (dupup) */

	return rs ;
}
/* end subroutine (spawner_setctty) */

int spawner_seteuid(spawner *op,uid_t uid) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_seteuid ;
	sc.pfd = -1 ;
	sc.cfd = -1 ;
	sc.euid = uid ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_seteuid) */

int spawner_setegid(spawner *op,gid_t gid) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_setegid ;
	sc.pfd = -1 ;
	sc.cfd = -1 ;
	sc.egid = gid ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_setegid) */

int spawner_sigignores(spawner *op) noex {
	if (op == nullptr) return SR_FAULT ;
	op->opts |= SPAWNER_OIGNINTR ;

	return SR_OK ;
}
/* end subroutine (spawner_sigignores) */

int spawner_sigignore(spawner *op,int sn) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_sigign ;
	sc.pfd = -1 ;
	sc.cfd = sn ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_sigignore) */

int spawner_sigdefault(spawner *op,int sn) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_sigdfl ;
	sc.pfd = -1 ;
	sc.cfd = sn ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_sigdefault) */

int spawner_sighold(spawner *op,int sn) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_sighold ;
	sc.pfd = -1 ;
	sc.cfd = sn ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_sighold) */

int spawner_sigrelease(spawner *op,int sn) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_sigrlse ;
	sc.pfd = -1 ;
	sc.cfd = sn ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_sigrelease) */

int spawner_fdclose(spawner *op,int cfd) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_fdclose ;
	sc.pfd = -1 ;
	sc.cfd = cfd ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_fdclose) */

int spawner_fdnull(spawner *op,int of) noex {
	scmd		sc ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	sc.cmd = cmd_fdnull ;
	sc.pfd = of ; /* open-flags */
	sc.cfd = -1 ;
	rs = vecobj_add(&op->cmds,&sc) ;

	return rs ;
}
/* end subroutine (spawner_fdnull) */

int spawner_fddup(spawner *op,int pfd) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs = dupup(pfd,3)) >= 0) {
	    scmd	sc ;
	    cint	nfd = rs ;
	    sc.cmd = cmd_fddup ;
	    sc.pfd = nfd ;
	    sc.cfd = -1 ;
	    rs = vecobj_add(&op->cmds,&sc) ;
	    if (rs < 0) {
	        u_close(nfd) ;
	    }
	} /* end if (dupup) */

	return rs ;
}
/* end subroutine (spawner_fddup) */

int spawner_fddup2(spawner *op,int pfd,int tfd) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs = dupup(pfd,3)) >= 0) {
	    scmd	sc ;
	    int		nfd = rs ;
	    sc.cmd = cmd_fddup2 ;
	    sc.pfd = nfd ;
	    sc.cfd = tfd ;
	    rs = vecobj_add(&op->cmds,&sc) ;
	    if (rs < 0) {
	        u_close(nfd) ;
	    }
	} /* end if (dupup) */

	return rs ;
}
/* end subroutine (spawner_fddup2) */

int spawner_fddupto(spawner *op,int pfd,int tfd) noex {
	return spawner_fddup2(op,pfd,tfd) ;
}
/* end subroutine (spawner_fddupto) */

int spawner_fdcreate(spawner *op,int cfd) noex {
	scmd		sc ;
	int		rs ;
	int		pipes[2] ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs = uc_piper(pipes,0,3)) >= 0) {
	    sc.cmd = cmd_fdcreate ;
	    sc.pfdend = pipes[0] ;
	    sc.pfd = pipes[1] ;
	    sc.cfd = cfd ;
	    rs = vecobj_add(&op->cmds,&sc) ;
	    if (rs < 0) {
	        int	i ;
	        for (i = 0 ; i < 2 ; i += 1) u_close(pipes[i]) ;
	    }
	} /* end if */

	return (rs >= 0) ? pipes[0] : rs ;
}
/* end subroutine (spawner_fdcreate) */

int spawner_envset(spawner *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (op && kp && vp) {
	    rs = envhelp_envset(&op->env,kp,vp,vl) ;
	}
	return rs ;
}
/* end subroutine (spawner_envset) */

int spawner_run(spawner *op) noex {
	int		rs = SR_OK ;
	cchar		*args[2] ;
	mainv		av ;

	if (op == nullptr) return SR_FAULT ;

	av = op->argv ;
	if ((av == nullptr) || (av[0] == nullptr)) {
	    av = args ;
	    args[0] = op->execfname ;
	    args[1] = nullptr ;
	}

/* sort all environment variables */

#if	CF_ENVSORT
	if (rs >= 0)
	    rs = envhelp_sort(&op->env) ;
#endif

	if (rs >= 0) {
	    void	**vpp{} ;
	    if ((rs = vecobj_getvec(&op->cmds,&vpp)) >= 0) {
	        scmd	**cv = scmdpp(vpp) ;
		mainv	ev ;
	        if ((rs = envhelp_getvec(&op->env,&ev)) >= 0) {
	    	    if ((rs = uc_fork()) == 0) {
	        	int	ex ;
	                rs = child(op,cv,av,ev) ;
	        	if (isNotPresent(rs)) {
	                    ex = EX_CANTCREAT ;
	                } else if (isIOError(rs)) {
	                    ex = EX_IOERR ;
	                } else {
	                    ex = mapex(mapexs,rs) ;
		        }
	                u_exit(ex) ; /* no STDIO flush, no exit-handlers */
	            } /* end if (child) */
	            if (rs >= 0) {
	                op->pid = (pid_t) rs ;
	                rs = procparent(cv) ;
	            }
		} /* end if (envhelp_getvec) */
	    } /* end if (vecobj_getvec) */
	} /* end if (ok) */

	if (rs >= 0) rs = op->pid ;

	return rs ;
}
/* end subroutine (spawner_run) */

int spawner_wait(spawner *op,int *csp,int opts) noex {
	int		rs = SR_INVALID ;
	if (op->pid >= 0) {
	    rs = u_waitpid(op->pid,csp,opts) ;
	}
	return rs ;
}
/* end subroutine (spawner_wait) */


/* local subroutines */

static int child(spawner *op,scmd **cv,mainv av,mainv ev) noex {
	cint		opts = op->opts ;
	int		rs = SR_OK ;
	cchar		*efname = op->execfname ;

	if ((rs >= 0) && (opts & SPAWNER_OIGNINTR)) {
	    rs = sigignores(sigigns) ;
	}

	if (rs >= 0) {
	    if (opts & SPAWNER_OSETSID) {
	        rs = u_setsid() ;
	    } else if (opts & SPAWNER_OSETPGRP) {
	        rs = u_setpgid(0,op->pgrp) ;
	    }
	}

	for (int i = 0 ; (rs >= 0) && (cv[i] != nullptr) ; i += 1) {
	    scmd	*cmdp = cv[i] ;
	    switch (cmdp->cmd) {
	    case cmd_setctty:
	        {
	            SIGIGN	si ;
	            pid_t	pgrp = getpgrp() ;
	            if ((rs = sigign_start(&si,sigouts)) >= 0) {
	                rs = uc_tcsetpgrp(cmdp->pfd,pgrp) ;
	                u_close(cmdp->pfd) ;
	                cmdp->pfd = -1 ;
	                sigign_finish(&si) ;
	            } /* end if (sigign) */
	        }
	        break ;
	    case cmd_seteuid:
	        rs = u_seteuid(cmdp->euid) ;
	        break ;
	    case cmd_setegid:
	        rs = u_setegid(cmdp->egid) ;
	        break ;
	    case cmd_sigign:
	        rs = uc_sigignore(cmdp->cfd) ;
	        break ;
	    case cmd_sigdfl:
	        rs = uc_sigdefault(cmdp->cfd) ;
	        break ;
	    case cmd_sighold:
	        rs = uc_sighold(cmdp->cfd) ;
	        break ;
	    case cmd_sigrlse:
	        rs = uc_sigrelease(cmdp->cfd) ;
	        break ;
	    case cmd_fdclose:
	        rs = u_close(cmdp->cfd) ;
	        cmdp->cfd = -1 ;
	        break ;
	    case cmd_fdnull:
	        {
	            cint	of = cmdp->pfd ; /* open-falgs */
	            rs = u_open(NULLFNAME,of,0666) ;
	        }
	        break ;
	    case cmd_fddup:
	        rs = u_dup(cmdp->pfd) ;
	        u_close(cmdp->pfd) ;
	        cmdp->pfd = -1 ;
	        break ;
	    case cmd_fddup2:
	        if (cmdp->pfd != cmdp->cfd) {
	            rs = u_dup2(cmdp->pfd,cmdp->cfd) ;
	            u_close(cmdp->pfd) ;
	        }
	        cmdp->pfd = -1 ;
	        break ;
	    case cmd_fdcreate:
	        if (cmdp->pfd != cmdp->cfd) {
	            rs = u_dup2(cmdp->pfd,cmdp->cfd) ;
	            u_close(cmdp->pfd) ;
	        }
	        cmdp->pfd = -1 ;
	        u_close(cmdp->pfdend) ;
	        cmdp->pfdend = -1 ;
	        break ;
	    } /* end switch */
	} /* end for */

	if ((rs >= 0) && (op->opts & SPAWNER_OSIGDEFS)) {
	    rs = sigdefaults(sigdefs) ;
	}

	if (rs >= 0) {
	    if ((rs = closefds(cv)) >= 0) {
	        if ((rs = defaultfds(cv)) >= 0) {
	            rs = uc_execve(efname,av,ev) ;
	        }
	    }
	}

	return rs ;
}
/* end subroutine (child) */

static int defaultfds(scmd **cv) noex {
	cint		rsbadf = SR_BADF ;
	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	    if (! isUsed(cv,i)) {
	        if ((rs = u_fcntl(i,F_GETFD,0)) == rsbadf) {
	            cint of = (i == 0) ? O_RDONLY : O_WRONLY ;
	            rs = u_open(NULLFNAME,of,0666) ;
	        }
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (defaultfds) */

static int closefds(scmd **cv) noex {
	for (int i = 3 ; i < NOFILE ; i += 1) {
	    if (! isUsed(cv,i)) u_close(i) ;
	}
	return SR_OK ;
}
/* end subroutine (closefds) */

static int procparent(scmd **cv) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; cv[i] != nullptr ; i += 1) {
	    scmd	*cmdp = cv[i] ;
	    switch (cmdp->cmd) {
	    case cmd_setctty:
	    case cmd_fdcreate:
	    case cmd_fddup:
	    case cmd_fddup2:
	        rs = u_close(cmdp->pfd) ;
	        cmdp->pfd = -1 ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (procparent) */

static int envhelp_load(envhelp *ehp,char *pwd,cchar *efname,mainv argv) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = envhelp_envset(ehp,"_EF",efname,-1)) >= 0) {
	    int		al = -1 ;
	    cchar	*ap = nullptr ;
	    if (argv != nullptr) {
		ap = argv[0] ;
	    }
	    if (ap == nullptr) {
		al = sfbasename(efname,-1,&ap) ;
	    }
	    if ((rs = envhelp_envset(ehp,"_A0",ap,al)) >= 0) {
	        cint	sulen = (MAXPATHLEN+22) ;
	        char	*subuf ;
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
	    cchar	*var = VARPWD ;
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
		    cint	req = _CS_PATH ;
	            if ((rs = uc_sysconfstr(pbuf,plen,req)) >= 0) {
		        rs = envhelp_envset(ehp,var,pbuf,rs) ;
	            } /* end if */
	            rs1 = uc_free(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
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

static bool isUsed(scmd **cv,int fd) noex {
	bool		f = false ;
	for (int i = 0 ; cv[i] != nullptr ; i += 1) {
	    scmd	*cmdp = cv[i] ;
	    if (isChildFD(cmdp->cmd)) {
	        f = (cmdp->cfd == fd) ;
	        if (f) break ;
	    }
	} /* end for */
	return f ;
}
/* end subroutine (isUsed) */

static bool isChildFD(int cmd) noex {
	bool		f = false ;
	switch (cmd) {
	case cmd_fddup2:
	case cmd_fdcreate:
	    f = TRUE ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isChildFD) */



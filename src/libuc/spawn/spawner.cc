/* spawner SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* spawn a local program */
/* version %I% last-modified %G% */

#define	CF_ENVSORT	1		/* sort the environment? */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
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
	|exit(2)|.  The thing to avoid that might be used by "accident"
	is a hidden |malloc(3c)| (or friends) after the |fork(2)|
	someplace.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC |uc_getpid(3uc)| */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucsig.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<sigignores.h>		/* LIBUC */
#include	<sigign.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<findfilex.h>		/* LIBUC */
#include	<xfile.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<iserror.h>		/* LIBUC */
#include	<mapex.h>		/* LIBU */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |NOFILE| */

#include	"spawner.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	SCMD		struct spawner_cmd

#ifndef	CF_ENVSORT
#define	CF_ENVSORT	0		/* sort the environment? */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
    extern int uc_piper(int *,int,int) noex ;
    extern int uc_closeonexec(int,int) noex ;
    extern int uc_tcsetpgrp(int,pid_t) noex ;
    extern int uc_isaexecve(cchar *,mainv,mainv) noex ;
    extern int uc_execve(cchar *,mainv,mainv) noex ;
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_fork() noex ;
    extern int uc_exit(int) noex ;
} /* end extern (C) */

extern "C" {
    extern int	sigdefaults(cint *) noex ;
    extern int	dupup(int,int) noex ;
} /* end extern (C) */


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
	cmd_fddupover,
	cmd_fdcreate,
	cmd_overlast
} ; /* end enum */

struct spawner_cmd {
	uid_t		euid ;
	gid_t		egid ;
	int		cmd ;
	int		pfdend ;
	int		pfd ;		/* "parent" FD */
	int		cfd ;		/* "child" FD */
} ; /* end struct */

typedef SCMD		scmd ;
typedef SCMD *		scmdp ;
typedef SCMD **		scmdpp ;

namespace {
    struct vars {
	int		maxpathlen ;
	int mkvars() noex {
	    int		rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
		maxpathlen = rs ;
	    }
	    return rs ;
	} ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int spawner_ctor(spawner *op,Args ... args) noex {
    	SPAWNER		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->cmdp = new(nothrow) vecobj) != np) ylikely {
		if ((op->envp = new(nothrow) envhelp) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-envhelp) */
		if (rs < 0) {
		    delete op->cmdp ;
		    op->cmdp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (spawner_ctor) */

local int spawner_dtor(spawner *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->envp) ylikely {
		delete op->envp ;
		op->envp = nullptr ;
	    }
	    if (op->cmdp) ylikely {
		delete op->cmdp ;
		op->cmdp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (spawner_dtor) */

template<typename ... Args>
local inline int spawner_magic(spawner *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SPAWNER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (spawner_magic) */

local int	spawner_runer(spawner *) noex ;
local int	spawner_child(spawner *,scmd **,mainv,mainv) noex ;

local int	procparent(scmd **) noex ;
local int	defaultfds(scmd **) noex ;
local int	closefds(scmd **) noex ;

local int	envhelp_load(envhelp *,char *,cchar *,mainv) noex ;

local int	findprog(char *,char *,cchar *) noex ;
local bool	isUsed(scmd **,int) noex ;
local bool	isChildFD(int) noex ;


/* local variables */

constexpr cpcchar	envbads[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"RANDOM",
	"SECONDS",
	nullptr
} ; /* end array */

constexpr int		sigigns[] = {
	SIGTERM,
	SIGINT,
	SIGHUP,
	SIGPIPE,
	SIGPOLL,
	SIGXFSZ,
	0
} ; /* end array */

constexpr int		sigdefs[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
	SIGPOLL,
	0
} ; /* end array */

constexpr int		sigouts[] = {
	SIGTTOU,
	0
} ; /* end array */

constexpr mapex_map	mapexs[] = {
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
} ; /* end array */

static vars		var ;
static cchar		*nullfname = sysword.w_devnull ;
constexpr bool		f_envsort = CF_ENVSORT ;


/* exported variables */

const spawnerms		spawnerm ;


/* exported subroutines */

namespace {
    struct starter {
	spawner		*op ;
	cchar		*fn ;
	mainv		av ;
	mainv		ev ;
	char		*a{} ;
	char		*pwd ;
	char		*pbuf ;
	starter(spawner *o,cc *f,mv µa,mv e) noex : op(o), fn(f) {
	    av = µa ;
	    ev = e ;
	} ; /* end ctor */
	operator int () noex ;
	int starts(cchar *) noex ;
    } ; /* end struct (starter) */
} /* end namespace */

int spawner_start(spawner *op,cchar *fn,mainv av,mainv ev) noex {
	int		rs ;
	if ((rs = spawner_ctor(op,fn)) >= 0) ylikely {
	    rs = SR_INVAL ;
	    if (fn[0]) {
		if (static cint	rsv = var.mkvars() ; (rs = rsv) >= 0) ylikely {
		    op->argv = av ;
		    op->pid = -1 ;
		    if (starter so(op,fn,av,ev) ; (rs = so) >= 0) ylikely {
			op->magval = SPAWNER_MAGIC ;
		    }
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		spawner_dtor(op) ;
	    }
	} /* end if (spawner_ctor) */
	return rs ;
} /* end subroutine (spawner_start) */

starter::operator int () noex {
    	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
    	int		rs ;
	int		ai = 0 ;
	if ((rs = lm_mall(sz,&a)) >= 0) ylikely {
	    cchar	*efname = fn ;
	    pwd		= (a + ((maxpath + 1) * ai++)) ;
	    pbuf	= (a + ((maxpath + 1) * ai++)) ;
	    /* find the program */
	    if (fn[0] != '/') {
	        if ((rs = findprog(pwd,pbuf,fn)) > 0) {
	            efname = pbuf ;
	        }
	    } else {
	        rs = perm(fn,-1,-1,nullptr,X_OK) ;
	    }
	    if (rs >= 0) {
	        rs = starts(efname) ;
	    }
	    rs = lm_rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (starter::operator) */

int starter::starts(cc *efname) noex {
    	int		rs ;
        if (cchar *cp ; (rs = lm_strw(efname,-1,&cp)) >= 0) ylikely {
            cint            sz = szof(scmd) ;
            op->execfname = cp ;
            if ((rs = vecobj_start(op->cmdp,sz,2,0)) >= 0) ylikely {
                envhelp     *ehp = op->envp ;
                if ((rs = envhelp_start(ehp,envbads,ev)) >= 0) ylikely {
                    rs = envhelp_load(ehp,pwd,efname,av) ;
                    if (rs < 0) {
                        envhelp_finish(ehp) ;
                    } /* end if (error) */
                } /* end if (envhelp_start) */
                if (rs < 0) {
                    vecobj_finish(op->cmdp) ;
                } /* end if (error) */
            } /* end if (vecobj_start) */
            if (rs < 0) {
		void *vp = voidp(op->execfname) ;
                lm_free(vp) ;
                op->execfname = nullptr ;
            } /* end if (error) */
        } /* end if (memory-acquire) */
	return rs ;
} /* end method (starter::starts) */

int spawner_finish(spawner *op) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    if (void **vpp{} ; (rs1 = vecobj_getvec(op->cmdp,&vpp)) >= 0) {
	        scmd	**cva = scmdpp(vpp) ;
	        for (int i = 0 ; cva[i] != nullptr ; i += 1) {
	            scmd	*cmdp = cva[i] ;
	            switch (cmdp->cmd) {
	            case cmd_fdcreate:
	                if (cmdp->pfd >= 0) {
			    u_close(cmdp->pfd) ;
			}
	                break ;
	            } /* end switch */
	        } /* end for */
	    }
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	    if (op->execfname) ylikely {
		void *vp = voidp(op->execfname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->execfname = nullptr ;
	    }
	    if (op->envp) ylikely {
	        rs1 = envhelp_finish(op->envp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->cmdp) ylikely {
	        rs1 = vecobj_finish(op->cmdp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = spawner_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (spawner_finish) */

int spawner_setsid(spawner *op) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    op->opts |= SPAWNER_OSETSID ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_setsid) */

int spawner_setpgrp(spawner *op,pid_t pgrp) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pgrp >= 0) {
	        op->opts |= SPAWNER_OSETPGRP ;
	        op->pgrp = pgrp ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_setpgrp) */

int spawner_setctty(spawner *op,int fdterm,pid_t pgrp) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((fdterm >= 0) && (pgrp >= 0)) {
	        op->pgrp = pgrp ;
	        if ((rs = dupup(fdterm,3)) >= 0) {
	            scmd	sc{} ;
	            cint	nfd = rs ;
	            sc.cmd = cmd_setctty ;
	            sc.pfd = nfd ;
	            sc.cfd = -1 ;
	            rs = vecobj_add(op->cmdp,&sc) ;
	            if (rs < 0) {
	                u_close(nfd) ;
	            } /* end if (error) */
	        } /* end if (dupup) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_setctty) */

int spawner_seteuid(spawner *op,uid_t uid) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_seteuid ;
	    sc.pfd = -1 ;
	    sc.cfd = -1 ;
	    sc.euid = uid ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_seteuid) */

int spawner_setegid(spawner *op,gid_t gid) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_setegid ;
	    sc.pfd = -1 ;
	    sc.cfd = -1 ;
	    sc.egid = gid ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_setegid) */

int spawner_sigignores(spawner *op) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    op->opts |= SPAWNER_OIGNINTR ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_sigignores) */

int spawner_sigignore(spawner *op,int sn) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_sigign ;
	    sc.pfd = -1 ;
	    sc.cfd = sn ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_sigignore) */

int spawner_sigdefault(spawner *op,int sn) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_sigdfl ;
	    sc.pfd = -1 ;
	    sc.cfd = sn ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_sigdefault) */

int spawner_sighold(spawner *op,int sn) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_sighold ;
	    sc.pfd = -1 ;
	    sc.cfd = sn ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_sighold) */

int spawner_sigrelease(spawner *op,int sn) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_sigrlse ;
	    sc.pfd = -1 ;
	    sc.cfd = sn ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_sigrelease) */

int spawner_fdclose(spawner *op,int cfd) noex {
	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_fdclose ;
	    sc.pfd = -1 ;
	    sc.cfd = cfd ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_fdclose) */

int spawner_fdnull(spawner *op,int of) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    scmd	sc{} ;
	    sc.cmd = cmd_fdnull ;
	    sc.pfd = of ; /* open-flags */
	    sc.cfd = -1 ;
	    rs = vecobj_add(op->cmdp,&sc) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_fdnull) */

int spawner_fddup(spawner *op,int pfd) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    if ((rs = dupup(pfd,3)) >= 0) ylikely {
	        scmd	sc{} ;
	        cint	nfd = rs ;
	        sc.cmd = cmd_fddup ;
	        sc.pfd = nfd ;
	        sc.cfd = -1 ;
	        rs = vecobj_add(op->cmdp,&sc) ;
	        if (rs < 0) {
	            u_close(nfd) ;
	        } /* end if (error) */
	    } /* end if (dupup) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_fddup) */

int spawner_fddupover(spawner *op,int pfd,int tfd) noex {
    	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    if ((rs = dupup(pfd,3)) >= 0) ylikely {
	        scmd	sc{} ;
	        int	nfd = rs ;
	        sc.cmd = cmd_fddupover ;
	        sc.pfd = nfd ;
	        sc.cfd = tfd ;
	        rs = vecobj_add(op->cmdp,&sc) ;
	        if (rs < 0) {
	            u_close(nfd) ;
	        } /* end if (error) */
	    } /* end if (dupup) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_fddupover) */

int spawner_fddupto(spawner *op,int pfd,int tfd) noex {
	return spawner_fddupover(op,pfd,tfd) ;
} /* end subroutine (spawner_fddupto) */

int spawner_fdcreate(spawner *op,int cfd) noex {
    	int		rs ;
	int		fd = 0 ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    int		pipes[2] ;
	    if ((rs = uc_piper(pipes,0,3)) >= 0) ylikely {
	        scmd	sc{} ;
	        sc.cmd = cmd_fdcreate ;
	        sc.pfdend = pipes[0] ;
	        sc.pfd = pipes[1] ;
	        sc.cfd = cfd ;
	        if ((rs = vecobj_add(op->cmdp,&sc)) >= 0) ylikely {
		    fd = pipes[0] ;
		}
	        if (rs < 0) {
	            for (int i = 0 ; i < 2 ; i += 1) {
			u_close(pipes[i]) ;
		    }
	        } /* end if (error handling) */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (spawner_fdcreate) */

int spawner_envset(spawner *op,cchar *kp,cchar *vp,int vl) noex {
    	int		rs ;
	if ((rs = spawner_magic(op,kp,vp)) >= 0) ylikely {
	    rs = envhelp_envset(op->envp,kp,vp,vl) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_envset) */

int spawner_run(spawner *op) noex {
	int		rs ;
	if ((rs = spawner_magic(op)) >= 0) ylikely {
	    rs = spawner_runer(op) ;
	}
	return rs ;
} /* end subroutine (spawner_run) */

local int spawner_runer(spawner *op) noex {
    	int		rs = SR_OK ;
	cchar		*args[2] ;
	mainv		av = op->argv ;
	if ((av == nullptr) || (av[0] == nullptr)) {
	    av = args ;
	    args[0] = op->execfname ;
	    args[1] = nullptr ;
	}
	/* sort all environment variables */
	if_constexpr (f_envsort) {
	    if (rs >= 0) {
	        rs = envhelp_sort(op->envp) ;
	    }
	} /* end if_constexpr (f_envsort) */
	if (rs >= 0) ylikely {
	    if (void **vpp{} ; (rs = vecobj_getvec(op->cmdp,&vpp)) >= 0) {
	        scmd	**cva = scmdpp(vpp) ;
		if (mainv ev{} ; (rs = envhelp_getvec(op->envp,&ev)) >= 0) {
	    	    if ((rs = uc_fork()) == 0) {
	        	int	ex ;
	                if ((rs = spawner_child(op,cva,av,ev)) >= 0) {
	                    ex = EX_UNKNOWN ;
			} else if (isNotPresent(rs)) {
	                    ex = EX_CANTCREAT ;
	                } else if (isIOError(rs)) {
	                    ex = EX_IOERR ;
	                } else {
	                    ex = mapex(mapexs,rs) ;
		        }
	                u_exit(ex) ; /* no STDIO flush, no exit-handlers */
		    } else if (rs >= 0) {
	                op->pid = pid_t(rs) ;
	                rs = procparent(cva) ;
	            }
		} /* end if (envhelp_getvec) */
		if (rs >= 0) rs = op->pid ;
	    } /* end if (vecobj_getvec) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (spawner_runer) */

int spawner_wait(spawner *op,int *csp,int opts) noex {
    	int		rs ;
	if ((rs = spawner_magic(op,csp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (op->pid >= 0) {
	        rs = u_waitpid(op->pid,csp,opts) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (spawner_wait) */


/* local subroutines */

local int spawner_child(spawner *op,scmd **cva,mainv av,mainv ev) noex {
	cint		opts = op->opts ;
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*efname = op->execfname ;
	if ((rs >= 0) && (opts & SPAWNER_OIGNINTR)) {
	    rs = sigignores(sigigns) ;
	}
	if ((rs >= 0) && (op->opts & SPAWNER_OSIGDEFS)) {
	    rs = sigdefaults(sigdefs) ;
	}
	if (rs >= 0) ylikely {
	    if (opts & SPAWNER_OSETSID) {
	        rs = u_setsid() ;
	    } else if (opts & SPAWNER_OSETPGRP) {
	        rs = u_setpgid(0,op->pgrp) ;
	    }
	} /* end if (ok) */
	for (int i = 0 ; (rs >= 0) && (cva[i] != nullptr) ; i += 1) {
	    scmd	*cmdp = cva[i] ;
	    switch (cmdp->cmd) {
	    case cmd_setctty:
	        {
	            const pid_t	pgrp = getpgrp() ;
	            if (sigign si ; (rs = sigign_start(&si,sigouts)) >= 0) {
	                rs = uc_tcsetpgrp(cmdp->pfd,pgrp) ;
			{
	                    u_close(cmdp->pfd) ;
	                    cmdp->pfd = -1 ;
			}
	                rs1 = sigign_finish(&si) ;
			if (rs >= 0) rs = rs1 ;
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
	            cint	of = cmdp->pfd ; /* open-flags */
	            rs = u_open(nullfname,of,0666) ;
	        }
	        break ;
	    case cmd_fddup:
	        rs = u_dup(cmdp->pfd) ;
	        u_close(cmdp->pfd) ;
	        cmdp->pfd = -1 ;
	        break ;
	    case cmd_fddupover:
	        if (cmdp->pfd != cmdp->cfd) {
	            rs = u_dupover(cmdp->pfd,cmdp->cfd) ;
	            u_close(cmdp->pfd) ;
	        }
	        cmdp->pfd = -1 ;
	        break ;
	    case cmd_fdcreate:
	        if (cmdp->pfd != cmdp->cfd) {
	            rs = u_dupover(cmdp->pfd,cmdp->cfd) ;
	            u_close(cmdp->pfd) ;
	        }
	        cmdp->pfd = -1 ;
	        u_close(cmdp->pfdend) ;
	        cmdp->pfdend = -1 ;
	        break ;
	    } /* end switch */
	} /* end for */
	if (rs >= 0) ylikely {
	    if ((rs = closefds(cva)) >= 0) {
	        if ((rs = defaultfds(cva)) >= 0) {
	            rs = uc_execve(efname,av,ev) ;
	        }
	    }
	} /* end if (ok) */
	return rs ;
} /* end subroutine (spawner_child) */

local int defaultfds(scmd **cva) noex {
	cint		rsbadf = SR_BADF ;
	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	    if (! isUsed(cva,i)) {
	        if ((rs = u_fcntl(i,F_GETFD,0)) == rsbadf) {
	            cint of = (i == 0) ? O_RDONLY : O_WRONLY ;
	            rs = u_open(nullfname,of,0666) ;
	        }
	    }
	} /* end for */
	return rs ;
} /* end subroutine (defaultfds) */

local int closefds(scmd **cva) noex {
	for (int i = 3 ; i < NOFILE ; i += 1) {
	    if (! isUsed(cva,i)) {
		u_close(i) ;
	    }
	} /* end for */
	return SR_OK ;
} /* end subroutine (closefds) */

local int procparent(scmd **cva) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; cva[i] != nullptr ; i += 1) {
	    scmd	*cmdp = cva[i] ;
	    switch (cmdp->cmd) {
	    case cmd_setctty:
	    case cmd_fdcreate:
	    case cmd_fddup:
	    case cmd_fddupover:
	        rs = u_close(cmdp->pfd) ;
	        cmdp->pfd = -1 ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end subroutine (procparent) */

local int envhelp_load(envhelp *ehp,char *pwd,cchar *efname,mainv argv) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = envhelp_envset(ehp,"_EF",efname,-1)) >= 0) ylikely {
	    int		al = -1 ;
	    cchar	*ap = nullptr ;
	    if (argv != nullptr) {
		ap = argv[0] ;
	    }
	    if (ap == nullptr) {
		al = sfbasename(efname,-1,&ap) ;
	    }
	    if ((rs = envhelp_envset(ehp,"_A0",ap,al)) >= 0) ylikely {
	        cint	sulen = (var.maxpathlen + 22) ;
	        if (char *subuf ; (rs = lm_mall((sulen+1),&subuf)) >= 0) {
		    if ((rs = ucpid) >= 0) {
			const pid_t	pid = pid_t(rs) ;
	                if ((rs = snshellunder(subuf,sulen,pid,efname)) > 0) {
	                    rs = envhelp_envset(ehp,"_",subuf,rs) ;
	                }
		    } /* end if (ucpid) */
	            rs = lm_rsfree(rs,subuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (envhelp_envset) */
	} /* end if (envhelp_envset) */
	if (rs >= 0) ylikely {
	    cchar	*vname = varname.pwd ;
	    if ((rs = envhelp_present(ehp,vname,-1,nullptr)) == rsn) {
	        int	pwdl = -1 ;
	        rs = SR_OK ;
	        if (pwd[0] == '\0') {
	            rs = getpwd(pwd,var.maxpathlen) ;
	            pwdl = rs ;
	        }
	        if (rs >= 0) {
	            rs = envhelp_envset(ehp,vname,pwd,pwdl) ;
	        }
	    } /* end if (envhelp_present) */
	} /* end if (ok) */
	if (rs >= 0) ylikely {
	    cchar	*vname = varname.pwd ;
	    if ((rs = envhelp_present(ehp,vname,-1,nullptr)) == rsn) {
	        cint	plen = (4 * var.maxpathlen) ;
	        if (char *pbuf ; (rs = lm_mall((plen+1),&pbuf)) >= 0) ylikely {
		    cint	req = _CS_PATH ;
	            if ((rs = uc_sysconfstr(req,pbuf,plen)) >= 0) {
		        rs = envhelp_envset(ehp,vname,pbuf,rs) ;
	            } /* end if */
	            rs1 = lm_free(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (envhelp_present) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (envhelp_load) */

local int findprog(char *pwd,char *pbuf,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if (ids id ; (rs = id.load) >= 0) ylikely {
	    if (strchr(fname,'/') != nullptr) {
	        if (pwd[0] == '\0') {
	            rs = getpwd(pwd,var.maxpathlen) ;
	        }
	        if (rs >= 0) ylikely {
	            if ((rs = mkpath2(pbuf,pwd,fname)) >= 0) ylikely {
	                pl = rs ;
			if (ustat sb ; (rs = uc_stat(pbuf,&sb)) >= 0) {
			    cint	am = X_OK ;
			    rs = permids(&id,&sb,am) ;
		        }
	            } /* end if (mkpath) */
	        } /* end if (ok) */
	    } else {
	        rs = findfilex(&id,pbuf,fname) ;
	        pl = rs ;
	    } /* end if */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (findprog) */

local bool isUsed(scmd **cva,int fd) noex {
	bool		f = false ;
	for (int i = 0 ; cva[i] != nullptr ; i += 1) {
	    scmd	*cmdp = cva[i] ;
	    if (isChildFD(cmdp->cmd)) {
	        f = (cmdp->cfd == fd) ;
	        if (f) break ;
	    }
	} /* end for */
	return f ;
} /* end subroutine (isUsed) */

local bool isChildFD(int cmd) noex {
	bool		f = false ;
	switch (cmd) {
	case cmd_fddupover:
	case cmd_fdcreate:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isChildFD) */



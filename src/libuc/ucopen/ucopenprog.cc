/* ucopenprog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* connect to a local program */
/* version %I% last-modified %G% */

#define	CF_ENVLOAD	0		/* |mkprogenv_load()| */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_openprog

	Description:
	This is a dialer to connect to a local program.

	Synopsis:
	int uc_openprog(cc *fname,int of,mainv argv,mainv envv) noex

	Arguments:
	fname		program to execute
	of		options to specify read-only or write-only
	argv		arguments to program
	envv		environment to program

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

	Note:
	On BSD flavored systems, |pipe(2)| does not open both ends
	of the pipe for both reading and writing, so we observe the
	old BSD behavior of the zeroth element FD being only open
	for reading and the oneth element FD only open for writing.

	Importand note on debugging:
	One some (maybe many) OS systems, turning on any debugging
	in this subroutine can cause hangs after the |fork(2)|.
	This is due to the famous (infamous) fork-safety problem
	on many UNIX®i®.  One UNIX® OS that has fork-safe lib-C
	subroutines (for the greater most part) is Solaris®.  They
	(the Solaris® people) seem to be among the only ones who
	took fork-safety seriously in their OS.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getprogpath.h>
#include	<estrings.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mkprogenv.h>
#include	<spawnproc.h>
#include	<xperm.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward reference */

static int	mkepath(char *,cchar *) noex ;
static int	mkfindpath(char *,cchar *) noex ;

#if	CF_ENVLOAD
static int	mkprogenv_load(MKPROGENV *,cchar *,mainv) noex ;
#endif

static int	openproger(cchar *,int,mainv,mainv,int *) noex ;
static int	spawnit(cchar *,int,mainv,mainv,int *) noex ;

static int	accmode(int) noex ;


/* local variables */

enum accmodes {
	accmode_rdonly,
	accmode_wronly,
	accmode_rdwr,
	accmode_overlast
} ;


/* exported variables */


/* exported subroutines */

int uc_openprogerr(cchar *pfn,int of,mainv argv,mainv envv,int *efdp) noex {
	int		rs ;
	int		fd = -1 ;

	if (pfn == nullptr) return SR_FAULT ;

	if (pfn[0] == '\0') return SR_INVALID ;

	while ((pfn[0] == '/') && (pfn[1] == '/')) {
	    pfn += 1 ;
	}

/* argument check */

	if ((rs = accmode(of)) >= 0) {
	    char	ebuf[MAXPATHLEN + 1] ;
	    if ((rs = mkepath(ebuf,pfn)) >= 0) {
		if (rs > 0) pfn = ebuf ;
		rs = openproger(pfn,of,argv,envv,efdp) ;
		fd = rs ;
	    } /* end if (mkepath) */
	} /* end if (accmode) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openprogerr) */

int uc_openprog(cchar *pfn,int of,mainv argv,mainv envv) noex {
	return uc_openprogerr(pfn,of,argv,envv,nullptr) ;
}
/* end subroutine (uc_openprog) */


/* local subroutines */

static int mkepath(char *ebuf,cchar *pfn) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = mkvarpath(ebuf,pfn,-1)) >= 0) {
	    if (rs > 0) {
	        el = rs ;
		pfn = ebuf ;
	    }
	    if (strncmp(pfn,"/u/",3) == 0) {
	        char	tbuf[MAXPATHLEN+1] ;
	        if ((rs = mkuserpath(tbuf,nullptr,pfn,-1)) > 0) {
		    el = rs ;
	            rs = mkpath1(ebuf,tbuf) ;
	            pfn = ebuf ;
		}
	    }
	    if ((rs >= 0) && (strchr(pfn,'/') == nullptr)) {
		cint	rsn = SR_NOENT ;
		if ((rs = perm(pfn,-1,-1,nullptr,X_OK)) == rsn) {
		    cchar	*tp ;
		    if ((tp = strchr(pfn,':')) != nullptr) {
			if (((tp-pfn) == 3) && (strncmp(pfn,"sys",3) == 0)) {
			    pfn = (tp+1) ;
			}
		    }
		        rs = mkfindpath(ebuf,pfn) ;
		        el = rs ;
		} /* end if (perm) */
	    } /* end if (ok) */
	} /* end if (mkvarpath) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkepath) */

static int mkfindpath(char *ebuf,cchar *pfn) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;
	int		el = 0 ;
	if ((rs = ids_load(&id)) >= 0) {
	    if (vecstr ps ; (rs = vecstr_start(&ps,5,0)) >= 0) {
		if ((rs = vecstr_addcspath(&ps)) >= 0) {
		    rs = getprogpath(&id,&ps,ebuf,pfn,-1) ;
		    el = rs ;
		} /* end if (vecstr_addcspath) */
		rs1 = vecstr_finish(&ps) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mkfindpath) */

static int openproger(cc *pfn,int of,mainv argv,mainv envv,int *efdp) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (mkprogenv pe ; (rs = mkprogenv_start(&pe,envv)) >= 0) {
	    if (mainv ev ; (rs = mkprogenv_getvec(&pe,&ev)) >= 0) {
		rs = spawnit(pfn,of,argv,ev,efdp) ;
		fd = rs ;
	    }
	    rs1 = mkprogenv_finish(&pe) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mkprogenv) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openproger) */

#if	CF_ENVLOAD
static int mkprogenv_load(MKPROGENV *pep,cchar *pfn,mainv argv) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mkprogenv_envset(pep,"_EF",pfn,-1)) >= 0) {
	    int		al = -1 ;
	    cchar	*ap = nullptr ;
	    if (argv != nullptr) ap = argv[0] ;
	    if (ap == nullptr) al = sfbasename(pfn,-1,&ap) ;
	    if ((rs = mkprogenv_envset(pep,"_A0",ap,al)) >= 0) {
		cint	sulen = (xstrlen(pfn)+22) ;
		if (char *subuf ; (rs = uc_malloc((sulen+1),&subuf)) >= 0) {
		    if ((rs = ucpid) >= 0 {
	    	        const pid_t	pid = rs ;
	    	        if ((rs = snshellunder(subuf,sulen,pid,pfn)) > 0) {
	        	    rs = mkprogenv_envset(pep,"_",subuf,rs) ;
	    	        }
		    } /* end if (ucgetpid) */
	    	    rs1 = uc_free(subuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (mkprogenv_envset) */
	} /* end if (mkprogenv_envset) */
	return rs ;
}
/* end subroutine (mkprogenv_load) */
#endif /* CF_ENVLOAD */

int spawnit(cchar *pfn,int of,mainv argv,mainv envv,int *fd2p) noex {
	int		rs ;
	int		fd = -1 ;
	if (int pout[2] ; (rs = uc_piper(pout,0,3)) >= 0) {
	    if ((of & O_NDELAY) || (of & O_NONBLOCK)) {
		if ((rs >= 0) && (of & O_NDELAY)) {
		    rs = uc_ndelay(pout[0],true) ;
		}
		if ((rs >= 0) && (of & O_NONBLOCK)) {
		    rs = uc_nonblock(pout[0],true) ;
		}
	    } /* end if (options) */
	    if (rs >= 0) {
		spawnproc_con	ps{} ;
		ps.fd[0] = pout[1] ;
		ps.fd[1] = pout[1] ;
		ps.disp[0] = SPAWNPROC_DDUP ;
		ps.disp[1] = SPAWNPROC_DDUP ;
		if (fd2p != nullptr) {
		    ps.disp[2] = SPAWNPROC_DCREATE ;
		} else {
		    ps.disp[2] = SPAWNPROC_DNULL ;
		}
	        if (of & O_NOCTTY) {
		    ps.opts |= SPAWNPROC_OIGNINTR ;
		    ps.opts |= SPAWNPROC_OSETSID ;
		}
	   	if ((rs = spawnproc(&ps,pfn,argv,envv)) >= 0) {
		    fd = pout[0] ;
		    u_close(pout[1]) ;
		    pout[1] = -1 ;
		    if (fd2p != nullptr) {
			*fd2p = ps.fd[2] ;
		    }
		} /* end if (spawnproc) */
	    } /* end if (ok) */
	    if (rs < 0) {
		int	i ;
		for (i = 0 ; i < 2 ; i += 1) {
		    if (pout[i] >= 0) {
			u_close(pout[i]) ;
			pout[i] = -1 ;
		    }
		} /* end for */
	    } /* end if (error-cleanup) */
	} /* end if (uc_piper) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (spawnit) */

static int accmode(int of) noex {
	cint		am = (of & O_ACCMODE) ;
	int		rs = SR_INVALID ;
	switch (am) {
	case O_RDONLY:
	    rs = accmode_rdonly ;
	    break ;
	case O_WRONLY:
	    rs = accmode_wronly ;
	    break ;
	case O_RDWR:
	    rs = accmode_rdwr ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (accmode) */



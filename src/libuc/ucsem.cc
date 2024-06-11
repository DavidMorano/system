/* ucsem SUPPORT */
/* lang=C++20 */

/* UNIX® Counting Semaphore (UCSEM) */
/* version %I% last-modified %G% */

#define	CF_CONDUNLINK	1		/* conditional unlink */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These are (secretly) POSIX® "named" semaphores!
	These do NOT have the same API as the w/ regular POSIX®
	interface.

	This module provides a sanitized version of the standard
	POSIX® semaphore facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff (memory exhaustion) that
	happens when a poorly configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<semaphore.h>
#include	<cerrno>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getxid.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"ucsem.h"


/* local defines */

#define	UCSEM_PATHPREFIX	"/tmp/ucsem"

#define	UCSEM_CHOWNVAR		_PC_CHOWN_RESTRICTED

#define	UCSEM_USERNAME1		"sys"
#define	UCSEM_USERNAME2		"adm"

#define	UCSEM_GROUPNAME1	"sys"
#define	UCSEM_GROUPNAME2	"sys"

#define	UCSEM_UID	3
#define	UCSEM_GID	3

#define	TO_NOSPC	10
#define	TO_MFILE	10
#define	TO_NFILE	10
#define	TO_INTR		10

#ifndef	CF_CONDUNLINK
#define	CF_CONDUNLINK	0		/* conditional unlink */
#endif


/* external subroutines */


/* forward references */

int		ucsemunlink(cchar *) noex ;
int		unlinkucsem(cchar *) noex ;

template<typename ... Args>
static inline int ucsem_magic(ucsem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UCSEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ucsem_magic) */

static int	ucsemdircheck(cchar *) noex ;
static int	ucsemdiradd(cchar *,mode_t) noex ;
static int	ucsemdirrm(cchar *) noex ;

static int	getucsemgid(void) noex ;


/* local variables */

constexpr bool		f_condunlink = CF_CONDUNLINK ;


/* exported subroutines */

int ucsem_open(ucsem *op,cchar *name,int oflag,mode_t om,uint count) noex {
	int		to_mfile = utimeout[uto_mfile] ;
	int		to_nfile = utimeout[uto_nfile] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && name) {
	    memset(op,0,sizeof(UCSEM)) ;
	    rs = SR_INVALID ;
	    if (name[0]) {
		char	*altname = nullptr ;
		if ((rs = malloc_mn(&altname)) >= 0) {
	            if (name[0] != '/') {
			if ((rs = getbufsize(getbufsize_mn)) >= 0) {
			    cint	mnlen = rs ;
	                    rs = sncpy2(altname,mnlen,"/",name) ;
	                    name = altname ;
			}
	            } /* end if (name does not start w/ '/') */
	            if (rs >= 0) {
	                bool	f_exit = true ;
	                repeat {
	                    rs = SR_OK ;
	                    op->sp = sem_open(name,oflag,om,count) ;
	                    if (op->sp == SEM_FAILED) rs = (- errno) ;
	                    if (rs < 0) {
	                        switch (rs) {
	                        case SR_MFILE:
	                            if (to_mfile-- > 0) {
	                                msleep(1000) ;
		                    } else {
			                f_exit = true ;
		                    }
	                            break ;
	                        case SR_NFILE:
	                            if (to_nfile-- > 0) {
	                                msleep(1000) ;
		                    } else {
			                f_exit = true ;
		                    }
	                            break ;
	                        case SR_NOSPC:
	                            if (to_nospc-- > 0) {
	                                msleep(1000) ;
		                    } else {
			                f_exit = true ;
		                    }
	                            break ;
		                case SR_INTR:
		                    break ;
		                default:
		                    f_exit = true ;
		                    break ;
	                        } /* end switch */
	                    } /* end if (error) */
	                } until ((rs >= 0) || f_exit) ;
	                if (rs >= 0) {
			    char	*np ;
			    if ((rs = malloc_mn(&np)) >= 0) {
				cint	mnlen = rs ;
				op->name = np ;
	                        strwcpy(np,name,mnlen) ;
	                        if (oflag & O_CREAT) ucsemdiradd(name,om) ;
	                        op->magic = UCSEM_MAGIC ;
			    }
	                } /* end if (opened) */
	            } /* end if (ok) */
		    rs1 = uc_free(altname) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mallocxx-altname) */
	    } /* end if (non-zero name) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_open) */

int ucsem_close(ucsem *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ucsem_magic(op)) >= 0) {
		rs = SR_BADFMT ;
	        if (op->sp) {
	            repeat {
	                if ((rs = sem_close(op->sp)) < 0) rs = (- errno) ;
	            } until (rs != SR_INTR) ;
		} /* end if (non-null) */
		if (op->name) {
		    rs1 = uc_free(op->name) ;
		    if (rs >= 0) rs = rs1 ;
		    op->name = nullptr ;
		}
		op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_close) */

int ucsem_trywait(ucsem *op) noex {
	int		rs ;
	if ((rs = ucsem_magic(op)) >= 0) {
	        repeat {
	            if ((rs = sem_trywait(op->sp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_trywait) */

#ifdef	COMMENT
int ucsem_getvalue(ucsem *op,int *rp) noex {
	int		rs ;
	if ((rs = ucsem_magic(op)) >= 0) {
	        repeat {
	            if ((rs = sem_getvalue(op->sp,rp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_getvalue) */
#endif /* COMMENT */

int ucsem_wait(ucsem *op) noex {
	int		rs ;
	if ((rs = ucsem_magic(op)) >= 0) {
	    repeat {
	        if ((rs = sem_wait(op->sp)) < 0) rs = (- errno) ;
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_wait) */

int ucsem_waiti(ucsem *op) noex {
	int		rs ;
	if ((rs = ucsem_magic(op)) >= 0) {
	        if ((rs = sem_wait(op->sp)) < 0) {
	            rs = (- errno) ;
	        }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_waiti) */

int ucsem_post(ucsem *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = ucsem_magic(op)) >= 0) {
	        repeat {
	            if ((rs = sem_post(op->sp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_post) */

int ucsem_unlink(ucsem *op) noex {
	int		rs ;
	if ((rs = ucsem_magic(op)) >= 0) {
		rs = SR_BADFMT ;
		if (op) {
		    rs = SR_OK ;
	            if (op->name[0] != '\0') {
	    		rs = unlinkucsem(op->name) ;
		        if_constexpr (f_condunlink) {
	    		    if (rs >= 0) {
	        		ucsemdirrm(op->name) ;
			    }
			} else {
	    		    ucsemdirrm(op->name) ;
		        }
		    } /* end if (not zero name) */
		} /* end if (not BADFMT) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ucsem_unlink) */


/* OTHER API (but related) */

int ucsemunlink(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        char	*altname = nullptr ;
		rs = SR_OK ;
		if ((rs = malloc_mn(&altname)) >= 0) {
		    cint	mnlen = rs ;
	            if (name[0] != '/') {
	                sncpy2(altname,mnlen,"/",name) ;
	                name = altname ;
	            }
	            repeat {
	                rs = SR_OK ;
	                if ((rs = sem_unlink(name)) < -1) {
			    rs = (- errno) ;
			}
	            } until (rs != SR_INTR) ;
		    if_constexpr (f_condunlink) {
		        if (rs >= 0) {
	    		    ucsemdirrm(name) ;
		        }
		    } else {
		        ucsemdirrm(name) ;
		    }
		    rs1 = uc_free(altname) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mallocxx-altname) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsemunlink) */

int unlinkucsem(cchar *name) noex {
	return ucsemunlink(name) ;
}
/* end subroutine (unlinkucsem) */


/* local subroutines */

static int ucsemdiradd(cchar *name,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    char	*tmpfname{} ;
	    if ((rs = malloc_mp(&tmpfname)) >= 0) {
		cchar	*pp = UCSEM_PATHPREFIX ;
		if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    cint	rsn = SR_NOENT ;
	            if ((rs = u_creat(tmpfname,om)) == rsn) {
	                if ((rs = ucsemdircheck(pp)) >= 0) {
	                    rs = u_creat(tmpfname,om) ;
	                }
	            }
	            if (rs >= 0) u_close(rs) ;
		} /* end if (mkpath2) */
		rs1 = uc_free(tmpfname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mallocxx-tmpfname) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsemdiradd) */

static int ucsemdirrm(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    char	*tmpfname{} ;
	    if ((rs = malloc_mp(&tmpfname)) >= 0) {
	        cchar	*pp = UCSEM_PATHPREFIX ;
	        if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    rs = u_unlink(tmpfname) ;
	        } /* end if (mkpath2) */
	        rs1 = uc_free(tmpfname) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mallocxx-tmpfnam) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsemdirrm) */

static int ucsemdircheck(cchar *pp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    char	*pwbuf{} ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        USTAT	sb ;
	        rs = u_stat(pp,&sb) ;
	        if ((rs < 0) || (! S_ISDIR(sb.st_mode))) {
	    	    rs = u_mkdir(pp,0777) ;
	            if (rs >= 0) {
	                rs = u_chmod(pp,(0777 | S_ISVTX)) ;
		    }
	            if (rs >= 0) {
	        	long	cv ;
	                rs = u_pathconf(pp,UCSEM_CHOWNVAR,&cv) ;
	                if ((rs < 0) || cv) {
	        	    const uid_t	euid = geteuid() ;
	        	    uid_t	uid ;
			    cchar	*un = UCSEM_USERNAME1 ;
	                    rs = uc_getpwnam(&pw,pwbuf,pwlen,un) ;
	                    if (rs < 0) {
			        un = UCSEM_USERNAME2 ;
	                        rs = uc_getpwnam(&pw,pwbuf,pwlen,un) ;
			    }
	                    uid = (rs >= 0) ? pw.pw_uid : UCSEM_UID ;
	                    if (euid != uid) {
			        if ((rs = getucsemgid()) >= 0) {
			            const gid_t	gid = rs ;
	              	            rs = u_chown(pp,uid,gid) ;
			        }
	                    } /* end if (UIDs different) */
	                } /* end if (CHOWN possibly allowed) */
	            } /* end if (was able to CHMOD) */
	        } /* end if (directory did not exist) */
	        rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (getbufsize) */
	return rs ;
}
/* end subroutine (ucsemdircheck) */

static int getucsemgid(void) noex {
	cint		nrs = SR_NOTFOUND ;
	int		rs ;
	cchar		*gn = UCSEM_GROUPNAME1 ;
	if ((rs = getgid_group(gn,-1)) == nrs) {
	    gn = UCSEM_GROUPNAME2 ;
	    if ((rs = getgid_group(gn,-1)) == nrs) {
		rs = UCSEM_GID ;
	    }
	}
	return rs ;
}
/* end subroutine (getucsemgid) */



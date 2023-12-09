/* ucsem */
/* lang=C++20 */

/* UNIX® Counting Semaphore (UCSEM) */

#define	CF_CONDUNLINK	1		/* conditional unlink */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides a sanitized version of the standard POSIX®
	semaphore facility provided with some new UNIX®i.  Some operating
	system problems are managed within these routines for the common stuff
	that happens when a poorly configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<semaphore.h>
#include	<cstring>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<getbufsize.h>
#include	<usupport.h>
#include	<mallocxx.h>
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


/* external subroutines */

extern "C" {
    extern int	mkpath2(char *,const char *,const char *) noex ;
    extern int	sncpy2(char *,int,const char *,const char *) noex ;
    extern int	getgid_group(cchar *,int) noex ;
    extern int	msleep(int) noex ;
    extern char	*strwcpy(char *,const char *,int) noex ;
}


/* forward references */

int		ucsemunlink(cchar *) noex ;
int		unlinkucsem(cchar *) noex ;

static int	ucsemdircheck(cchar *) noex ;
static int	ucsemdiradd(cchar *,mode_t) noex ;
static int	ucsemdirrm(cchar *) noex ;

static int	getucsemgid(void) noex ;


/* exported subroutines */

int ucsem_open(UCSEM *psp,cchar *name,int oflag,mode_t om,uint count) noex {
	int		to_mfile = utimeout[uto_mfile] ;
	int		to_nfile = utimeout[uto_nfile] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (psp && name) {
	    memset(psp,0,sizeof(UCSEM)) ;
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
	                    psp->sp = sem_open(name,oflag,om,count) ;
	                    if (psp->sp == SEM_FAILED) rs = (- errno) ;
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
				psp->name = np ;
	                        strwcpy(np,name,mnlen) ;
	                        if (oflag & O_CREAT) ucsemdiradd(name,om) ;
	                        psp->magic = UCSEM_MAGIC ;
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

int ucsem_close(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == UCSEM_MAGIC) {
		rs = SR_BADFMT ;
	        if (psp->sp) {
	            repeat {
	                if ((rs = sem_close(psp->sp)) < 0) rs = (- errno) ;
	            } until (rs != SR_INTR) ;
		} /* end if (non-null) */
		if (psp->name) {
		    rs1 = uc_free(psp->name) ;
		    if (rs >= 0) rs = rs1 ;
		    psp->name = nullptr ;
		}
		psp->magic = 0 ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_close) */

int ucsem_trywait(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	     rs = SR_NOTOPEN ;
	     if (psp->magic == UCSEM_MAGIC) {
		sem_t	*sp = (psp->sp != NULL) ? psp->sp : &psp->s ;
	        repeat {
	            if ((rs = sem_trywait(sp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_trywait) */

#ifdef	COMMENT
int ucsem_getvalue(UCSEM *psp,int *rp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    if (psp->magic == UCSEM_MAGIC) {
	        sem_t	*sp = (psp->sp != NULL) ? psp->sp : &psp->s ;
	        repeat {
	            if ((rs = sem_getvalue(sp,rp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_getvalue) */
#endif /* COMMENT */

int ucsem_wait(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == UCSEM_MAGIC) {
		sem_t	*sp = (psp->sp != NULL) ? psp->sp : &psp->s ;
	           repeat {
	               if ((rs = sem_wait(sp)) < 0) rs = (- errno) ;
	           } until (rs != SR_INTR) ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_wait) */

int ucsem_waiti(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == UCSEM_MAGIC) {
		sem_t	*sp = (psp->sp != NULL) ? psp->sp : &psp->s ;
	        if ((rs = sem_wait(sp)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_waiti) */

int ucsem_post(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == UCSEM_MAGIC) {
		sem_t	*sp = (psp->sp != NULL) ? psp->sp : &psp->s ;
	        repeat {
	            if ((rs = sem_post(sp)) < 0) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsem_post) */

int ucsem_unlink(UCSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == UCSEM_MAGIC) {
		rs = SR_BADFMT ;
		if (psp) {
		    rs = SR_OK ;
	            if (psp->name[0] != '\0') {
	    		rs = unlinkucsem(psp->name) ;
#if	CF_CONDUNLINK
	    		if (rs >= 0)
	        		ucsemdirrm(psp->name) ;
#else
	    		ucsemdirrm(psp->name) ;
#endif /* CF_CONDUNLINK */
		    } /* end if (not zero name) */
		} /* end if (not BADFMT) */
	    } /* end if (was open) */
	} /* end if (non-null) */
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
	            if ((rs = sem_unlink(name)) < -1) rs = (- errno) ;
	        } until (rs != SR_INTR) ;
#if	CF_CONDUNLINK
	if (rs >= 0)
	    ucsemdirrm(name) ;
#else
	ucsemdirrm(name) ;
#endif /* CF_CONDUNLINK */

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

static int ucsemdiradd(cchar *name,mode_t perm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    char	*tmpfname ;
	    if ((rs = malloc_mp(&tmpfname)) >= 0) {
		cchar	*pp = UCSEM_PATHPREFIX ;
		if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    cint	rsn = SR_NOENT ;
	            if ((rs = u_creat(tmpfname,perm)) == rsn) {
	                if ((rs = ucsemdircheck(pp)) >= 0) {
	                    rs = u_creat(tmpfname,perm) ;
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
	    char	*tmpfname ;
	    if ((rs = malloc_mp(&tmpfname)) >= 0) {
	        cchar	*pp = UCSEM_PATHPREFIX ;
	        if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    {
		        rs = u_unlink(tmpfname) ;
		    }
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
	    struct passwd	pw ;
	    const int		pwlen = getbufsize(getbufsize_pw) ;
	    char		*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        struct ustat	sb ;
	        uid_t		euid, uid ;
	        long		cv ;
	        rs = u_stat(pp,&sb) ;
	        if ((rs < 0) || (! S_ISDIR(sb.st_mode))) {
	    	    rs = u_mkdir(pp,0777) ;
	            if (rs >= 0) {
	                rs = u_chmod(pp,(0777 | S_ISVTX)) ;
		    }
	            if (rs >= 0) {
	                rs = u_pathconf(pp,UCSEM_CHOWNVAR,&cv) ;
	                if ((rs < 0) || cv) {
			    cchar	*un = UCSEM_USERNAME1 ;
	                    euid = geteuid() ;
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
	const int	nrs = SR_NOTFOUND ;
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



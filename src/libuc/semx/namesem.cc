/* namesem SUPPORT (named semaphore) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* POSIX® real-time -- named -- Counting Semaphore (NAMESEM) */
/* version %I% last-modified %G% */

#define	CF_CONDUNLINK	1		/* conditional unlink */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	namesem

	Description:
	This object essentially (secretly) implements a POSIX®
	"named" semaphore!  This object does *not* have the same
	API as the regular POSIX® named-semaphore interface.  This
	module provides a sanitized version of the standard POSIX®
	semaphore facility provided with some new UNIX®i.  Some
	operating system problems are managed within these routines
	for the common stuff, like memory exhaustion.

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |mode_t| */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<semaphore.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getxid.h>
#include	<errtimer.hh>
#include	<posname.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"namesem.h"


/* local defines */

#define	NAMESEM_PATHPREFIX	"/tmp/namesem"

#define	NAMESEM_CHOWNVAR	_PC_CHOWN_RESTRICTED

#define	NAMESEM_USERNAME1	"sys"
#define	NAMESEM_USERNAME2	"adm"

#define	NAMESEM_GROUPNAME1	"sys"
#define	NAMESEM_GROUPNAME2	"sys"

#define	NAMESEM_UID	3
#define	NAMESEM_GID	3

#define	TO_NOSPC	10
#define	TO_MFILE	10
#define	TO_NFILE	10
#define	TO_INTR		10

#ifndef	CF_CONDUNLINK
#define	CF_CONDUNLINK	0		/* conditional unlink */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int namesem_magic(namesem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == NAMESEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (namesem_magic) */

static int	namesemopen(namesem *,cc *,int,mode_t,int) noex ;
static int	namesemclose(namesem *) noex ;

static int	namesemdircheck(cchar *) noex ;
static int	namesemdiradd(cchar *,mode_t) noex ;
static int	namesemdirrm(cchar *) noex ;

static int	getnamesemgid(void) noex ;


/* local variables */

constexpr bool		f_condunlink = CF_CONDUNLINK ;


/* exported variables */


/* exported subroutines */

int namesem_open(namesem *op,cchar *name,int of,mode_t om,uint c) noex {
    	NAMESEM		*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && name) {
	    memclear(hop) ;
	    rs = SR_INVALID ;
	    if (name[0]) {
		cchar	*nn ;
		if (posname pn ; (rs = pn.start(name,-1,&nn)) >= 0) {
		    if ((rs = namesemopen(op,nn,of,om,c)) >= 0) {
			if (char *bp ; (rs = malloc_mn(&bp)) >= 0) {
			    cint	mnlen = rs ;
			    op->name = bp ;
	                    strwcpy(bp,nn,mnlen) ;
	                    if (of & O_CREAT) {
				rs = namesemdiradd(nn,om) ;
			    }
			    if (rs >= 0) {
	                        op->magic = NAMESEM_MAGIC ;
			    }
			    if (rs < 0) {
				uc_free(op->name) ;
				op->name = nullptr ;
			    }
			} /* end if (memory-allocation) */
	            } /* end if (opened) */
		    rs1 = pn.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (posname) */
	    } /* end if (non-zero name) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namesem_open) */

int namesem_close(namesem *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = namesem_magic(op)) >= 0) {
	    {
	        rs1 = namesemclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->name) {
		rs1 = uc_free(op->name) ;
		if (rs >= 0) rs = rs1 ;
		op->name = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_close) */

int namesem_trywait(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	    repeat {
	        if ((rs = sem_trywait(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_trywait) */

int namesem_wait(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	    repeat {
	        if ((rs = sem_wait(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_wait) */

int namesem_waiti(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	   if ((rs = sem_wait(op->sp)) < 0) {
	       rs = (- errno) ;
	   }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_waiti) */

int namesem_post(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	    repeat {
	        if ((rs = sem_post(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_post) */

int namesem_unlink(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	    if (op->name[0] != '\0') {
		rs = unlinknamesem(op->name) ;
		if_constexpr (f_condunlink) {
		    if (rs >= 0) {
			namesemdirrm(op->name) ;
		    }
		} else {
		    namesemdirrm(op->name) ;
		}
	    } /* end if (not zero name) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_unlink) */

int namesem_count(namesem *op) noex {
	int		rs ;
	if ((rs = namesem_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (namesem_count) */


/* OTHER API (but related) */

int namesemunlink(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		cchar	*nn ;
		if (posname pn ; (rs = pn.start(name,-1,&nn)) >= 0) {
	            repeat {
	                if ((rs = sem_unlink(nn)) < 0) {
			    rs = (- errno) ;
			}
	            } until (rs != SR_INTR) ;
		    if_constexpr (f_condunlink) {
		        if (rs >= 0) {
	    		    namesemdirrm(nn) ;
		        }
		    } else {
		        namesemdirrm(nn) ;
		    }
		    rs1 = pn.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (posname) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namesemunlink) */

int unlinknamesem(cchar *name) noex {
	return namesemunlink(name) ;
}
/* end subroutine (unlinknamesem) */


/* local subroutines */

static int namesemopen(namesem *op,cc *name,int of,mode_t om,int c) noex {
	errtimer	to_mfile = utimeout[uto_mfile] ;
	errtimer	to_nfile = utimeout[uto_nfile] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	errtimer	to_nospc = utimeout[uto_nospc] ;
        reterr          r ;
    	int		rs ;
        repeat {
            rs = SR_OK ;
	    if ((op->sp = sem_open(name,of,om,c)) == SEM_FAILED) {
                rs = (- errno) ;
                r(rs) ;                 /* <- default causes exit */
	        switch (rs) {
	        case SR_MFILE:
                    r = to_mfile(rs) ;
		    break ;
	        case SR_NFILE:
                    r = to_nfile(rs) ;
		    break ;
		case SR_NOMEM:
                    r = to_nomem(rs) ;
		    break ;
	        case SR_NOSPC:
                    r = to_nospc(rs) ;
		    break ;
	        case SR_INTR:
		    r(false) ;
	            break ;
	        } /* end switch */
		rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (namesemopen) */

static int namesemclose(namesem *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op->sp) {
	    repeat {
	        if ((rs = sem_close(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	    op->sp = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namesemclose) */

static int namesemdiradd(cchar *name,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    if (char *tmpfname ; (rs = malloc_mp(&tmpfname)) >= 0) {
		cchar	*pp = NAMESEM_PATHPREFIX ;
		if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    cint	rsn = SR_NOENT ;
	            if ((rs = u_creat(tmpfname,om)) == rsn) {
	                if ((rs = namesemdircheck(pp)) >= 0) {
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
/* end subroutine (namesemdiradd) */

static int namesemdirrm(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    if (char *tmpfname ; (rs = malloc_mp(&tmpfname)) >= 0) {
	        cchar	*pp = NAMESEM_PATHPREFIX ;
	        if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
		    rs = u_unlink(tmpfname) ;
	        } /* end if (mkpath2) */
	        rs1 = uc_free(tmpfname) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mallocxx-tmpfnam) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namesemdirrm) */

static int namesemdircheck(cchar *pp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    if (char *pwbuf ; (rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        USTAT	sb ;
	        rs = u_stat(pp,&sb) ;
	        if ((rs < 0) || (! S_ISDIR(sb.st_mode))) {
		    cmode	dm = 0777 ;
	    	    if ((rs = u_mkdir(pp,dm)) >= 0) {
	                rs = u_chmod(pp,(dm | S_ISVTX)) ;
		    }
	            if (rs >= 0) {
	        	long	cv ;
	                rs = u_pathconf(pp,NAMESEM_CHOWNVAR,&cv) ;
	                if ((rs < 0) || cv) {
	        	    const uid_t	euid = geteuid() ;
	        	    uid_t	uid ;
			    cchar	*un = NAMESEM_USERNAME1 ;
	                    rs = uc_getpwnam(&pw,pwbuf,pwlen,un) ;
	                    if (rs < 0) {
			        un = NAMESEM_USERNAME2 ;
	                        rs = uc_getpwnam(&pw,pwbuf,pwlen,un) ;
			    }
	                    uid = (rs >= 0) ? pw.pw_uid : NAMESEM_UID ;
	                    if (euid != uid) {
			        if ((rs = getnamesemgid()) >= 0) {
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
/* end subroutine (namesemdircheck) */

static int getnamesemgid(void) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	cchar		*gn = NAMESEM_GROUPNAME1 ;
	if ((rs = getgid_group(gn,-1)) == rsn) {
	    gn = NAMESEM_GROUPNAME2 ;
	    if ((rs = getgid_group(gn,-1)) == rsn) {
		rs = NAMESEM_GID ;
	    }
	}
	return rs ;
}
/* end subroutine (getnamesemgid) */



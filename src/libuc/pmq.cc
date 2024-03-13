/* pmq SUPPORT (LIBUC) */
/* lang=C++20 */

/* Posix Message Queue (PMQ) */
/* version %I% last-modified %G% */

#define	CF_PMQDIR	0		/* manage message-queue directory */
#define	CF_GETPMQ	0		/* manage message-queue IDs */
#define	CF_UGETPW	0		/* use |ugetpw(3uc)| */

/* revision history:

	= 1999-07-23, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This module provides a sanitized version of the standard
	POSIX� Message-Queue facility provided with some newer
	UNIX�i.  Some operating system problems are managed within
	these routines for the common stuff that happens when a
	poorly configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<libmallocxx.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pmq.h"


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#define	GETPW_UID	ugetpw_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_UGETPW */

#define	PMQ_PATHPREFIX	"/tmp/pmq"

#define	PMQ_CHOWNVAR	_PC_CHOWN_RESTRICTED

#define	PMQ_USERNAME1	"sys"
#define	PMQ_USERNAME2	"adm"

#define	PMQ_GROUPNAME1	"sys"
#define	PMQ_GROUPNAME2	"adm"

#define	PMQ_UID		3
#define	PMQ_GID		3

#ifndef	CF_PMQDIR
#define	CF_PMQDIR	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getgid_group(cchar *,int) noex ;
    extern int	isNotValid(int) noex ;
}


/* local structures */

namespace {
    struct posixhelp ;
    typedef int (posixhelp::*posixhelp_f)(pmq *) noex ;
    struct posixhelp {
	posixhelp_f	m ;
	SIGEVENT	*sep ;
	const pmq_attr	*nattr ;
	pmq_attr	*oattr ;
	cchar		*sbuf ;
	char		*rbuf ;
	uint		*priop ;
	uint		prio ;
	int 		of ;
	int		slen ;
	int		rlen ;
	mode_t		om ;
	posixhelp() = default ;
	posixhelp(int f,mode_t m,const pmq_attr *a) noex {
	    of = f ;
	    om = m ;
	    nattr = a ;
	} ;
	posixhelp(cchar *b,int l,uint p) noex {
	    sbuf = b ;
	    slen = l ;
	    prio = p ;
	} ;
	posixhelp(char *b,int l,uint *p) noex {
	    rbuf = b ;
	    rlen = l ;
	    priop = p ;
	} ;
	posixhelp(pmq_attr *o,const pmq_attr *n = nullptr) noex {
	    oattr = o ;
	    nattr = n ;
	} ;
	posixhelp(SIGEVENT *s) noex {
	    sep = s ;
	} ;
	int operator () (pmq *) noex ;
	int open(pmq *) noex ;
	int close(pmq *) noex ;
	int send(pmq *) noex ;
	int recv(pmq *) noex ;
	int attrset(pmq *) noex ;
	int attrget(pmq *) noex ;
	int notify(pmq *) noex ;
	int unlink(pmq *) noex ;
    } ; /* end struct (posixhelp) */
}


/* forward references */

int		uc_unlinkpmq(cchar *) noex ;

template<typename ... Args>
static inline int pmq_ctor(pmq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->name = nullptr ;
	    op->magic = 0 ;
	}
	return rs ;
}

static inline int pmq_dtor(pmq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}

template<typename ... Args>
static inline int pmq_magic(pmq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PMQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	pmq_nameload(pmq *,cchar *) noex ;
static int	pmq_nameclean(pmq *) noex ;

#if	CF_PMQDIR
static int	pmqdiradd(cchar *,mode_t) noex ;
static int	pmqdirrm(cchar *) noex ;
static int	pmqdircheck(cchar *) noex ;
#endif /* CF_PMQDIR */

#if	CF_GETPMQ
static int	getpmquid(void) noex ;
static int	getpmqgid(void) noex ;
#endif /* CF_GETPMQ */

static constexpr mqd_t mkmqdbad() noex {
	mqd_t	r = 0 ;
	return (~r) ;
}


/* local variables */

static constexpr mqd_t		mqdbad = mkmqdbad() ;


/* exported variables */


/* exported subroutines */

int pmq_open(pmq *op,cchar *name,int of,mode_t om,const pmq_attr *attr) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = pmq_ctor(op,name)) >= 0) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		if ((rs = pmq_nameload(op,name)) >= 0) {
		    posixhelp	po(of,om,attr) ;
		    po.m = &posixhelp::open ;
		    if ((rs = po(op)) >= 0) {
			rc = rs ;
			op->magic = PMQ_MAGIC ;
		    }
		    if (rs < 0) {
			pmq_nameclean(op) ;
		    } /* end if (cleanup on error) */
	        } /* end if (pmq-nameload) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pmq_dtor(op) ;
	    }
	} /* end if (pmq_ctor) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pmq_open) */

int pmq_close(pmq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pmq_magic(op)) >= 0) {
	    {
		rs1 = pmq_nameclean(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        posixhelp	po ;
		po.m = &posixhelp::close ;
	        rs1 = po(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = pmq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_close) */

int pmq_send(pmq *op,cchar *sbuf,int slen,uint prio) noex {
	int		rs ;
	if ((rs = pmq_magic(op,sbuf)) >= 0) {
	    posixhelp	po(sbuf,slen,prio) ;
	    po.m = &posixhelp::send ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_send) */

int pmq_recv(pmq *op,char *rbuf,int rlen,uint *priop) noex {
	int		rs ;
	if ((rs = pmq_magic(op,rbuf)) >= 0) {
	    posixhelp	po(rbuf,rlen,priop) ;
	    po.m = &posixhelp::recv ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_send) */

int pmq_attrset(pmq *op,const pmq_attr *nattr,pmq_attr *oattr) noex {
	int		rs ;
	if ((rs = pmq_magic(op,nattr)) >= 0) {
	    posixhelp	po(oattr,nattr) ;
	    po.m = &posixhelp::attrset ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_attrset) */

int pmq_attrget(pmq *op,pmq_attr *oattr) noex {
	int		rs ;
	if ((rs = pmq_magic(op,oattr)) >= 0) {
	    posixhelp	po(oattr) ;
	    po.m = &posixhelp::attrget ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_attrget) */

int pmq_notify(pmq *op,SIGEVENT *sep) noex {
	int		rs ;
	if ((rs = pmq_magic(op)) >= 0) {
	    posixhelp	po(sep) ;
	    po.m = &posixhelp::notify ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_notify) */

int pmq_unlink(pmq *op) noex {
	int		rs ;
	if ((rs = pmq_magic(op)) >= 0) {
	    posixhelp	po ;
	    po.m = &posixhelp::unlink ;
	    rs = po(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pmq_unlink) */

/* OTHER API (but related) */
int uc_unlinkpmq(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        char	*altname{} ;
		if ((rs = libmalloc_mn(&altname)) >= 0) {
		    cint	maxnamelen = rs ;
	            if (name[0] != '/') {
	                rs = sncpy(altname,maxnamelen,"/",name) ;
	                name = altname ;
		    } else if (name[1] == '\0') {
			rs = SR_INVALID ;
	            }
		    if (rs >= 0) {
	                bool	f_exit = false ;
	                repeat {
	                    if ((rs = mq_unlink(name)) < 0) {
			        rs = (- errno) ;
			    }
	                    if (rs < 0) {
		                switch (rs) {
		                case SR_INTR:
	    	                    break ;
		                default:
		                    f_exit = true ;
		                    break ;
		                } /* end switch */
	                    } /* end if (error) */
	                } until ((rs >= 0) || f_exit) ;
		    } /* end if (ok) */
		    rs1 = uc_libfree(altname) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_unlinkpmq) */


/* local subroutines */

static int pmq_nameload(pmq *op,cchar *name) noex {
	int		nl = strlen(name) ;
	int		rs = SR_OK ;
	cchar		*prefix = "" ;
	if (name[0] != '/') {
	    prefix = "/" ;
	    nl += 1 ;
	} else if (nl == 1) {
	    rs = SR_INVALID ;
	} /* end if (needed rooted name) */
	if (rs >= 0) {
	    char	*np{} ;
	    if ((rs = uc_libmalloc((nl+1),&np)) >= 0) {
	        if ((rs = sncpy(np,nl,prefix,name)) >= 0) {
		    op->name = np ;
	        }
	        if (rs < 0) {
		    uc_libfree(np) ;
	        }
	    } /* end if (m-a) */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (pmq_nameload) */

static int pmq_nameclean(pmq *op) noex {
	int		rs = SR_OK ;
	if (op->name) {
	    rs = uc_libfree(op->name) ;
	    op->name = nullptr ;
	}
	return rs ;
}
/* end subroutine (pmq_nameclean) */

int posixhelp::operator () (pmq *op) noex {
	int		to_mfile = utimeout[uto_mfile] ;
	int		to_nfile = utimeout[uto_nfile] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = (this->*m)(op)) < 0) {
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
		case SR_NOMEM:
		    if (to_nomem -- > 0) {
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
	    } /* end if (ok) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end method (posixhelp::operator) */

int posixhelp::open(pmq *op) noex {
	int		rs = SR_OK ;
	mqd_t		res ;
	if ((res = mq_open(op->name,of,om,nattr)) == mqdbad) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::open) */

int posixhelp::close(pmq *op) noex {
	int		rs ;
	if ((rs = mq_close(op->pq)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::close) */

int posixhelp::send(pmq *op) noex {
	int		rs ;
	if ((rs = mq_send(op->pq,sbuf,slen,prio)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::send) */

int posixhelp::recv(pmq *op) noex {
	int		rs ;
	if ((rs = mq_receive(op->pq,rbuf,rlen,priop)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::recv) */

int posixhelp::attrset(pmq *op) noex {
	int		rs ;
	if ((rs = mq_setattr(op->pq,nattr,oattr)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::setattr) */

int posixhelp::attrget(pmq *op) noex {
	int		rs ;
	if ((rs = mq_getattr(op->pq,oattr)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::attrget) */

int posixhelp::notify(pmq *op) noex {
	int		rs ;
	if ((rs = mq_notify(op->pq,sep)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (posixhelp::notify) */

int posixhelp::unlink(pmq *op) noex {
	int		rs = SR_NOENT ;
	if (op->name[0] != '\0') {
	    if ((rs = mq_unlink(op->name)) < 0) {
		rs = (- errno) ;
	    } else {
	        op->name[0] = '0' ;
	    }
	} /* end if (non-empty) */
	return rs ;
}
/* end method (posixhelp::unlink) */

#if	CF_PMQDIR

static int pmqdiradd(cchar *name,mode_t om) noex {
	int		rs ;
	cchar		*pp = PMQ_PATHPREFIX ;
	char		tmpfname[MAXPATHLEN + 1] ;
	if ((rs = mkpath2(tmpfname,pp,name)) >= 0) {
	    if ((rs = u_creat(tmpfname,om)) == SR_NOENT) {
	        if ((rs = pmqdircheck(pp)) >= 0) {
	            rs = u_creat(tmpfname,om) ;
		}
	    } /* end if (u_creat) */
	    if (rs >= 0) u_close(rs) ;
	} /* end if (mkpath) */
	return rs ;
}
/* end subroutine (pmqdiradd) */

static int pmqdirrm(cchar *name) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pp = PMQ_PATHPREFIX ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkpath2(tbuf,pp,name)) >= 0) {
	        rs = u_unlink(tbuf) ;
	    }
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (pmqdirrm) */

static int pmqdircheck(cchar *pp) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(pp,&sb)) == SR_NOENT) {
	    const mode_t	dm = 0777 ;
	    const uid_t		euid = geteuid() ;
	    if ((rs = u_mkdir(pp,dm)) >= 0) {
	        if ((rs = u_chmod(pp,(dm|S_ISVTX))) >= 0) {
		    cint	n = PMQ_CHOWNVAR ;
	            if ((rs = u_pathconf(pp,n,nullptr)) == 0) {
	            	if ((rs = getpmquid()) >= 0) {
	    		    const uid_t		uid = rs ;
	                    if (euid != uid) { /* get a GID */
				if ((rs = getpmqgid()) >= 0) {
				    const gid_t	gid = rs ;
				    rs = u_chown(pp,uid,gid) ;
				} /* end if (getpmqgid) */
	                    } /* end if (UIDs different) */
			} /* end if (getpmquid) */
	            } /* end if (pathconf) */
	        } /* end if (CHMOD) */
	    } /* end if (mkdir) */
	} /* end if (directory did not exist) */
	return rs ;
}
/* end subroutine (pmqdircheck) */

#endif /* CF_PMQDIR */

#if	CF_GETPMQ

static int getpmquid(void) noex {
	int		rs ;
	int		rs1 ;
	int		uid = -1 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = rs ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        cchar	*un = PMQ_USERNAME1 ;
	        if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
	            uid = pw.pw_uid ;
	        } else if (rs == rsn) {
	    	    un = PMQ_USERNAME2 ;
	            if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
	    	        uid = pw.pw_uid ;
		    } else if (rs == rsn) {
		        rs = SR_OK ;
		        uid = PMQ_UID ;
		    }
	        }
	        rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? uid : rs ;
}
/* end subroutine (getpmquid) */

static int getpmqgid(void) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	cchar		*gn = PMQ_GROUPNAME1 ;
	if ((rs = getgid_group(gn,-1)) == rsn) {
	    gn = PMQ_GROUPNAME2 ;
	    if ((rs = getgid_group(gn,-1)) == rsn) {
		rs = PMQ_GID ;
	    }
	}
	return rs ;
}
/* end subroutine (getpmqgid) */

#endif /* CF_GETPMQ */



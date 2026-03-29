/* mailspool SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a mail-spool file w/ its associated lock */
/* version %I% last-modified %G% */

#define	CF_UCGETPW	0		/* use |uc_getpw{x}(3uc)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailspool

	Description:
	This subroutine is used to deliver new mail to the mail
	spool file for a given recipient.

	Synopsis:
	typedef mailspool	ms_t ;
	typedef mode_t		om_t ;
	int mailspool_open(ms_t *op,cc *md,cc *un,int of,om_t om,int to) noex

	Arguments:
	op		object pointer
	md		mail directory
	un		username
	of		open-flags
	om		open-mode
	to		time-out

	Returns:
	>=0		dile-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<uclock.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<bufsizevar.hh>
#include	<estrings.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mailspool.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	TO_SPOOL	(10*60)

#define	MS		mailspool


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef	mode_t		om_t ;


/* external subroutines */

extern "C" {
    extern int uc_unlink(cchar *) noex ;
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_fminmod(int,mode_t) noex ;
    extern int uc_close(int) noex ;
} /* end extern */


/* external variables */


/* local structures */

struct si {
	mailspool	*op ;
	cchar		*md ;
	cchar		*un ;
	cchar		*mfname ;
	gid_t		egid ;
	mode_t		om ;
	int		of ;
	int		to ;
	bool		f_create ;
} ; /* end struct (si) */


/* forward references */

template<typename ... Args>
local inline int mailspool_magic(mailspool *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MAILSPOOL_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (mailspool_magic) */

local int	mailspool_lfbegin(MS *,cchar *,cchar *) noex ;
local int	mailspool_lfend(MS *) noex ;

local int	si_start(si *,MS *,cchar *,cchar *,int,mode_t,int) noex ;
local int	si_finish(si *) noex ;
local int	si_checkcreate(si *) noex ;
local int	si_trying(si *) noex ;
local int	si_lockoutbegin(si *,time_t) noex ;
local int	si_lockoutend(si *,int) noex ;
local int	si_lockcreate(si *) noex ;
local int	si_lockin(si *) noex ;
local int	si_minmod(si *,int,mode_t) noex ;
local int	si_chown(si *) noex ;

local int	getlockcmd(int) noex ;


/* local variables */

static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mailspool_open(MS *op,cchar *md,cchar *un,int of,mode_t om,int to) noex {
	int		rs = SR_FAULT ;
	int		mfd = -1 ;
	if (op && md && un) ylikely {
	    of |= O_LARGEFILE ;
	    si	sub, *sip = &sub ;
	    if ((rs = si_start(sip,op,md,un,of,om,to)) >= 0) ylikely {
	        if ((rs = mailspool_lfbegin(op,md,un)) >= 0) ylikely {
	            if ((rs = si_checkcreate(sip)) >= 0) ylikely {
	                if ((rs = si_trying(sip)) >= 0) ylikely {
		            op->magic = MAILSPOOL_MAGIC ;
		        }
	            }
		    if (rs < 0) {
		        mailspool_lfend(op) ;
		    }
	        } /* end if (mailspool-lf) */
	        mfd = si_finish(sip) ;
	        if (rs >= 0) rs = mfd ;
	    } /* end if (si) */
	    if (rs < 0) {
	        mailspool_close(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? mfd : rs ;
}
/* end subroutine (mailspool_open) */

int mailspool_close(MS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailspool_magic(op)) >= 0) ylikely {
	    if (op->mfd >= 0) ylikely {
	        rs1 = u_close(op->mfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mfd = -1 ;
	    } 
	    if (op->lfname) ylikely {
	        if ((op->lfname[0] != '\0') && op->f_created) {
	            rs1 = uc_unlink(op->lfname) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        rs1 = mailspool_lfend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailspool_close) */

local int mailspool_lfbegin(MS *op,cchar *md,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = maxpathlen) >= 0) ylikely {
	    cint	maxpath = rs ;
	    cint	sz = ((rs + 1) * 2) ;
	    int		ai = 0 ;
	    if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) ylikely {
		cint	nlen = maxpath ;
		char	*nbuf = (a + ((maxpath + 1) * ai++)) ;
	        if ((rs = sncpy(nbuf,nlen,un,".lock")) >= 0) ylikely {
	    	    char	*lfn = (a + ((maxpath + 1) * ai++)) ;
	            if ((rs = mkpath(lfn,md,nbuf)) >= 0) ylikely {
		        if (cchar *cp ; (rs = libmem.strw(lfn,rs,&cp)) >= 0) {
		            op->lfname = cp ;
		        }
	            }
	        }
	        rs1 = libmem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return rs ;
}
/* end subroutine (mailspool_lfbegin) */

local int mailspool_lfend(MS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->lfname) ylikely {
	    void *vp = voidp(op->lfname) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->lfname = nullptr ;
	}
	return rs ;
}
/* end subroutine (mailspool_lfend) */

int mailspool_setlockinfo(MS *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		rs1 ;
	int		wl = 0 ;
	if ((rs = mailspool_magic(op,wbuf)) >= 0) ylikely {
	    cint	of = O_WRONLY ;
	    cmode	om = 0666 ;
	    if (wlen < 0) wlen = lenstr(wbuf) ;
	    if ((rs = u_open(op->lfname,of,om)) >= 0) ylikely {
	        cint	fd = rs ;
		{
	            rs = u_write(fd,wbuf,wlen) ;
		    wl = rs ;
		}
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (file) */
	} /* end if (magic) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (mailspool_setlockinfo) */


/* private subroutines */

local int si_start(si *sip,MS *op,cc *md,cc *un,int of,om_t om,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) ylikely {
	    memclear(sip) ;
	    sip->op = op ;
	    sip->md = md ;
	    sip->un = un ;
	    sip->of = of ;
	    sip->om = om ;
	    sip->to = to ;
	    sip->egid = getegid() ;
	    if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	        if ((rs = mkpath(tbuf,md,un)) >= 0) ylikely {
	            if (cchar *cp ; (rs = libmem.strw(tbuf,rs,&cp)) >= 0) {
		        sip->mfname = cp ;
		    } /* end if (memory-allocation) */
		}
		rs1 = lm_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (si_start) */

local int si_finish(si *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		mfd = -1 ;
	if (sip) ylikely {
	    mailspool	*op = sip->op ;
	    rs = SR_OK ;
	    if (sip->mfname) ylikely {
		void *vp = voidp(sip->mfname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->mfname = nullptr ;
	    }
	    mfd = op->mfd ;
	} /* end if (magic) */
	return (rs >= 0) ? mfd : rs ;
}
/* end subroutine (si_finish) */

local int si_checkcreate(si *sip) noex {
	int		rs ;
	cchar		*mfname = sip->mfname ;
	if (ustat sb ; (rs = u_stat(mfname,&sb)) >= 0) {
	    if (! S_ISREG(sb.st_mode)) {
		rs = SR_ISDIR ;
	    }
	} else if (isNotPresent(rs)) {
	    cint	of = sip->of ;
	    if (of&O_CREAT) {
		rs = SR_OK ;
		sip->f_create = true ;
	    }
	}
	return rs ;
}
/* end subroutine (si_checkcreate) */

local int si_trying(si *sip) noex {
	time_t		ti_start = getustime ;
	time_t		ti_now ;
	int		rs = SR_OK ;
	int		mfd = -1 ;
	int		rs1 ;
	ti_now = ti_start ;
	while (rs >= 0) {
	    if ((rs = si_lockoutbegin(sip,ti_now)) > 0) {
		rs = si_lockin(sip) ;
		mfd = rs ;
	        rs1 = si_lockoutend(sip,rs) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (si-lockout) */
	    if (mfd >= 0) break ;
	    if (rs == SR_AGAIN) rs = SR_OK ;
	    if (rs >= 0) sleep(1) ;
	    ti_now = time(nullptr) ;
	    if ((ti_now - ti_start) >= sip->to) {
		rs = SR_AGAIN ;
	    }
	} /* end while */
	return (rs >= 0) ? mfd : rs ;
}
/* end subroutine (si_trying) */

local int si_lockoutbegin(si *sip,time_t dt) noex {
	mailspool	*op = sip->op ;
	int		rs ;
	int		f = false ;
	cchar		*lfname ;
	lfname = op->lfname ;
	if (ustat sb ; (rs = u_stat(lfname,&sb)) >= 0) {
	    if ((dt-sb.st_mtime) >= TO_SPOOL) {
		if ((rs = u_unlink(lfname)) >= 0) {
		    rs = si_lockcreate(sip) ;
		    f = rs ;
		}
	    }
	} else if (isNotAccess(rs)) {
	    rs = si_lockcreate(sip) ;
	    f = rs ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (si_lockoutbegin) */

local int si_lockoutend(si *sip,int prs) noex {
	mailspool	*op = sip->op ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (prs < 0) {
	    char	*bp = cast_const<charp>(op->lfname) ;
	    rs1 = uc_unlink(op->lfname) ;
	    if (rs >= 0) rs = rs1 ;
	    bp[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (si_lockoutend) */

local int si_lockcreate(si *sip) noex {
	mailspool	*op = sip->op ;
	cint		of = (O_RDWR|O_CREAT|O_EXCL) ;
	cmode		om = 0664 ;
	int		rs ;
	if ((rs = u_open(op->lfname,of,om)) >= 0) {
	    op->f_created = true ;
	    u_close(rs) ;
	    rs = 1 ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (si_lockcreate) */

local int si_lockin(si *sip) noex {
	mailspool	*op = sip->op ;
	cmode		om = sip->om ;
	cint		of = sip->of ;
	int		rs ;
	int		mfd = -1 ;
	cchar		*mfname = sip->mfname ;
	if ((rs = u_open(mfname,of,om)) >= 0) ylikely {
	    mfd = rs ;
	    if ((rs = getlockcmd(of)) >= 0) ylikely {
		cint	cmd = rs ;
	        if ((rs = uc_lockfile(mfd,cmd,0L,0,1)) >= 0) ylikely {
		    if ((rs = si_minmod(sip,mfd,om)) >= 0) ylikely {
		        if ((rs = si_chown(sip)) >= 0) ylikely {
		            op->mfd = mfd ;
		        }
		    }
		} /* end if (lock) */
	    } /* end if (getlockcmd) */
	    if (rs < 0) {
		u_close(mfd) ;
		op->mfd = -1 ;
	    }
	} /* end if (file-open) */
	return (rs >= 0) ? mfd : rs ;
}
/* end subroutine (si_lockin) */

local int si_minmod(si *sip,int mfd,mode_t om) noex {
	int		rs = SR_OK ;
	if (sip->f_create) {
	    rs = uc_fminmod(mfd,om) ;
	}
	return rs ;
}
/* end subroutine (si_minmod) */

local int si_chown(si *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (sip->f_create) ylikely {
	    const gid_t		egid = sip->egid ;
	    cchar		*md = sip->md ;
	    if (ustat sb ; (rs = u_stat(md,&sb)) >= 0) ylikely {
		const gid_t	gid = sb.st_gid ;
	        if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	            cint	pwlen = rs ;
	            if (ucentpwx pw ; (rs = pw.nam(pwbuf,pwlen,sip->un)) >= 0) {
			const uid_t	euid = geteuid() ;
		        const uid_t	uid = pw.pw_uid ;
			if ((uid != euid) || (gid != egid)) {
			    cint	cmd = _PC_CHOWN_RESTRICTED ;
	                    if ((rs = u_pathconf(md,cmd,nullptr)) == 0) {
			        cchar	*mfname = sip->mfname ;
			        f = true ;
    		                rs = u_chown(mfname,uid,gid) ;
	                    } else if (rs == SR_NOSYS) {
	                        rs = SR_OK ;
	                    }
			} /* end if (needed) */
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
		    }
		    rs1 = lm_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (stat) */
	} /* end if (file was created) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (si_chown) */

local int getlockcmd(int of) noex {
	cint		am = (of & O_ACCMODE) ;
	int		cmd = SR_INVALID ;
	switch (am) {
	case O_WRONLY:
	case O_RDWR:
	    cmd = F_WLOCK ;
	    break ;
	case O_RDONLY:
	    cmd = F_RLOCK ;
	    break ;
	} /* end switch */
	return cmd ;
}
/* end subroutine (getlockcmd) */



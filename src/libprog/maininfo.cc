/* maininfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* main information */
/* version %I% last-modified %G% */

#define	CF_DEBUGN	0		/* special debugging */
#define	CF_SIGHAND	1		/* install csignalandlers */
#define	CF_SIGALTSTACK	0		/* do *not* define */

/* revision history:

	= 2001-11-01, David A­D­ Morano
	This subroutine was written for use as a front-end for Korn
	Shell (KSH) commands that are compiled as stand-alone
	programs.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	maininfo

	Descroption:
	This is the front-end to make the various SHELL (KSH)
	built-in commands into stand-alone programs.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<vecstr.h>
#include	<upt.h>
#include	<sncpyx.h>
#include	<sfx.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"maininfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MI	maininfo


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef int (*thrsub)(void *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	maininfo_utiler(MI *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int maininfo_start(MI *mip,int argc,mainv argv) noex {
    	cnullptr	np{} ;
	sigset_t	ss ;
	cint	sig = SIGTIMEOUT ;
	int		rs ;
	cchar	*argz = nullptr ;

	memclear(mip) ;
	if ((argc > 0) && (argv != nullptr)) argz = argv[0] ;

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
	if (argz == nullptr) argz = getexecname() ;
#endif

	uc_sigsetempty(&ss) ;
	uc_sigsetadd(&ss,sig) ;
	if ((rs = u_sigmask(SIG_BLOCK,&ss,&mip->savemask)) >= 0) {
	    if ((rs = vecstr_start(&mip->stores,2,0)) >= 0) {
	        cchar	*cp ;
	        if (int cl ; (cl = sfbasename(argz,-1,&cp)) > 0) {
	            cchar	**vpp = &mip->progname ;
	            if (cp[0] == '-') {
	                mip->fl.progdash = true ;
	                cp += 1 ;
	                cl -= 1 ;
	            }
	            if (cl > 0) {
	                if (cchar *tp = strnrchr(cp,cl,'.') ; tp) {
	                    cl = intconv(tp - cp) ;
	                }
	            }
	            if (cl > 0) {
	                rs = maininfo_setentry(mip,vpp,cp,cl) ;
	            } else {
	                rs = SR_DOM ;
	            }
	        } else {
	            rs = SR_DOM ;
	        }
	        if (rs < 0) {
	            vecstr_finish(&mip->stores) ;
		}
	    } /* end if (vecstr_start) */
	} /* end if (u_sigmask) */
	return rs ;
}
/* end subroutine (maininfo_start) */

int maininfo_finish(MI *mip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mip) {
	    rs = SR_OK ;
	    {
	        rs1 = vecstr_finish(&mip->stores) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = u_sigmask(SIG_SETMASK,&mip->savemask,nullptr) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (maininfo_finish) */

int maininfo_setentry(MI *mip,cchar **epp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (mip && epp) {
	    int		oi = -1 ;
	    rs = SR_OK ;
	    if (*epp) {
	        oi = vecstr_findaddr(&mip->stores,*epp) ;
	    }
	    if (vp) {
	        len = strnlen(vp,vl) ;
	        rs = vecstr_store(&mip->stores,vp,len,epp) ;
	    } else {
	        *epp = nullptr ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        rs1 = vecstr_del(&mip->stores,oi) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (maininfo_setentry) */

#if	CF_SIGALTSTACK
int maininfo_sigbegin(MI *mip,maininfohand_t sh,cint *sigcatches) noex {
    	cnullptr	np{} ;
	size_t		ms ;
	cint	ps = getpagesize() ;
	cint	ss = (2*SIGSTKSZ) ;
	cint	mp = (PROT_READ|PROT_WRITE) ;
	cint	mf = (MAP_PRIVATE|MAP_NORESERVE|MAP_ANON) ;
	int		rs ;
	int		fd = -1 ;
	void		*md ;
	ms = iceil(ss,ps) ;
	if ((rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    mip->mdata = md ;
	    mip->msize = ms ;
	    mip->astack.ss_size = ms ;
	    mip->astack.ss_sp = md ;
	    mip->astack.ss_flags = 0 ;
	    if ((rs = u_sigaltstack(&mip->astack,nullptr)) >= 0) {
	        rs = sighand_start(&mip->sh,np,np,sigcatches,sh) ;
	        if (rs < 0) {
	            mip->astack.ss_flags = SS_DISABLE ;
	            u_sigaltstack(&mip->astack,nullptr) ;
	        }
	    } /* end if (u_sigaltstack) */
	    if (rs < 0) {
	        u_munmap(mip->mdata,mip->msize) ;
	        mip->mdata = nullptr ;
	    }
	} /* end if (mmap) */
#if	CF_DEBUGN
	nprintf(NDF,"maininfo_sigbegin: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (maininfo_sigbegin) */
#else /* CF_SIGALTSTACK */
int maininfo_sigbegin(MI *mip,maininfohand_t sh,cint *sigcatches) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
#if	CF_SIGHAND
	rs = sighand_start(&mip->sh,np,np,sigcatches,sh) ;
#endif
#if	CF_DEBUGN
	nprintf(NDF,"maininfo_sigbegin: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (maininfo_sigbegin) */
#endif /* CF_SIGALTSTACK */

int maininfo_sigend(MI *mip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mip) {
	    rs = SR_OK ;
#if	CF_SIGHAND
	{
	    rs1 = sighand_finish(&mip->sh) ;
	    if (rs >= 0) rs = rs1 ;
	}
#endif
#if	CF_SIGALTSTACK
	{
	    mip->astack.ss_flags = SS_DISABLE ;
	    rs1 = u_sigaltstack(&mip->astack,nullptr) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (mip->mdata) {
	    rs1 = u_munmap(mip->mdata,mip->msize) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mdata = nullptr ;
	    mip->msize = 0 ;
	}
#endif /* CF_SIGALTSTACK */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (maininfo_sigend) */

int maininfo_utilbegin(MI *op,int f_run) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (f_run) {
	        thrsub	w = (thrsub) maininfo_utiler ;
	        pthread_t	tid ;
	        if ((rs = uptcreate(&tid,nullptr,w,op)) >= 0) {
	            op->tid = tid ;
	            op->fl.utilout = true ;
	        }
	    } /* end if (run) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (maininfo_utilbegin) */

int maininfo_utilend(MI *op) noex {
	int		rs = SR_OK ;
	if (op->fl.utilout) {
	    int		trs = SR_OK ;
	    op->fl.utilout = false ;
	    if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	        rs = trs ;
	    }
	}
	return rs ;
}
/* end subroutine (maininfo_utilend) */

int maininfo_srchname(MI *mip,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (mip && rpp) {
	    cchar	*srch = mip->progname ;
	    rs = SR_OK ;
	    *rpp = srch ;
	    if (hasuc(srch,-1)) {
	        cint	slen = MAXNAMELEN ;
	        char	sbuf[MAXNAMELEN+1] ;
	        if ((rs = sncpylc(sbuf,slen,srch)) >= 0) {
	            rs = maininfo_setentry(mip,rpp,sbuf,rs) ;
	        }
	    } else {
	        rs = lenstr(srch) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (maininfo_srchname) */


/* private subroutines */

local int maininfo_utiler(MI *mip) noex {
	cint		of = (O_WRONLY|O_APPEND) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cchar		*fn = "here.txt" ;
	if ((rs = u_open(fn,of,0664)) >= 0) {
	    cint	wlen = LINEBUFLEN ;
	    cint	fd = rs ;
	    cchar	*fmt = "hello world!\n" ;
	    char	wbuf[LINEBUFLEN+1] ;
	    if ((rs = bufprintf(wbuf,wlen,fmt)) >= 0) {
	        rs = u_write(fd,wbuf,rs) ;
		len = rs ;
	    }
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (open) */
	mip->f_done = true ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (maininfo_utiler) */



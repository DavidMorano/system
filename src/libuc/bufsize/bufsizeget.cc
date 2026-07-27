/* bufsizeget SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get various system buffer sizes */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_LOAD		1		/* load defaults from file */

/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David A-D- Morano
	I updated this to use the 'constexpr' capability of C++14
	(a very small improvement, hardly worth it).  The 'constexpr'
	capability in C++11 was garbage, in my opinion, and I could
	hardly wait until they finally fixed it (which they did in
	C++14).

*/

/* Copyright © 2001,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufsizeget

	Description:
	We return various system buffer sizes. Some of these are
	extracted from the operating system. The rest we just made
	up values by ourselves.

	Synopsis:
	int bufsizeget(int w) noex

	Arguments:
	w		which buffer-type to return information on

	Returns:
	>0		buffer size
	==0		no value was found
	<0		error (system-return)

	Notes:
	1. Like many of these system-service related facilities, they
	are all (mostly) implemented as dynamically initialized (on
	first use) emulated user-space kernel-calls (the normal and
	typical case).  They are thread-safe and everything else
	safe (except not Async-Signal safe). Unlike most emulated
	kernal-calls, this one does not need any finalization
	(destruction) before module (shared-memory-obejct) un-load
	or on program exit. Note that there are no locks. This is
	because multiple threads can be running though this object
	simultaneously without destructively interfering with each
	other. Some work-updates may be (harmlessly) repeated if
	multiple threads collide on data that they think needs
	updaing.  But this is otherwise all harmless.  This is not
	a typical situation for emulated kernel-calls. In case you
	were wondering, this is among the most simple emulated
	kernel-calls you will ever see!
	2. This object (below) does not need any "shutdown" (or
	finalization) mechanism since it does not need to release
	anything on any kind of "exit" whether it is a program exit
	or a shared-object unload operation.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<climits>		/* POSIX */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strstr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<matostr.h>		/* LIBUC */
#include	<cfdecmf.h>		/* LIBUC */
#include	<mailvalues.hh>		/* |hostnamemult| + |nodenamemult| */
#include	<bufsizes.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"bufsizeget.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import bufsizedata ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#ifndef	MAXLINELEN
#define	MAXLINELEN	(2 * 1024)
#endif


/* imported modules */

import bufsizedata ;			/* default values */
import ureserve ;			/* |sf{x}(3u)| */


/* imported namespaces */

using libu::strnchr ;			/* subroutine */
using libu::cfdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class ubufmgr {
	aflag		finit ;
	aflag		finitdone ;
	aflag		fbegin ;
	aflag		floaded ;
	int init	() noex ;
	int begin	() noex ;
	int retrieve	(int) noex ;
	int sysbs	(int,int) noex ;
	int def		(int) noex ;
	int zoneinfo	(int) noex ;
	int mailaddr	(int) noex ;
    public:
	int		bs[bufsize_overlast] ; /* Buffer-Size */
	int operator [] (int) noex ;
	int fileload	(cchar *) noex ;
	int fileread	(int,csize) noex ;
	int fileline	(cchar *,csize) noex ;
	int fileparse	(cchar *,int) noex ;
	int filelook	(cchar *,int,cchar *,int) noex ;
	ubufmgr() noex {
	    for (int i = 0 ; i < bufsize_overlast ; i += 1) {
		bs[i] = 0 ;
	    } /* end for */
	} ; /* end ctor */
    } ; /* end class (ubufmgr) */
} /* end namespace */


/* forward references */


/* local variables */

static ubufmgr		ubuf_vals ;
constexpr bufsizedata	bufdata ;
constexpr int		maxline		= MAXLINELEN ;
constexpr char		fileconf[]	= BUFSIZEGET_CONF ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_load		= CF_LOAD ;


/* exported subroutines */

int bufsizeget(int w) noex {
    	int		rs ;
    	DPRINTF("ent w=%d\n",w) ;
	{
	    rs = ubuf_vals[w] ;
	}
    	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (busizeget) */


/* local subroutines */

int ubufmgr::operator [] (int w) noex {
	int		rs = SR_INVALID ;
	DPRINTF("ent »bufsizeget« w=%d\n",w) ;
	if ((w >= 0) && (w < bufsize_overlast)) {
	    DPRINTF("name=%s\n",bufsizenames[w]) ;
	    if ((rs = bs[w]) == 0) {
		DPRINTF("-> init\n") ;
	        if ((rs = init()) >= 0) {
		    DPRINTF("init() rs=%d\n",rs) ;
	            rs = retrieve(w) ;
		    DPRINTF("retrieve() rs=%d\n",rs) ;
	        } /* end if (ubufmgr::init) */
		DPRINTF("init-out rs=%d\n",rs) ;
	    } /* end if (need initialization) */
	    DPRINTF("bs[%d]-out rs=%d\n",w,rs) ;
	} /* end if (valid) */
	DPRINTF("ret »bufszieget« rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (ubufmgr::operator) */

int ubufmgr::init() noex {
	cint		to = utimeout[uto_busy] ;
	int		rs = SR_OK ;
	int		f = false ;
	DPRINTF("ent »init«\n") ;
	if (! finit.testandset) {
	    DPRINTF("-> begin\n") ;
	    if ((rs = begin()) >= 0) {
	        DPRINTF("begin() rs=%d\n",rs) ;
	        finitdone = true ;
	        f = true ;
	    }
	    DPRINTF("begin-out rs=%d\n",rs) ;
	    if (rs < 0) finit = false ;
	} else if (! finitdone) {
	    timewatch	tw(to) ;
	    cauto lamb = [this] () -> int {
	        int	rsl = SR_OK ;
	        if (!finit) {
		    rsl = SR_LOCKFAIL ;		/* <- failure */
	        } else if (finitdone) {
		    rsl = 1 ;			/* <- OK ready */
	        }
	        return rsl ;
	    } ; /* end lambda (lamb) */
	    rs = tw(lamb) ;		/* <- time-watching occurs in there */
	} /* end if (time-watching) */
	DPRINTF("ret »init« rs=%d f=%d\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ubufmgr::init) */

int ubufmgr::begin() noex {
	int		rs = SR_OK ;
	if (! fbegin) {
	    fbegin = true ;
	    if_constexpr (f_load) {
	        rs = fileload(fileconf) ;
	    } /* end if_constexpr (f_load) */
	} /* end if (fileload) */
	return rs ;
} /* end subroutine (ubufmgr::begin) */

int ubufmgr::retrieve(int w) noex {
	int		rs ;
	DPRINTF("ent w=%d bs=%d\n",w,bs[w]) ;
	if ((rs = bs[w]) == 0) {
	    cint	name = bufdata[w].name ;
	    cint	defval = bufdata[w].defval ;
	    DPRINTF("need\n") ;
	    if (name >= 0) {
	        DPRINTF("name-yes\n") ;
	        if ((rs = sysbs(w,name)) == SR_NOTSUP) {
	            DPRINTF("sysbs-notsup\n") ;
		    rs = (defval) ? defval : BUFSIZEGET_DEFVAL ;
		    bs[w] = rs ;
		} else if (rs == 0) {
	            DPRINTF("sysbs-zero\n") ;
 		    rs  = BUFSIZEGET_DEFVAL ;
		} /* end if (ubufmgr::sysbs) */
	        DPRINTF("sysbs-out rs=%d\n",rs) ;
	    } else {
	        DPRINTF("name-not\n") ;
		rs = def(w) ;
		bs[w] = rs ;
	    } /* end if */
	} /* end if (getting default value) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (ubufmgr::retrieve) */

int ubufmgr::sysbs(int w,int name) noex {
	int		rs = bs[w] ;
	DPRINTF("ent w=%d name=%d\n",w,name) ;
	if (bs[w] == 0) {
	    DPRINTF("need\n") ;
	    if ((rs = uc_sysconfval(name,nullptr)) >= 0) {
	        DPRINTF("uc_sysconf() rs=%d\n",rs) ;
	        bs[w] = rs ;
	    } else if ((rs == SR_NOENT) || (rs == SR_NOSYS)) {
	        DPRINTF("uc_sysconf() rs=%d\n",rs) ;
	        DPRINTF("uc_sysconf() NOENT || NOSYS\n") ;
		rs = 0 ; /* specifies no-entry available */
	    } /* end if */
	    DPRINTF("uc_sysconf-out rs=%d\n",rs) ;
	} /* end if */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (ubufmgr::sysbs) */

int ubufmgr::def(int w) noex {
	cint		defval = bufdata[w].defval ;
	int		rs = SR_OK ;
	if (defval >= 0) {
    	    bs[w] = (defval) ? defval : BUFSIZEGET_DEFVAL ;
	} else {
	    switch (w) {
	    case bufsize_zi:
		rs = zoneinfo(w) ;
		break ;
	    case bufsize_mailaddr:
		rs = mailaddr(w) ;
	        break ;
	    default:
		rs = SR_NOTSUP ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
} /* end method (ubufmgr::def) */

/* yes; I call myself recursively - repeatedly (deal with it) */
int ubufmgr::zoneinfo(int w) noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	    bs[w] = rs ;
	}
	return rs ;
} /* end method (ubufmgr::zoneinfo) */

/* yes; I call myself recursively - repeatedly (deal with it) */
int ubufmgr::mailaddr(int w) noex {
    	cint		hostmult = mailvalue.hostnamemult ;
    	cint		nodemult = mailvalue.nodenamemult ;
    	int		rs ;
	if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
	    cint	hnl = rs ;
	    if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
		cint	nnl = rs ;
		rs = ((hostmult * hnl) + (nodemult * nnl)) ;
		bs[w] = rs ;
	    }
	} /* end if (bufsizeget) */
	return rs ;
} /* end method (ubufmgr::mailaddr) */

int ubufmgr::fileload(cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		cint	of = O_RDONLY ;
		if ((rs = u_open(fn,of,0)) >= 0) ylikely {
		    cint	fd = rs ;
		    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
			csize	fsize = size_t(sb.st_size) ;
		        if (S_ISREG(sb.st_mode)) ylikely {
			    if (fsize > 0) {
			        rs = fileread(fd,fsize) ;
			        c += rs ;
			    } /* end if (non-zero positive) */
			} /* end if (regular file) */
		    } /* end if (stat) */
		    rs1 = u_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (ubufmgr::fileload) */

int ubufmgr::fileread(int fd,csize ms) noex {
	cnullptr	np{} ;
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (void *md ; (rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) ylikely {
	    cint	cmd = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(md,ms,cmd)) >= 0) ylikely {
		size_t	ll = ms ;
		cchar	*lp = charp(md) ;
		for (cchar *tp ; (tp = charp(memchr(lp,'\n',ll))) != np ; ) {
		    csize lsize = size_t((tp + 1) - lp) ;
		    {
		        rs = fileline(lp,lsize) ;
		        c += rs ;
		    }
		    ll -= lsize ;
		    lp += lsize ;
		    if (rs < 0) break ;
		} /* end for */
	    } /* end if (memory-advise) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (ubufmgr::fileread) */

int ubufmgr::fileline(cchar *lp,csize lsize) noex {
    	int		rs = SR_OK ;
	int		c = 0 ;
	if (lsize <= maxline) {
	    cint lsz = intconv(lsize) ;
	    cchar *cp ;
	    if (int cl ; (cl = sfcontent(lp,lsz,&cp)) > 0) {
		rs = fileparse(cp,cl) ;
		c += rs ;
	    } /* end if (sfcontent) */
	} /* end if (sanity-check) */
	return (rs >= 0) ? c : rs ;
} /* end method (ubufmgr::fileline) */

int ubufmgr::fileparse(cchar *sp,int sl) noex {
    	int		rs = SR_OK ;
	int		kl = -1 ;
	int		vl = 0 ;
	int		c = 0 ; /* return-value */
	cchar		*kp = sp ;
	cchar		*vp = nullptr ;
	DPRINTF("ent\n") ;
	if (cchar *tp = strnchr(sp,sl,'=') ; tp) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
		if ((kl = sfshrink(sp,tl,&kp)) > 0) {
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		    if ((vl = sfshrink(sp,sl,&vp)) > 0) {
			rs = filelook(kp,kl,vp,vl) ;
			c += rs ;
		    } /* end if (non-zero positive) */
		} /* end if (non-zero positive) */
	    } /* end if (non-zero positive) */
	} else {
	    if ((kl = sfnext(sp,sl,&kp)) > 0) {
		sl -= intconv((kp + kl) - sp) ;
		sp = (kp + kl) ;
		if ((vl = sfnext(sp,sl,&vp)) > 0) {
		    rs = filelook(kp,kl,vp,vl) ;
		    c += rs ;
		} /* end if (sfnext) */
	    } /* end if (sfnext) */
	} /* end if */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (ubufmgr::fileline) */

int ubufmgr::filelook(cchar *kp,int kl,cchar *vp,int vl) noex {
    	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	cpcchar		*names = bufsizenames ;	/* <- from |bufsizes| */
	DPRINTF("ent\n") ;
	if_constexpr (f_debug) {
	    strnul ks(kp,kl) ;
	    strnul vs(vp,vl) ;
	    DPRINTF("key=%s\n",ccp(ks)) ;
	    DPRINTF("val=%s\n",ccp(vs)) ;
	} /* end if_constexpr (f_debug) */
	if (int w ; (w = matocasestr(names,2,kp,kl)) >= 0) {
	    if (int v ; (rs = cfdecmf(vp,vl,&v)) >= 0) {
		DPRINTF("load v=%d\n",v) ;
		bs[w] = v ;
	    } else if (isNotValid(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (matocasestr) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (ubufmgr::filelook) */



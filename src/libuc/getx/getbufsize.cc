/* getbufsize SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get various system buffer sizes */
/* version %I% last-modified %G% */


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
	getbufsize

	Description:
	We return various system buffer sizes. Some of these are
	extracted from the operating system. The rest we just made
	up values by ourselves.

	Synopsis:
	int getbufsize(int w) noex

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
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strstr(3c)| */
#include	<usystem.h>
#include	<timewatch.hh>
#include	<vecstr.h>
#include	<matostr.h>
#include	<cfdecmf.h>
#include	<isnot.h>
#include	<mailvalues.hh>		/* |hostnamemult| + |nodenamemult| */
#include	<localmisc.h>

#include	"getbufsize.h"


/* local defines */


/* imported modules */

import bufsizedata ;


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class ubufsize {
	aflag		finit ;
	aflag		finitdone ;
	aflag		fbegin ;
	aflag		floaded ;
	int init() noex ;
	int begin() noex ;
	int load() noex ;
	int loadent(cchar *) noex ;
	int retrieve(int) noex ;
	int sysbs(int,int) noex ;
	int def(int) noex ;
	int zoneinfo(int) noex ;
	int mailaddr(int) noex ;
    public:
	int		bs[getbufsize_overlast] ; /* Buffer-Size */
	int operator [] (int) noex ;
    } ; /* end class (ubufsize) */
} /* end namespace */


/* forward references */


/* local variables */

static ubufsize			ubufsize_data ;

static constexpr bufsizedata	bufdata ;


/* exported subroutines */

int getbufsize(int w) noex {
	return ubufsize_data[w] ;
}
/* end subroutine (getbusize) */


/* local subroutines */

int ubufsize::operator [] (int w) noex {
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < getbufsize_overlast)) {
	    if ((rs = bs[w]) == 0) {
	        if ((rs = init()) >= 0) {
	            rs = retrieve(w) ;
	        } /* end if (ubufsize::init) */
	    } /* end if (need initialization) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (ubufsize::operator) */

int ubufsize::init() noex {
	cint		to = utimeout[uto_busy] ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! finit.testandset) {
	    if ((rs = begin()) >= 0) {
	        finitdone = true ;
	        f = true ;
	    }
	    if (rs < 0) finit = false ;
	} else if (! finitdone) {
	    timewatch	tw(to) ;
	    auto lamb = [this] () -> int {
	        int	rsl = SR_OK ;
	        if (!finit) {
		    rsl = SR_LOCKLOST ;		/* <- failure */
	        } else if (finitdone) {
		    rsl = 1 ;			/* <- OK ready */
	        }
	        return rsl ;
	    } ; /* end lambda (lamb) */
	    rs = tw(lamb) ;		/* <- time-watching occurs in there */
	} /* end if (time-watching) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ubufsize::init) */

int ubufsize::begin() noex {
	int		rs = SR_OK ;
	if (! fbegin) {
	    fbegin = true ;
	    rs = load() ;
	}
	return rs ;
}
/* end subroutine (ubufsize::begin) */

int ubufsize::load() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (! floaded) {
	    floaded = true ;
	    if (vecstr cfv ; (rs = cfv.start(1,0)) >= 0) {
	        cchar	*fn = GETBUFSIZE_CONF ;
	        if ((rs = cfv.envfile(fn)) >= 0) {
	            cchar	*kp ;
	            for (int i = 0 ; cfv.get(i,&kp) >= 0 ; i += 1) {
	                if (kp) {
			    rs = loadent(kp) ;
	                } /* end if (non-null) */
	                if (rs < 0) break ;
	            } /* end for */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	        rs1 = cfv.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	} /* end if (need load) */
	return rs ;
}
/* end subroutine (getbufsize_load) */

int ubufsize::loadent(cchar *kp) noex {
    	int		rs = SR_OK ;
	int		kl = -1 ;
	int		vl = 0 ;
	cchar		*vp = nullptr ;
	cchar		**vars = bufsizenames ;
	if (cchar *tp ; (tp = strchr(kp,'=')) != nullptr) {
	    kl = intconv(tp - kp) ;
	    vp = (tp+1) ;
	    vl = -1 ;
	} /* end if */
	if (int w ; (w = matocasestr(vars,4,kp,kl)) >= 0) {
	    if (int v ; (rs = cfdecmfi(vp,vl,&v)) >= 0) {
		bs[w] = v ;
	    } else if (isNotValid(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (matocasestr) */
	return rs ;
} 
/* end method (ubufsize::loadent) */

int ubufsize::retrieve(int w) noex {
	int		rs ;
	if ((rs = bs[w]) == 0) {
	    cint	name = bufdata[w].name ;
	    cint	defval = bufdata[w].defval ;
	    if (name >= 0) {
	        if ((rs = sysbs(w,name)) == SR_NOTSUP) {
		    rs = (defval) ? defval : GETBUFSIZE_DEFVAL ;
		    bs[w] = rs ;
		} else if (rs == 0) {
 		   rs  = GETBUFSIZE_DEFVAL ;
		} /* end if (ubufsize::sysbs) */
	    } else {
		rs = def(w) ;
		bs[w] = rs ;
	    } /* end if */
	} /* end if (getting default value) */
	return rs ;
}
/* end subroutine (ubufsize::retrieve) */

int ubufsize::sysbs(int w,int name) noex {
	int		rs = bs[w] ;
	if (bs[w] == 0) {
	    if ((rs = uc_sysconfval(name,nullptr)) >= 0) {
	        bs[w] = rs ;
	    } else if (rs == SR_NOENT) {
		rs = 0 ; /* specifies no-entry available */
	    }
	} /* end if */
	return rs ;
}
/* end method (ubufsize::sysbs) */

int ubufsize::def(int w) noex {
	cint		defval = bufdata[w].defval ;
	int		rs = SR_OK ;
	if (defval >= 0) {
    	    bs[w] = (defval) ? defval : GETBUFSIZE_DEFVAL ;
	} else {
	    switch (w) {
	    case getbufsize_zi:
		rs = zoneinfo(w) ;
		break ;
	    case getbufsize_mailaddr:
		rs = mailaddr(w) ;
	        break ;
	    default:
		rs = SR_NOTSUP ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
}
/* end method (ubufsize::def) */

/* yes; I call myself recursively - repeatedly (deal with it) */
int ubufsize::zoneinfo(int w) noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    bs[w] = rs ;
	}
	return rs ;
}
/* end method (ubufsize::zoneinfo) */

/* yes; I call myself recursively - repeatedly (deal with it) */
int ubufsize::mailaddr(int w) noex {
    	cint		hostmult = mailvalue.hostnamemult ;
    	cint		nodemult = mailvalue.nodenamemult ;
    	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    cint	hnl = rs ;
	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		cint	nnl = rs ;
		rs = ((hostmult * hnl) + (nodemult * nnl)) ;
		bs[w] = rs ;
	    }
	}
	return rs ;
}
/* end method (ubufsize::mailaddr) */



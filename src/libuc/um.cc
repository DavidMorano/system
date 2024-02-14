/* um SUPPORT (UNIX® Memory-Management) */
/* lang=C++23 */

/* UNIX® Memory-Management interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the subroutine declarations for various
	UNIX® system Memory-Management subroutines.

	Names:
	u_mmapbegin
	u_mmapend
	u_mlockbegin
	u_mlockend
	u_mlockallbegin
	u_mlockallend
	u_mcntl
	u_mincore
	u_mprotect
	u_madvise
	u_msync
	u_mlockp

	Aliases:
	u_mapfile		-> u_mmapbegin
	u_mmap			-> u_mmapbegin
	u_munmap		-> u_mmapend

	More Solaris® bugs!

	Stupid Solaris tries to prevent developers from mapping
	files while simultaneously using record locks on that file.
	There are even some reports that (stupid) Solaris also
	prevents users from mapping files and allowing that same
	file from being mounted remotely. However, the problem is
	that (stupid) Solaris is not consistent in its attempts to
	prevent uses from mapping these sorts of files. If someone
	maps a file that has record locks on it but maps it at
	exactly the size of that file, Solaris prevents it (or
	allows it, I forget). But if someone maps a file beyond the
	end of it, then (stupid) Solaris denies (or allows) it.

	Developer beware!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<cerrno>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"um.h"


/* local defines */


/* local namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct um ;
    typedef int (um::*mem_f)(void *,size_t) noex ;
    struct um {
	mem_f		m ;
	char		*vec ;
	void		*vp ;
	void		*arg ;
	off_t		off ;
	int		pr ;
	int		fl ;
	int		fd ;
	int		attr ;
	int		mask ;
	um() noex { } ;
	um(int p,int f,int d,off_t o,void *v) noex {
	    pr = p ;
	    fl = f ;
	    fd = d ;
	    off = o ;
	    vp = v ;
	} ;
	um(int f) noex : fl(f) { } ;
	um(char *v) noex : vec(v) { } ;
	um(int c,void *a,int t,int m) noex {
	    fl = c ;
	    arg = a ;
	    attr = t ;
	    mask = m ;
	} ;
	int operator () (void *,size_t) noex ;
	int mapbegin(void *,size_t) noex ;
	int mapend(void *,size_t) noex ;
	int lockbegin(void *,size_t) noex ;
	int lockend(void *,size_t) noex ;
	int lockallbegin(void *,size_t) noex ;
	int lockallend(void *,size_t) noex ;
	int cntl(void *,size_t) noex ;
	int incore(void *,size_t) noex ;
	int protect(void *,size_t) noex ;
	int advise(void *,size_t) noex ;
	int sync(void *,size_t) noex ;
	int lockp(void *,size_t) noex ;
    } ; /* end struct (um) */
}


/* forward references */


/* local variables */

static cvoid	*mapfailed = reinterpret_cast<voidp>(MAP_FAILED) ;


/* exported variables */


/* exported subroutines */

int u_mmapbegin(void *ma,size_t ms,int pr,int fl,int fd,
		off_t off,void *vp) noex {
	um		umo(pr,fl,fd,off,vp) ;
	umo.m = &um::mapbegin ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mmapbegin) */

int u_mmapend(void *ma,size_t ms) noex {
	um		umo ;
	umo.m = &um::mapend ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mmapend) */

int u_mlockbegin(void *ma,size_t ms) noex {
	um		umo ;
	umo.m = &um::lockbegin ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mlockbegin) */

int u_mlockend(void *ma,size_t ms) noex {
	um		umo ;
	umo.m = &um::lockend ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mlockend) */

int u_mlockallbegin(int flags) noex {
	const nullptr_t	np{} ;
	um		umo(flags) ;
	umo.m = &um::lockallbegin ;
	return umo(np,0z) ;
}
/* end subroutine (u_mlockallbegin) */

int u_mlockallend() noex {
	const nullptr_t	np{} ;
	um		umo ;
	umo.m = &um::lockallend ;
	return umo(np,0z) ;
}
/* end subroutine (u_mlockallend) */

int u_mcntl(void *ma,size_t ms,int cmd,void *arg,int attr,int mask) noex {
	um		umo(cmd,arg,attr,mask) ;
	umo.m = &um::cntl ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mcntl) */

int u_mincore(void *ma,size_t ms,char *vec) noex {
	um		umo(vec) ;
	umo.m = &um::incore ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mincore) */

int u_mprotect(void *ma,size_t ms,int cmd) noex {
	um		umo(cmd) ;
	umo.m = &um::protect ;
	return umo(ma,ms) ;
}
/* end subroutine (u_mprotect) */

int u_madvise(void *ma,size_t ms,int cmd) noex {
	um		umo(cmd) ;
	umo.m = &um::advise ;
	return umo(ma,ms) ;
}
/* end subroutine (u_madvise) */

int u_msync(void *ma,size_t ms,int cmd) noex {
	um		umo(cmd) ;
	umo.m = &um::sync ;
	return umo(ma,ms) ;
}
/* end subroutine (u_msync) */

int u_mlockp(int cmd) noex {
	const nullptr_t	np{} ;
	um		umo(cmd) ;
	umo.m = &um::lockp ;
	return umo(np,0z) ;
}
/* end subroutine (u_mlockp) */


/* legacy */

int u_mapfile(void *ma,size_t ms,int pr,int fl,int fd,
			off_t off,void *vp) noex {
	return u_mmapbegin(ma,ms,pr,fl,fd,off,vp) ;
}
/* end subroutine (u_mapfile) */

int u_mmap(void *ma,size_t ms,int pr,int fl,int fd,
			off_t off,void *vp) noex {
	return u_mmapbegin(ma,ms,pr,fl,fd,off,vp) ;
}
/* end subroutine (u_mmap) */

int u_munmap(void *ma,size_t ms) noex {
	return u_mmapend(ma,ms) ;
}


/* local subroutines */

int um::operator () (void *ma,size_t ms) noex {
	int		to_nosr = utimeout[uto_nosr] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nolck = utimeout[uto_nolck] ;
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    errno = 0 ;
	    if ((rs = (this->*m)(ma,ms)) < 0) {
	        switch (rs) {
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
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
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (um::operator) */

int um::mapbegin(void *ma,size_t ms) noex {
	void		**rpp = (void **) vp ;
	int		rs = SR_FAULT ;
	if (rpp) {
	    void	*ra{} ;
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        rs = SR_OK ;
	        ra = mmap(ma,ms,pr,fl,fd,off) ;
	        if (ra == mapfailed) rs = (- errno) ;
	    }
	    *rpp = (rs >= 0) ? ra : nullptr ;
	}
	return rs ;
}
/* end method (um::mapbegin) */

int um::mapend(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = munmap(ma,ms)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::mapend) */

int um::lockbegin(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = mlock(ma,ms)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::lockbegin) */

int um::lockend(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = munlock(ma,ms)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::lockend) */

int um::lockallbegin(void *,size_t) noex {
	int		rs ;
	if ((rs = mlockall(fl)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (um::lockallbegin) */

int um::lockallend(void *,size_t) noex {
	int		rs ;
	if ((rs = munlockall()) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (um::lockallend) */

int um::cntl(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = memcntl(ma,ms,fl,arg,attr,mask)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::cntl) */

int um::incore(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma && vec) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = mincore(ma,ms,vec)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::incore) */

int um::protect(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = mprotect(ma,ms,fl)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::protect) */

int um::advise(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = madvise(ma,ms,fl)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::advise) */

int um::sync(void *ma,size_t ms) noex {
	int		rs = SR_FAULT ;
	if (ma) {
	    rs = SR_INVALID ;
	    if (ms > 0) {
	        if ((rs = msync(ma,ms,fl)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (um::sync) */

int um::lockp(void *,size_t) noex {
	int		rs ;
	if ((rs = plock(fl)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (um::lockp) */



/* ufiler SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

	= 2025-06-04, David A­D­ Morano
	I enhanced one of the subroutine in here.  I enhanced the
	|u_resolvepath(3u)| subroutine to allow the caller-supplied
	result buffer length to be unspecified (that is to be
	negative).  The value of the maximum-path-length will be
	used instead.  This can be dnagerous for the careless caller,
	but I think I have a use for this in implementing the
	|uc_realpath(3uc)| subroutine in the LIBUC library.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_access
	u_chdir
	u_chmod
	u_minmod
	u_chown
	u_lchown
	u_link
	u_lstat
	u_mkdir
	u_mknod
	u_pathconf
	u_readlink
	u_rename
	u_resolvepath
	u_rmdir
	u_stat
	u_statfs
	u_statvfs
	u_symlink
	u_truncate
	u_unlink
	u_xattrget
	u_xattrset


	Name:
	u_rename

	Description:
	This subroutine renames a file, link, or a diretory.

	Note (u_rename):
	Contrary to a good bit of confusion (at least at the present
	time) this subroutine is indeed a system call.  Why it was
	advertized as a STDIO call at one point was pure craziness.
	Maybe it is still advertised as a STDIO call with your
	implementation but it should not be.  Check its object file
	for any crazy STDIO functions of variables.

	The confusion over this subroutine is just part of some of
	the wacked out craziness from the earlier UNIX® days.  Yes,
	UNIX® went on to become big (I watched it myself through
	the decades from nothing to take-over-the-world status) but
	the guys up in the old Computer-Science Area-11 research
	division could have taken some time to have cleaned some
	things up before they got too entrenched (like the confusion
	over this subroutine call).

	Name:
	u_resolvepath

	Description:
	This subroutine takes an existing path and creates a new
	path that does not contain any symbolic or relative components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>		/* standard says this is necessary */
#include	<sys/stat.h>		/* |S_IFMT| */
#include	<sys/xattr.h>		/* is this now ubiquitous? */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uint32_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ufileop.h"

import ulibvals ;

/* local defines */


/* imported namespaces */

using namespace	ufileop ;		/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	rename(cchar *,cchar *) noex ;
}


/* external variables */


/* local structure */

namespace {
    struct ufiler ;
    typedef int (ufiler::*ufiler_m)(cchar *) noex ;
    struct ufiler : ufileopbase {
	ufiler_m	m = nullptr ;
	cchar		*dfn ;
	cchar		*xaname ;
	cvoid		*xaval ;
	char		*rbuf ;
	dev_t		dev ;
	size_t		sz ;
	uint32_t	foff ;
	int		oo ;
	int		rlen ;
	mode_t		fm ;
	ufiler() noex { } ;
	ufiler(mode_t om,dev_t d = 0) noex : fm(om), dev(d) { } ;
	ufiler(cchar *d) noex : dfn(d) { } ;
	ufiler(cc *n,cvoid *v,size_t s,uint32_t fo,int o) noex {
	    xaname = n ;
	    xaval = v ;
	    sz = s ;
	    foff = fo ;
	    oo = o ;
	} ;
	ufiler(char *rb,int rl) noex : rbuf(rb), rlen(rl) { } ;
	int callstd(cchar *fn) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fn) ;
	    }
	    return rs ;
	} ;
	void submem(ufiler_m mem) noex {
	    m = mem ;
	} ;
	int imkdir(cchar *) noex ;
	int imknod(cchar *) noex ;
	int irename(cchar *) noex ;
	int iresolvepath(cchar *) noex ;
	int irmdir(cchar *) noex ;
	int isymlink(cchar *) noex ;
	int ixattrget(cchar *) noex ;
	int ixattrset(cchar *) noex ;
    } ; /* end struct (ufiler) */
} /* end namespace */


/* forward references */

static int getrlen(int rlen) noex {
	int		rs ;
	if ((rs = rlen) < 0) {
	    rs = ulibval.maxpathlen ;
	}
	return rs ;
} /* end subroutine (getrlen) */

static sysret_t	std_resolvepath(cchar *,char *,int) noex ;


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;


/* exported variables */


/* exported subroutines */

int u_access(cchar *fname,int am) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = access(fname,am)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_access) */

int u_chdir(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = chdir(fname)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_chdir) */

int u_chmod(cchar *fname,mode_t m) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        m &= (~ S_IFMT) ;
	        repeat {
	            if ((rs = chmod(fname,m)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_chmod) */

int u_minmod(cchar *name,mode_t m) noex {
	int		rs = SR_FAULT ;
	int		fchanged = false ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	            cmode	om = (sb.st_mode & (~ S_IFMT)) ;
	            cmode	nm = (m & (~ S_IFMT)) ;
	            if ((om & nm) != nm) {
			fchanged = true ;
		        rs = u_chmod(name,(om | nm)) ;
	            }
	        } /* end if (u_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fchanged : rs ;
}
/* end subroutine (u_minmod) */

int u_chown(cchar *fname,uid_t uid,gid_t gid) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = chown(fname,uid,gid)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_chmod) */

int u_lchown(cchar *fname,uid_t uid,gid_t gid) noex {
	int		rs ;
	repeat {
	    if ((rs = lchown(fname,uid,gid)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_lchown) */

int u_link(cchar *fname,cchar *dst) noex {
	int		rs = SR_FAULT ;
	if (fname && dst) {
	    rs = SR_INVALID ;
	    if (fname[0] && dst[0]) {
	        repeat {
	            if ((rs = link(fname,dst)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_link) */

int u_lstat(cchar *fname,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname && sbp) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = lstat(fname,sbp)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_lstat) */

int u_mkdir(cchar *fn,mode_t fm) noex {
	ufiler		fo(fm) ;
	fo.m = &ufiler::imkdir ;
	return fo(fn) ;
}
/* end subroutine (u_mkdir) */

int u_mknod(cchar *fn,mode_t fm,dev_t dev) noex {
	ufiler		fo(fm,dev) ;
	fo.m = &ufiler::imknod ;
	return fo(fn) ;
}
/* end subroutine (u_mknod) */

int u_pathconf(cchar *fn,int name,long *rp) noex {
	int		rs = SR_FAULT ;
        long            lw = 0 ;
	if (fn) {
            rs = SR_INVALID ;
            if (fn[0] && (name >= 0)) {
                rs = SR_OK ;
                errno = 0 ;
                if ((lw = pathconf(fn,name)) < 0) {
                    if_constexpr (f_sunos) {
                        if ((name == _PC_CHOWN_RESTRICTED) && (lw == -1L)) {
                            lw = 0 ;
                        }
                    } /* end if_constexpr (f_sunos) */
                    if (errno != 0) rs = (- errno) ;
                } /* end if (pathconf) */
                if ((rs >= 0) && (lw >= 0)) {
                    rs = intsat(lw) ;
                } /* end if (ok) */
            } /* end if (valid) */
	} /* end if (non-null) */
        if (rp) {
            *rp = (rs >= 0) ? lw : 0l ;
        }
        return rs ;
}
/* end subroutine (u_pathconf) */

int u_readlink(cchar *fname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = getrlen(rlen)) >= 0) {
		    cauto rl = readlink ;
	            repeat {
	                if (ssize_t rsize ; (rsize = rl(fname,rbuf,rlen)) < 0) {
		            rs = (- errno) ;
	                } else {
		            rs = intsat(rsize) ;
	                }
	            } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	            if (rs >= 0) {
	                rbuf[rs] = '\0' ;
	            } else {
	                rbuf[0] = '\0' ;
	            }
	        } /* end if (getrlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_readlink) */

int u_rename(cchar *fn,cchar *dfn) noex {
	ufiler		fo(dfn) ;
	fo.m = &ufiler::irename ;
	return fo(fn) ;
}
/* end subroutine (u_rename) */

int u_resolvepath(cchar *fname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = getrlen(rlen)) >= 0) {
	            ufiler	fo(rbuf,rs) ;
	            fo.m = &ufiler::iresolvepath ;
	            rs = fo(fname) ;
	        } /* end if (getrlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_resolvepath) */

int u_rmdir(cchar *fn) noex {
	ufiler		fo ;
	fo.m = &ufiler::irmdir ;
	return fo(fn) ;
}
/* end subroutine (u_rmdir) */

int u_stat(cchar *fname,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = statfile(fname,sbp)) < 0) {
		        rs = (- errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_stat) */

int u_statfs(cchar *fname,ustatfs *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = statfilefs(fname,sbp)) < 0) {
		        rs = (- errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_statfs) */

int u_statvfs(cchar *fname,ustatvfs *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = statfilevfs(fname,sbp)) < 0) {
		        rs = (- errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_statvfs) */

int u_symlink(cchar *fn,cchar *dfn) noex {
	ufiler		fo(dfn) ;
	fo.m = &ufiler::isymlink ;
	return fo(fn) ;
}
/* end subroutine (u_symlink) */

int u_truncate(cchar *fname,off_t fo) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = truncate(fname,fo)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_truncate) */

int u_unlink(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        repeat {
	            if ((rs = unlink(fname)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_unlink) */

int u_xattrget(cc *fn,cc *n,void *v,size_t sz,uint32_t foff,int o) noex {
	ufiler		fo(n,v,sz,foff,o) ;
	fo.m = &ufiler::ixattrget ;
	return fo(fn) ;
}

int u_xattrset(cc *fn,cc *n,cvoid *v,size_t sz,uint32_t foff,int o) noex {
	ufiler		fo(n,v,sz,foff,o) ;
	fo.m = &ufiler::ixattrset ;
	return fo(fn) ;
}


/* local subroutines */

int ufiler::imkdir(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = mkdir(fname,fm)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ufiler::imkdir) */

int ufiler::imknod(cchar *fn) noex {
	int		rs = SR_FAULT ;
	if ((rs = mknod(fn,fm,dev)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiler::umknod) */

int ufiler::irename(cchar *fn) noex {
	int		rs ;
	if ((rs = rename(fn,dfn)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiler::irename) */

int ufiler::iresolvepath(cchar *fn) noex {
    	return std_resolvepath(fn,rbuf,rlen) ;
}
/* end method (ufiler::iresolvepath) */

int ufiler::irmdir(cchar *fn) noex {
	int		rs ;
	if ((rs = rmdir(fn)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiler::irmdir) */

int ufiler::isymlink(cchar *fn) noex {
	int		rs ;
	if ((rs = symlink(fn,dfn)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiler::isymlink) */

int ufiler::ixattrget(cchar *fn) noex {
	void		*val = const_cast<voidp>(xaval) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = getxattr(fn,xaname,val,sz,foff,oo)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (ufiler::ixattrget) */

int ufiler::ixattrset(cchar *fn) noex {
	int		rs ;
	if ((rs = setxattr(fn,xaname,xaval,sz,foff,oo)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiler::ixattrset) */

static sysret_t std_resolvepath(cchar *fn,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (fn && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fn[0]) {
		csize	rsize = size_t(rlen) ;
	        repeat {
	            if ((rs = resolvepath(fn,rbuf,rsize)) >= 0) {
		        if (rs <= rlen) {
			    rbuf[rs] = '\0' ;
		        } else {
			    rbuf[rlen] = '\0' ;
			    rs = SR_OVERFLOW ;
		        }
		    } else if (rs < 0) {
		        rs = (- errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (std_resolvepath) */



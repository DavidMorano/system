/* ufiler SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_access
	u_chdir
	u_chmod
	u_chown
	u_lchown
	u_link
	u_lstat
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
	path that does not contain any symbolic components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>		/* |S_IFMT| */
#include	<unistd.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<clanguage.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ufileop.h"


/* local defines */


/* imported namespaces */

using namespace	ufileop ;		/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structure */

namespace {
    struct ufiler ;
    typedef int (ufiler::*ufiler_m)(cchar *) noex ;
    struct ufiler : ufileopbase {
	ufiler_m	m = nullptr ;
	cchar		*dfn ;
	dev_t		dev ;
	mode_t		fm ;
	ufiler() noex { } ;
	ufiler(mode_t m,dev_t d) noex : fm(m), dev(d) { } ;
	ufiler(cchar *d) noex : dfn(d) { } ;
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
	int imknod(cchar *) noex ;
	int irename(cchar *) noex ;
	int irmdir(cchar *) noex ;
	int isymlink(cchar *) noex ;
    } ; /* end struct (ufiler) */
}


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;


/* exported variables */


/* exported subroutines */

int u_access(cchar *fname,int am) noex {
	int		rs ;
	repeat {
	    if ((rs = access(fname,am)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_access) */

int u_chdir(cchar *d) noex {
	int		rs ;
	repeat {
	    if ((rs = chdir(d)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_chdir) */

int u_chmod(cchar *name,mode_t m) noex {
	int		rs ;
	m &= (~ S_IFMT) ;
	repeat {
	    if ((rs = chmod(name,m)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_chmod) */

int u_chown(cchar *name,uid_t uid,gid_t gid) noex {
	int		rs ;
	repeat {
	    if ((rs = chown(name,uid,gid)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
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

int u_link(cchar *src,cchar *dst) noex {
	int		rs ;
	repeat {
	    if ((rs = link(src,dst)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_link) */

int u_lstat(cchar *fname,USTAT *sbp) noex {
	int		rs ;
	repeat {
	    if ((rs = lstat(fname,sbp)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_lstat) */

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
                        if ((name == _PC_CHOWN_RESTRICTED) && (lw == -1)) {
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
	int		rs ;
	repeat {
	    if ((rs = readlink(fname,rbuf,rlen)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	if (rs >= 0) {
	    rbuf[rs] = '\0' ;
	} else {
	    rbuf[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (u_readlink) */

int u_rename(cchar *fn,cchar *dfn) noex {
	ufiler		fo(dfn) ;
	fo.m = &ufiler::irename ;
	return fo(fn) ;
}
/* end subroutine (u_rename) */

int u_resolvepath(cchar *fn,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (fn && rbuf) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        repeat {
	            errno = 0 ;
	            if ((rs = resolvepath(fn,rbuf,rlen)) < 0) {
		        rs = (- errno) ;
	            }
	        } until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	        if ((rs >= 0) && (rs <= rlen)) {
	            rbuf[rs] = '\0' ;
	        }
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

int u_stat(cchar *fn,USTAT *sbp) noex {
	int		rs ;
	repeat {
	    if ((rs = stat(fn,sbp)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_stat) */

int u_statfs(cchar *fn,STATFS *sbp) noex {
	int		rs ;
	repeat {
	    if ((rs = statfs(fn,sbp)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_statfs) */

int u_statvfs(cchar *fn,STATVFS *sbp) noex {
	int		rs ;
	repeat {
	    if ((rs = statvfs(fn,sbp)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_statvfs) */

int u_symlink(cchar *fn,cchar *dfn) noex {
	ufiler		fo(dfn) ;
	fo.m = &ufiler::isymlink ;
	return fo(fn) ;
}
/* end subroutine (u_symlink) */

int u_truncate(cchar *path,off_t fo) noex {
	int		rs ;
	repeat {
	    if ((rs = truncate(path,fo)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_truncate) */

int u_unlink(cchar *fname) noex {
	int		rs ;
	repeat {
	    if ((rs = unlink(fname)) < 0) {
		rs = (- errno) ;
	    }
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_unlink) */


/* local subroutines */

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



/* ufiler SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:


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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structure */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_access(cchar *fname,int am) noex {
	int		rs ;
	repeat {
	    if ((rs = access(fname,am)) < 0) rs = (- errno) ;
	} until ((rs != SR_AGAIN) && (rs != SR_INTR)) ;
	return rs ;
}
/* end subroutine (u_access) */

int u_chdir(cchar *d) noex {
	int		rs ;

	repeat {
	    if ((rs = chdir(d)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_chdir) */

int u_chmod(cchar *name,mode_t m) noex {
	int		rs ;

	m &= (~ S_IFMT) ;
	repeat {
	    if ((rs = chmod(name,m)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_chmod) */

int u_chown(cchar *name,uid_t uid,gid_t gid) noex {
	int		rs ;

	repeat {
	    if ((rs = chown(name,uid,gid)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_chmod) */

int u_lchown(cchar *fname,uid_t uid,gid_t gid) noex {
	int		rs ;

	repeat {
	    if ((rs = lchown(fname,uid,gid)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_lchown) */

int u_link(cchar *src,cchar *dst) noex {
	int		rs ;

	repeat {
	    if ((rs = link(src,dst)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_link) */

int u_lstat(cchar *fname,struct ustat *sbp) noex {
	int		rs ;

	repeat {
	    if ((rs = lstat(fname,sbp)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_lstat) */

int u_mknod(cchar *path,mode_t m,dev_t dev) noex {
	int		rs = SR_FAULT ;
	if (path) {
	    rs = SR_INVALID ;
	    if (path[0]) {
		int	to_dquot = utimeout[uto_dquot] ;
		int	to_nospc = utimeout[uto_nospc] ;
		bool	f_exit = false ;
	        repeat {
	            if ((rs = mknod(path,m,dev)) < 0) rs = (- errno) ;
	            if (rs < 0) {
	                switch (rs) {
	                case SR_DQUOT:
	                    if (to_dquot -- > 0) {
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
	                case SR_AGAIN:
	                case SR_INTR:
	                    break ;
		        default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_mknod) */

int u_pathconf(cchar *fname,int name,long *rp) noex {
	long		lw ;
	int		rs = SR_OK ;
	errno = 0 ;
	lw = pathconf(fname,name) ;
	if ((lw < 0) && (errno != 0)) rs = (- errno) ;
#ifdef	_PC_CHOWN_RESTRICTED /* can you Solaris® guys fix this brain-damage? */
	if ((rs >= 0) && (name == _PC_CHOWN_RESTRICTED) && (lw == -1)) {
	   lw = 0 ;
	}
#endif /* _PC_CHOWN_RESTRICTED */
	if (rp)
	    *rp = (rs >= 0) ? lw : 0 ;
	}
	if (rs >= 0) rs = (lw & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_pathconf) */

int u_readlink(cchar *fname,char *rbuf,int rlen) noex {
	int		rs ;

	repeat {
	    if ((rs = readlink(fname,rbuf,rlen)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if (rs >= 0) {
	    rbuf[rs] = '\0' ;
	} else {
	    rbuf[0] = '\0' ;
	}

	return rs ;
}
/* end subroutine (u_readlink) */

int u_rename(cchar *old,cchar *new) noex {
	int		rs ;
	int		to_io = TO_IO ;
	int		to_dquot = TO_DQUOT ;
	int		to_nospc = TO_NOSPC ;
	int		to_busy = TO_BUSY ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = rename(old,new)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_IO:
	            if (to_io-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_DQUOT:
	            if (to_dquot -- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_BUSY:
	            if (to_busy-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error condition) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_rename) */


int u_resolvepath(cchar *fname,char *rbuf,int rlen) noex {
	int		rs ;

	if (input == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (input[0] == '\0') return SR_INVALID ;

	repeat {
	    errno = 0 ;
	    if ((rs = resolvepath(input,rbuf,rlen)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if ((rs >= 0) && (rs <= rlen)) {
	    rbuf[rs] = '\0' ;
	}

	return rs ;
}
/* end subroutine (u_resolvepath) */

int u_rmdir(cchar *fname) noex {
	int		rs ;
	int		to_io = TO_IO ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = rmdir(fname)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_IO:
	            if (to_io-- > 0) {
		        msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_rmdir) */

int u_statvfs(cchar *fname,STATVFS *sbp) noex {
	int		rs ;

	repeat {
	    if ((rs = statvfs(fname,sbp)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_statvfs) */

int u_symlink(cchar *src,cchar *dst) noex {
	int		rs ;
	int		to_io = TO_IO ;
	int		to_dquot = TO_DQUOT ;
	int		to_nospc = TO_NOSPC ;
	int		to_busy = TO_BUSY ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = symlink(src,dst)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_IO:
	            if (to_io-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_DQUOT:
	            if (to_dquot-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_BUSY:
	            if (to_busy-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error condition) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_symlink) */

int u_truncate(cchar *path,off_t fo) noex {
	int		rs ;

	repeat {
	    if ((rs = truncate(path,fo)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_truncate) */

int u_unlink(cchar *fname) noex {
	int		rs ;

	repeat {
	    if ((rs = unlink(fname)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_unlink) */



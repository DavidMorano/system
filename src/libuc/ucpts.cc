/* ucpts SUPPORT */
/* lang=C++20 */

/* get the filename (path) of a slave-pseudo terminal device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This is written to get a portable (reentrant and thread-safe)
	version of PTRNAME as we can get.

	= 2018-10-03, David A.D. Morano
	I modernized this by replacing custom path creation crap
	with a call to the |snsd(3uc)| subroutine.

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_ptsname

	Description:
	This code only has meaning on the newer System V UNIX®
	releases with the PTS dirver.  This is now needed to get
	the filename of the slave side device of the new pseudo-terminal
	clone multiplexor driver.  A new slave-side filename looks
	something like '/dev/pts/26'.  Unlike other versions of
	this sort of function, this is thread-safe!

	The algorithm (from SVR3 docs) is:

	Check that the FD argument is a file descriptor of an opened
	master.  Do this by sending an 'ISPTM' |ioctl(2)| message
	down stream.  The |ioctl(2)| call will fail if: (1) FD is
	not a valid file descriptor, (2) the file represented by
	FD does not understand 'ISPTM' (not a master device).  If we
	have a valid master, get its minor number via |fstat(2)|.
	Concatenate it to PTSPREFIX and return it as the name of
	the slave device.


	Name:
	uc_ptsgrant

	Description:
	Grant the use of a pseudo-terminal on System V Release 3
	(SVR3) UNIX® OS systems.


	Name:
	uc_ptsunlock

	Description:
	This subroutine unlocks a PTS-type pseudo-terminal on System
	V Release 3 (SVR3) UNIX® OS systems for use.


	Notes:
	Q. What operating systems support what?
	A. The question is really about what operating systems
	support the |ptsname_r(3c)| subroutine.  The following
	operating systems are known to support the |ptsname_r(3c)|
	subroutine:
	+ Darwin (macOS)
	+ Linux
	+ HPUX
	+ OSF1
	+ Tru64
	Q. What are the return values?
	A. On all operating systems that support the |ptsname_r(3c)|
	subroutine except for Linux, on error a '-1' is returned
	and 'errno' is set to the error code.  But on Linux, on
	error the subroutine returns 'not-zero' and sets the 'errno'
	variable to the error code.  Linux sucks cock meat.
	Q. What operating systems DO-NOT support the |ptsname_r(3c)|
	subroutine?
	A. SunOS Solaris!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>		/* |ptsname_r(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<ustropts.h>
#include	<snx.h>

#include	"ucpts.h"


/* local defines */

#define	PTSPREFIX	"/dev/pts/"	/* slave name */
#define	PTSMAXDEVS	1000000000	/* rather arbitrary */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;
constexpr bool		f_linux = F_LINUX ;


/* exported variables */


/* exported subroutines */

int uc_ptsname(int fd,char *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (nbuf) {
	    if constexpr (f_sunos) {
	        STRIOCTL	istr{} ;
		cint		req = I_STR ;
	        istr.ic_cmd = ISPTM ;
	        istr.ic_len = 0 ;
	        istr.ic_timout = 0 ;
	        istr.ic_dp = nullptr ;
	        if ((rs = u_ioctl(fd,req,&istr)) >= 0) {
	            USTAT	sb ;
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
		        cint	mdev = minor(sb.st_rdev) ;
	    	        rs = SR_INVALID ;
		        if (mdev < PTSMAXDEVS) {
			    cchar	*prefix = PTSPREFIX ;
		            if ((rs = snsd(nbuf,nlen,prefix,mdev)) >= 0) {
			        len = rs ;
		                rs = u_access(nbuf,0) ;
		            }
		        } /* end if (valid) */
	            } /* end if (u_fstat) */
	        } /* end if (u_ioctl) */
	    } else if constexpr (f_darwin || f_linux) {
		if ((rs = ptsname_r(fd,nbuf,nlen)) != 0) {
		    rs = (- errno) ;
		}
	    } /* end if-constexpr (f_farwin || f_linux) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_ptsname) */

int uc_ptsgrant(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = grantpt(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_ptsgrant) */

int uc_ptsunlock(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = unlockpt(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_ptsunlock) */



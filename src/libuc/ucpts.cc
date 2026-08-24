/* ucpts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the filename (path) of a slave-pseudo terminal device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A-D- Morano
	This is written to get a portable (reentrant and thread-safe)
	version of PTRNAME as we can get.

	= 2018-10-03, David A-D- Morano
	I modernized this by replacing custom path creation crap
	with a call to the |snsd(3uc)| subroutine.

	= 2025-11-09, David A-D- Morano
	I (finally) replaced the STREAMS® weirdo implementation
	with calling the (I think non-standard) |ptsname_t(3c)| 
	standard-library subroutine.  I find it amazing how long
	the original implementation of that lasted through the
	prior decades.  This shows how long any single -- properly
	working -- implmentation can survive!

*/

/* Copyright © 1998,2018,2025 David A-D- Morano.  All rights reserved. */

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
	+ Solaris (support is limited to OS versions from 11.1 onwards)

	Q. What are the return values?
	A. On all operating systems that support the |ptsname_r(3c)|
	subroutine except for Linux and Solaris®, on error a '-1'
	is returned and |errno| is set to the error code.  But on
	Linux, on error the subroutine returns 'not-zero' and sets
	the |errno| variable to the error code.  Linux sucks cock
	meat.  Also, on Solaris (version 11.1 and onwards that support it)
	on error it returns an ERRNO value directly.

	Q. What operating systems DO-NOT support the |ptsname_r(3c)|
	subroutine?
	A. Versions of Solaris before 11.1 did NOT support the 
	|ptsname_r(3c)| subroutine call.

	Q. What other problems does the version on (the latest) Solaris®
	have?
	A. On Solaris® dating from version 11.1, on error it returns
	an ERRNO value directly rather than -1 and setting the
	|errno| variable.

	Error-return-summary:
	Solaris		returns the ERRNO number directory, otherwise 0
	Linux		returns a non-zero on error and sets |errno|
	Darwin		-1 and sets |errno|
	SysV		I have no idea what it does

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |ptsname_r(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucpts.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	mkname(int,char *,int) noex ;


/* local variables */

cbool		f_linux		= F_LINUX ;
cbool		f_sunos		= F_SUNOS ;


/* exported variables */


/* exported subroutines */

int uc_ptsname(int fd,char *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	if (nbuf) ylikely {
	    rs = SR_INVALID ;
	    if (nlen >= 0) ylikely {
		if (syshas.ptsnamer) {
    	            csize	nsize = size_t(nlen) ;
	            errno = 0 ;
	            if ((rs = ptsname_r(fd,nbuf,nsize)) != 0) {
		        if_constexpr (f_linux) {
	                    rs = (neg errno) ;
		        } else if_constexpr (! f_sunos) {
			    if (rs < 0) {
	                        rs = (neg errno) ;
			    } else {
		                rs = lenstr(nbuf) ;
			    }
		        } /* end if_constexpr (f_linux) */
	            } else {
		        rs = lenstr(nbuf) ;
		    }
		} else {
		    rs = mkname(fd,nbuf,nlen) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_ptsname) */

int uc_ptsgrant(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = grantpt(fd)) < 0) {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (uc_ptsgrant) */

int uc_ptsunlock(int fd) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = unlockpt(fd)) < 0) {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (uc_ptsunlock) */


/* local subroutines */

local int mkname(int fd,char *nbuf,int nlen) noex {
    	int		rs ;
	if (cchar *rp = ptsname(fd) ; rp) { /* <- thread-safe! */
	    rs = sncpy(nbuf,nlen,rp) ;
	} else {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (mkname) */



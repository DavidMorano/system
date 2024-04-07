/* nisdomainname SUPPORT */
/* lang=C++20 */

/* get the NIS domain name for the current host */
/* version %I% last-modified %G% */

#define	CF_DEBUGFILE	0		/* test the file-reading code */

/* revision history:

	= 1995-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nisdomainname

	Description:
	Get the NIS domain name for the local host.

	Synopsis:
	int nisdomainname(char *dbuf,int dlen) noex

	Arguments:
	dbuf		buffer to place name in
	dlen		length of user-supplied buffer

	Returns:
	>=0		length of NIS domain name
	<0		error (system-return)

	Implementation:

	We try to find the NIS domain in the following order:
	1. envionment variable 'NISDOMAIN'
	2. from the kernel (using |sysinfo(2)|)
	3. from reading the file '/etc/defaultdomain'

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<sys/utsname.h>

#if		defined(SYSHAS_SYSINFO) && (SYSHAS_SYSINFO > 0)
#include	<sys/systeminfo.h>
#endif

#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<filer.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"nisdomainname.h"


/* local defines */

#define	VARNISDOMAIN	"NISDOMAIN"

#if	CF_DEBUGFILE
#define	NISDOMAINNAME	"defaultdomain"
#else
#define	NISDOMAINNAME	"/etc/defaultdomain"
#endif

#define	TO_NISFILE	30		/* time-out for NIS file access */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	nisfile(char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int nisdomainname(char *rbuf,int rlen) noex {
	int		rs = SR_OK ;

	if (rbuf == NULL) return SR_FAULT ;

	if (rlen < 1) return SR_OVERFLOW ;

	rbuf[0] = '\0' ;

/* try the environment */

	{
	    cchar	*var = VARNISDOMAIN ;
	    cchar	*cp ;
	    if (((cp = getenv(var)) != NULL) && (cp[0] != '\0')) {
	        rs = sncpy1(rbuf,rlen,cp) ;
	    }
	} /* end block (environment variable) */

/* try the system kernel */

#if	(CF_DEBUGFILE == 0)
#if	defined(SYSHAS_SYSINFO) && (SYSHAS_SYSINFO > 0)
#if	defined(SI_SRPC_DOMAIN)
	if ((rs >= 0) && (rbuf[0] == '\0')) {
	    rs = u_sysinfo(SI_SRPC_DOMAIN,rbuf,rlen) ;
	} /* end if (from the kernel) */
#endif /* defined(SI_SRPC_DOMAIN) */
#endif
#endif /* (CF_DEBUGFILE == 0) */

/* try the NIS domain-name file */

	if ((rs >= 0) && (rbuf[0] == '\0')) {
	    rs = nisfile(rbuf,rlen,NISDOMAINNAME) ;
	}

	if ((rs >= 0) && (rbuf[0] == '\0')) {
	    rs = SR_NOTAVAIL ;
	}

	return rs ;
}
/* end subroutine (nisdomainname) */


/* local subroutines */

static int nisfile(char *rbuf,int rlen,cchar *fname) noex {
	cint		to = TO_NISFILE ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;

	if (rbuf == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) {
	    filer	fb ;
	    cint	fd = rs ;

	    if ((rs = filer_start(&fb,fd,0L,256,0)) >= 0) {
		cint		llen = LINEBUFLEN ;
		int		len ;
		int		cl ;
		cchar		*tp ;
		cchar		*cp ;
		char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = filer_readln(&fb,lbuf,llen,to)) > 0) {
	            len = rs ;

		    if (lbuf[len-1] == '\n') len -= 1 ;

		    if ((tp = strnchr(lbuf,len,'#')) != NULL) {
			len = (tp-lbuf) ;
		    }

		    if (lbuf[0] != '#') {
	                if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
	    		    while ((cl > 0) && (cp[cl - 1] == '.')) cl -= 1 ;
	    		    rs = snwcpy(rbuf,rlen,cp,cl) ;
			    rl = rs ;
			}
		    }

		    if (rl > 0) break ;
		    if (rs < 0) break ;
	        } /* end while */

	        rs1 = filer_finish(&fb) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (reading file) */

	    rs1 = uc_close(fd) ;
	} /* end if (open) */

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (nisfile) */



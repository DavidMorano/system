/* progexpand SUPPORT */
/* lang=C++20 */

/* expand out some per program parameters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	progexpand

	Descrption: 
	This subroutine expands out some per program (the daemon
	program) parameters into the configuration strings.  Actually
	this little subroutine is used by many programs.

#	The following substitutions are made on command strings:

#		%V	version string
#		%B	programroot basename
#		%P	program name
#		%S	program search name
#		%N	machine node name
#		%D	machine INET domain name
#		%H	machine INET host name
#		%U	invoking username
#		%G	invoking groupname
#		%R	program root directory
#

	Synopsis:
	int progexpand(proginfo *pip,char *rbuf,int rlen,cchar *sp,int sl) noex

	Arguments:
	pip		program-information-pointer
	rbuf		result buffer pointer
	rlen		result buffer length
	sp		source pointer
	sl		source length

	Returns:
	>=0		resulting length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progexpand(proginfo *pip,char *rbuf,int rlen,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		elen = 0 ;
	if (pip && rbuf && sbuf) {
	cint		hlen = MAXHOSTNAMELEN ;
	int		len = strnlen(sbuf,slen) ;
	int		cl ;
	cchar		*bp = sbuf ;
	cchar		*cp ;
	char		hbuf[MAXHOSTNAMELEN + 1] ;
	char		*rbp = rbuf ;
	while ((len > 0) && (elen < rlen)) {
	    cint	ch = MKCHAR(*bp) ;
	    switch (ch) {
	    case '%':
	        bp += 1 ;
	        len -= 1 ;
	        if (len == 0) break ;
		cl = 0 ;
	        switch (ch) {
	        case 'V':
	            cp = pip->version ;
	            cl = strlen(cp) ;
	            break ;
#if	defined(DEFS_ROOTNAME) && (DEFS_ROOTNAME > 0)
	        case 'B':
		    if ((cp = pip->rootname) != NULL) {
	            	cl = strlen(cp) ;
		    }
	            break ;
#endif /* P_RCPMUXD */
	        case 'P':
	            cp = pip->progname ;
	            cl = strlen(cp) ;
	            break ;
	        case 'S':
		    if ((cp = pip->searchname) != NULL) {
	            	cl = strlen(cp) ;
		    }
	            break ;
	        case 'N':
	            cp = pip->nodename ;
	            cl = strlen(cp) ;
	            break ;
	        case 'D':
	            cp = pip->domainname ;
	            cl = strlen(cp) ;
	            break ;
	        case 'H':
	            cp = hbuf ;
	            rs = snsds(hbuf,hlen,pip->nodename,pip->domainname) ;
		    cl = rs ;
	            break ;
/* handle the expansion of our program root */
	        case 'R':
	            if ((cp = pip->pr) != NULL) {
	                cl = strlen(cp) ;
		    }
	            break ;
	        case 'U':
		    if ((cp = pip->username) != NULL) {
			cl = strlen(cp) ;
		    }
	            break ;
	        case 'G':
		    if ((cp = pip->groupname) != NULL) {
			cl = strlen(cp) ;
		    }
	            break ;
	        default:
	            cp = bp ;
	            cl = 1 ;
		    break ;
	        } /* end switch */
	        bp += 1 ;
	        len -= 1 ;
	        if ((elen + cl) > rlen) {
		    rs = SR_OVERFLOW ;
	            break ;
		}
	        if (cl > 0) strncpy(rbp,cp,cl) ;
	        rbp += cl ;
	        elen += cl ;
	        break ;
	    default:
	        *rbp++ = *bp++ ;
	        elen += 1 ;
	        len -= 1 ;
		break ;
	    } /* end switch */
	} /* end while */
	if ((rs >= 0) && (elen >= rlen)) {
	    rs = SR_OVERFLOW ;
	}
	rbuf[elen] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? elen : rs ;
}
/* end subroutine (progexpand) */



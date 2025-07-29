/* mkutmpid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a UTMPX entry ID (c_string) */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-10-09, David A­D­ Morano
	I finally made this because so many programs have difficulty
	in creating UTMP ID values.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkutmpid

	Description:
	This subroutine is used to create UTMP ID values.  These
	values are (generally) four-character strings (without
	terminating NULs) that are used in the ID field of the UTMP
	record.  Many programs often struggle to create correct or
	valid IDs that follow historic UNIX conventions, so we are
	trying to help out with this subroutine!

	Special note: This subroutine will not NUL terminate the
	ID buffer if the created ID is the maximum length of the
	supplied buffer.  This is fine when the buffer is the exact
	buffer of the ID field in a UTMP record (for example) but
	this is not good if you want to try to print the buffer out
	as a normal string.  Supply a buffer one character larger
	than needed and NUL terminate it at the end if you want to
	treat the resulting buffer as a normal C-language string
	(NUL terminated).

	Note on IDs:

	Certain ID prefixes are reserved.  Generally, and in the
	"old" days, IDs without any leading alphabetic characters
	were taken to be terminal logins.  Other IDs with lower-case
	alphabetic letter prefixes were other IDs usually from the
	INITTAB file that were not associated with terminal logins.
	However, with the advent of "pseudo" logins using pseudo
	terminals, other schemes for assigning IDs needed to be
	developed since most BSD-like pseudo terminal logins occurred
	without using any mechanism derived from the INIT program
	at all!

	The following have been in use (but are not necessarily endorsed
	here):

	r		RLOGIN
	t		TELNET

	The following are strongly recommended for all future use:

	P		any pseudo terminal with a device prefix 'pts'
	R		any pseudo terminal with a device prefix 'rx'
	X		any pseudo terminal with a device prefix 'xt'

	It is recommended that both RLOGIN and TELNET convert over
	to using the upper case prefixes as above. Doing so will
	avoid clashes with the use of some older style pseudo terminals.

	Also, note that lower case prefixes may be used by other
	services that use the old BSD-style pseudo terminals that
	have device names starting with a lower case letter.  For
	example, a pseudo terminal with a device file name of 'ttyp8'
	would get an ID of 'p8'.  See the problem!  Services that
	use these terminal devices are naturally going to be using
	lower case letters between about 'p' and up to possibly
	'z', depending on how many of these pseudo terminals are
	configured into the system.  It is strongly recommended
	that lower case letter IDs from INITAB not use lower case
	letters in this range.  This gives preference to the BSD-style
	pseudo terminals, but so be it.  They need IDs also and it
	is not clear what to do for them anyway.  Many services
	have algorithms that take the device name (like 'ttyp8')
	and strips the 'tty' part off and then uses the remainder
	for the ID.  This seems reasonable to keep since at least
	the range of lower case letters used is manageable.

	It seems reasonable that ID usage with additional upper
	case alphabetic characters should be considered for future
	needs.

	Synopsis:
	int mkutmpid(char *idbuf,int idlen,cchar *tbuf,int tlen)

	Arguments:
	idbuf		user supplied buffer to receive result
	idlen		length of user supplied buffer
	tbuf		terminal device c-string pointer
	tlen		terminal device c-string length

	Returns:
	>=0		OK, and length of resulting ID string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strnxchr.h>
#include	<localmisc.h>

#include	"mkutmpid.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct prefix {
	cchar		*name ;
	cchar		*prefix ;
    } ;
    struct tmper ;
    typedef int (tmper::*tmp_m)(cchar *,int) noex ;
    struct tmper {
	cchar	*sbuf ;
	char	*rbuf ;
	int	rlen ;
	int	slen ;
	tmper(char *rp,int rl,cchar *dp,int dl) noex {
	    sbuf = dp ;
	    rbuf = rp ;
	    slen = dl ;
	    rlen = rl ;
	} ; /* end ctor */
	operator int () noex ;
	int extdev(cchar **) noex ;
	int subdirs(cchar *,int) noex ;
	int basename(cchar *,int) noex ;
	int special(cchar *,int) noex ;
	int something(cchar *,int) noex ;
    } ; /* end struct (tmper) */
}


/* forward references */

static int	idcpy(char *,int,cchar *,int,cchar *,int) noex ;


/* local variables */

static constexpr struct prefix	prefixes[] = {
	{ "pts", "P" },
	{ "term", "" },
	{ "tty", "" },
	{ "rx", "R" },
	{ "xt", "X" },
	{ nullptr, nullptr }
} ;

static constexpr struct prefix	specials[] = {
	{ "console", "co" },
	{ "syscon", "cs" },
	{ nullptr, nullptr }
} ;

static tmp_m	tmpcalls[] {
	&tmper::subdirs,
	&tmper::basename,
	&tmper::special,
	&tmper::something,
	nullptr
} ;

static constexpr cchar		dpre[] = "/dev/" ;	/* device prefix */


/* exported variables */


/* exported subroutines */

int mkutmpid(char *idbuf,int idlen,cchar *devbuf,int devlen) noex {
	tmper	to(idbuf,idlen,devbuf,devlen) ;
	return to ;
}
/* end subroutine (mkutmpid) */


/* local subroutines */

tmper::operator int () noex {
	int		rs = SR_FAULT ;
	if (rbuf && sbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (rlen >= 1) ylikely {
		rs = SR_DOM ;
	        memclear(rbuf,rlen) ;
	        if (slen < 0) slen = lenstr(sbuf) ;
		if (slen > 0) ylikely {
		    if (cchar *lp ; (rs = extdev(&lp)) >= 0) ylikely {
		        cint	ll = rs ;
		        rs = 0 ;
		        for (int i = 0 ; (rs == 0) && tmpcalls[i] ; i += 1) {
		            tmp_m	m = tmpcalls[i] ;
		            rs = (this->*m)(lp,ll) ;
		        } /* end for */
		    } /* end if (extdev) */
		} /* end if (domain) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (tmper::operator) */

int tmper::extdev(cchar **rpp) noex {
	static constexpr int	dl = lenstr(dpre) ;
	int		rs = SR_OK ;
	int		ll = slen ;
	cchar		*lp = sbuf ;
	if (ll >= dl) {
	    if (strncmp(lp,dpre,dl) == 0) {
	        lp += dl ;
	        ll -= dl ;
	    }
	} /* end if (possible) */
	if (ll < 1) rs = SR_INVALID ;
	*rpp = lp ;
	return (rs >= 0) ? ll : rs ;
}
/* end method (tmper::extdev) */

int tmper::subdirs(cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if ((ll > 0) && (rbuf[0] == '\0')) ylikely {
	    if (cchar *tp ; (tp = strnchr(lp,ll,'/')) != nullptr) ylikely {
	        cint	sl = intconv(tp - lp) ;
	        cint	cl = (ll - intconv((tp + 1) - lp)) ;
	        int	pl = 0 ;
	        int	i ; /* used-afterwards */
	        cchar	*sp = lp ;
	        cchar	*cp = (tp + 1) ;
	        cchar	*pp{} ;
	        for (i = 0 ; prefixes[i].name ; i += 1) {
	            pp = prefixes[i].name ;
	            pl = lenstr(pp) ;
	            if ((pl == sl) && (strncmp(pp,sp,pl) == 0)) break ;
	        } /* end for */
	        if (prefixes[i].name && pp && pl) ylikely {
	            rs = idcpy(rbuf,rlen,pp,pl,cp,cl) ;
	        } /* end if */
	    } /* end if (tried for a directory match) */
	} /* end if (needed) */
	return rs ;
}
/* end method (tmper::subdirs) */

int tmper::basename(cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if ((ll > 0) && (rbuf[0] == '\0')) ylikely {
	    int		i ; /* used-afterwards */
	    int		cl = ll ;
	    int		pl = 0 ;
	    cchar	*cp = lp ;
	    cchar	*pp{} ;
	    for (i = 0 ; prefixes[i].name ; i += 1) {
	        pp = prefixes[i].name ;
	        pl = lenstr(pp) ;
	        if ((pl <= cl) && (strncmp(pp,lp,pl) == 0)) break ;
	    } /* end for */
	    if (prefixes[i].name && pp && pl) ylikely {
	        rs = idcpy(rbuf,rlen,pp,pl,cp,cl) ;
	    } /* end if */
	} /* end if (needed) */
	return rs ;
}
/* end method (tmper::basename) */

int tmper::special(cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if ((ll > 0) && (rbuf[0] == '\0')) ylikely {
	    int		i = 0 ;
	    int		pl = 0 ;
	    cchar	*pp{} ;
	    for (i = 0 ; specials[i].name ; i += 1) {
	        pp = specials[i].name ;
		pl = lenstr(pp) ;
	        if (strcmp(pp,lp) == 0) break ;
	    } /* end for */
	    if (prefixes[i].name && pp && pl) ylikely {
	        rs = idcpy(rbuf,rlen,pp,pl,nullptr,0) ;
	    } /* end if */
	} /* end if (needed) */
	return rs ;
}
/* end method (tmper::special) */

int tmper::something(cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if ((ll > 0) && (rbuf[0] == '\0')) ylikely {
	    int		cl = 0 ;
	    cchar	*cp ;
	    if ((cp = strnchr(lp,ll,'/')) != nullptr) {
	        cp += 1 ;
	        cl = intconv(ll - (cp - lp)) ;
	    } else {
	        cp = lp ;
	        cl = ll ;
	    } /* end if */
	    if (cl > rlen) {
	        rs = idcpy(rbuf,rlen,nullptr,0,(cp+(cl-rlen)),rlen) ;
	    } else if (cp) {
	        rs = idcpy(rbuf,rlen,nullptr,0,cp,cl) ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end method (tmper::something) */

static int idcpy(char *idbuf,int idlen,cchar *pp,int pl,cchar *cp,int cl) noex {
	int		rs ;
	int		j = 0 ;
	for (int k = 0 ; (j < idlen) && (k < pl) ; j += 1) {
	    idbuf[j] = pp[k++] ;
	}
	for (int k = 0 ; (j < idlen) && (k < cl) ; j += 1) {
	    idbuf[j] = cp[k++] ;
	}
	rs = j ; /* <- return value */
	while (j < idlen) {
	    idbuf[j++] = '\0' ;
	}
	return rs ;
}
/* end subroutine (idcpy) */



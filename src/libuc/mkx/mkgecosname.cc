/* mkgecosname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make (or get) a GECOS name out of the raw PASSWD c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-12, David A­D­ Morano
	This code was originally adopted from a similar
	subroutine that was a part of the PCS package distribution.
	However, the original was garbage so this version is
	essentially a complete rewrite of the original.  Also the
	name of the subroutine is changed from the original.  The
	original name of the subroutine was GECOSNAME.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
									
	Name:
	mkgecosname

	Description:
	This subroutine gets the GECOS version of a user name (a
	type of a real name) from the fifth field of the system
	password file ('/etc/passwd').

	Synopsis:
	int mkgecosname(char *rbuf,int rflen,cchar *gf) noex

	Arguments:
	rbuf		user supplied result buffer to contain the full-name
	rlen		length of the user supplied result buffer
	gf		input buffer containing the GECOS field string

	Returns:
	>=0	length of GECOS name
	<0	failed (system-return)


	Name:
	getgecosname

	Description:
	This subroutine is the core workhorse of the function.
	It is used in the implementation of |mkgecosname(3uc)| above.
	It is provided for historical reasons, since some old
	routines used this in the past.

	Synopsis:
	int getgecosname(cchar *gbuf,int glen,cchar **rpp) noex

	Arguments:
	gbuf		source c-string pointer
	glen		source c-string length
	rpp		pointer to pointer to hold result (constant-char)

	Returns:
	>=0		succeded and this is the result length
	<0		error (system-return)


	Notes:
	The GECOS field of the PASSWD database should be formatted
	in one of the following ways:

	    name,office,work_phone,home_phone
	    organization-name(account,bin)office,work_phone,home_phone,printer
	    name(account)office,work_phone,home_phone,printer
	    name

	Note also that an ampersand character ('&') that appears
	anywhere in the GCOS field is to be logically replaced by
	the corresponding username of the entry.

	The 'name' part of the GCOS entry may contain hyphen ('-')
	characters.

	The original AT&T GECOS field contained:

	    department-name(account,bin)

	and was further put into a 'struct comment' with fields:

	    c_dept
	    c_name
	    c_acct
	    c_bin

	Some modern possibilities for the GECOS field are:

	    org_dept-name(account,bin)office,work_phone,home_phone
	    org_dept-name(account,bin)office,work_phone,home_phone,printer

	Actual examples:

	    XNR64430-d.a.morano(126483,BIN8221)
	    rockwell-d.a.morano(126283,BIN8221)4B-411,5336,6175679484,hp0

	Design note:

	Note that in the |getgecosname(3dam)| subroutine below, we
	use the subroutine |strnlen(3dam)| to get the length of the
	supplied GECOS string length if one was not also given.
	This is done in order to guard against the problem of an
	indefinitely long GECOS string being given to us (in order
	words: garbage) for whatever reason.

	I do not really know how an indefinitely long GECOS string
	-- and one that doesn't have traditional GECOS special
	characters in it -- could get passed to us from reasonable
	callers.  But then again, not all of our callers are always
	reasonable!

	Our goal is to prevent stray memory references -- in order
	to avoid core dumps!  Someone else can have core dumps if
	they want them -- but we do not!  Deal with it!

	If everyone guarded their subroutines the way that we try
	to do, maybe there would not be so much crap out there --
	like all of the core dumping that is going on!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<snwcpy.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"mkgecosname.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */

#ifndef	CH_LPAREN
#define	CH_LPAREN	0x28
#endif

#ifndef	MAXGECOSLEN
#define	MAXGECOSLEN	1024
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	maxgecoslen = MAXGECOSLEN ;


/* exported variables */


/* exported subroutines */

int mkgecosname(char *rbuf,int rlen,cchar *gf) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && gf) ylikely {
	    if (cchar *cp{} ; (rs = getgecosname(gf,-1,&cp)) >= 0) ylikely {
		cint	cl = rs ;
	        rs = snwcpyhyphen(rbuf,rlen,cp,cl) ;
		rl = rs ;
	    } /* end if (getgecosname) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkgecosname) */

int getgecosname(cchar *gbuf,int µglen,cchar **rpp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		cl = 0 ; /* return-value */
	cchar		*cp = nullptr ;
	if (gbuf) ylikely {
	    rs = SR_OK ;
	    if (int glen ; (glen = getlenstr(gbuf,µglen)) > 0) ylikely {
	        cint	sch = CH_LPAREN ;
	        cchar	*tp ;
	        bool	f = true ;
	        f = f && ((cp = strnchr(gbuf,glen,'-')) != np) ;
	        {
		    cint	tl = intconv(glen - (cp - gbuf)) ;
	            f = f && ((tp = strnchr(cp,tl,sch)) != np) ;
	        }
	        if (f) {
	            cp += 1 ;
	            cl = intconv(tp - cp) ;
	        } else if ((tp = strnchr(gbuf,glen,sch)) != np) {
	            cp = gbuf ;
	            cl = intconv(tp - gbuf) ;
	        } else if ((tp = strnchr(gbuf,glen,'-')) != np) {
	            cp = (tp + 1) ;
	            cl = glen - intconv((tp + 1) - gbuf) ;
	        } else {
	            cp = gbuf ;
	            cl = glen ;
	        } /* end if */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	if (rpp) *rpp = cp ;
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (getgecosname) */



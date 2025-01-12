/* readcmdkey SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* read a "command" type of key (keyboard key-press) from the keyboard */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	readcmdkey

	Description:
	This subroutine processes characters in order to determine
	what type of control sequence we have.  The first character
	of the sequence is provided when we are called.  Subsequent
	characters are read by us.

	Synopsis:
	int readcmdkey(readcmdkey *ckp,uterm *utp,int to,int ch) noex

	Arguments:
	ckp		supplied structure for results
	utp		UTERM object-pointer 
	to		time-out
	ch		initial (first) character

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<uterm.h>
#include	<ascii.h>
#include	<cfdec.h>
#include	<kbdinfo.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"readcmdkey.h"


/* local defines */

#define	TR_OPTS		(FM_NOFILTER | FM_NOECHO | FM_RAWIN | FM_TIMED)


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */

static int isinter(int) noex ;
static int isfinalesc(int) noex ;
static int isfinalcsi(int) noex ;
static int isparam(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int readcmdkey(readcmdkey *ckp,uterm *utp,int to,int ch) noex {
	cint	llen = LINEBUFLEN ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	i ;
	int	pi = -1 ;
	int	pl = -1 ;
	int	ktype = -1 ;
	int	kinter = -1 ;
	int	ropts = TR_OPTS ;
	int	n = 1 ;
	int	f_csi ;
	int	f ;

	char	lbuf[LINEBUFLEN + 1], *lp ;


	if (ckp == NULL)
	    return SR_INVALID ;

	if (utp == NULL)
	    return SR_INVALID ;

	memclear(ckp) ;

	f_csi = (ch == CH_CSI) ;

	lbuf[0] = '\0' ;
	for (i = 0 ; (rs >= 0) && (i < llen) ; ((i += 1),(n += 1))) {

	    lp = (lbuf + i) ;
	    rs = uterm_reade(utp,lp,1,to,ropts,NULL,NULL) ;

	    if (rs < 0)
	        break ;

	    ch = (*lp & 0xff) ;
	    if (f_csi) {

	        if (pi < 0) pi = i ;

	        f = isfinalcsi(ch) ;

	        if (f) {
	            ktype = (ch == '~') ? KBDINFO_TFKEY : KBDINFO_TCSI ;
	            break ;
	        }

	        f = isinter(ch) ;

	        if (f) {
	            if (pl < 0) pl = (i - pi) ;
	        }

	        f = f || isparam(ch) ;
	        if (! f) break ;

	    } else if ((n == 1) && (ch == CH_LBRACK)) {

	        f_csi = TRUE ;

	    } else if ((n == 1) && (ch == 'O')) {

	        ktype = KBDINFO_TPF ;

	    } else if (ktype == KBDINFO_TPF) {

	        kinter = ch ;
	        break ;

	    } else {

	        if (pi < 0) pi = i ;

	        f = isfinalesc(ch) ;

	        if (f) {
	            ktype = KBDINFO_TESC ;
	            kinter = ch ;
	            break ;
	        }

	        f = isinter(ch) ;

	        if (! f)
	            break ;

	    } /* end if */

	} /* end for (reading characters) */

/* parse any parameter */

	if (f_csi && (ktype >= 0)) {
	    int	cl ;
	    const char	*cp, *tp ;


	    if (pl < 0) pl = (i - pi) ;

	    cp = (lbuf + pi) ;
	    cl = pl ;

	    if (cl > 0) {

	        if ((tp = strnchr(cp,cl,';')) != NULL)
	            cl = (tp - cp) ;

	        rs1 = cfdeci(cp,cl,&kinter) ;
	        if (rs1 < 0)
	            ktype = -1 ;

	    } else
	        kinter = ch ;

	} /* end if */

	ckp->type = ktype ;
	ckp->inter = kinter ;

	return rs ;
}
/* end subroutine (readcmdkey) */


/* local subroutines */

static int isinter(int ch) noex {
	return ((ch >= 0x20) && (ch <= 0x2F)) ;
}
/* end subroutines (isinter) */

static int isfinalesc(int ch) noex {
	return ((ch >= 0x30) && (ch <= 0x7E)) ;
}
/* end subroutines (isfinalesc) */

static int isfinalcsi(int ch) noex {
	return ((ch >= 0x40) && (ch <= 0x7E)) ;
}
/* end subroutines (isfinalcsi) */

static int isparam(int ch) noex {
	return ((ch >= 0x30) && (ch <= 0x3F)) ;
}
/* end subroutines (isparam) */



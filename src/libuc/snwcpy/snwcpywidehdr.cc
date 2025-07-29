/* snwcpywidehdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* counted-string copy while compacting white-space from the source */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-10 David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpywidehdr

	Description:
	Similar to |snwcpycompact(3uc)| except that the source
	consists of wide-characters. This is meant to be used on
	wide-character strongs taken from mail-msg header values
	(consisting of string data).

	Synopsis:
	int snwcpywidehdr(char *dbuf,int dlen,cwchar *wp,int wl) noex

	Arguments:
	dbuf		result buffer
	dlen		length of supplied result buffer
	sp		source string
	sl		source string length

	Returns:
	>=0		resulting string length
	<0		error (system-return)

	Implemetation note:
	We could have used either |sbuf(3uc)| or |storebuf(3uc)|
	or what a few other subroutines of this ilk have done. But
	this subroutine was written at a time before resort to those
	interfaces was automatic.  It is a little messy, but it
	works just fine as it is!

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpywidehdr(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* presumably for |wchar_t| type */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<strmgr.h>
#include	<straltwchar.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int		wsfnext(cwchar *,int,cwchar **) noex ;
    extern int		wsnlen(cwchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		ch_sub = mkchar('¿') ;


/* exported variables */


/* exported subroutines */

int snwcpywidehdr(char *dbuf,int dlen,cwchar *wsp,int wsl) noex {
	int		rs ;
	int		rs1 ;
	int		dl = 0 ;
	if (dlen < 0) dlen = INT_MAX ;
	if (wsl < 0) wsl = wsnlen(wsp,-1) ;
	if (strmgr m ; (rs = m.start(dbuf,dlen)) >= 0) {
	    cwchar	*wp ;
	    for (int wl ; (wl = wsfnext(wsp,wsl,&wp)) > 0 ; ) {
	        if (dl > 0) {
	            rs = m.chr(' ') ;
	            if (rs >= 0) dl += 1 ;
	        }
	        if (rs >= 0) {
		    int		i ; /* used-afterwards */
	            for (i = 0 ; (rs >= 0) && (i < wl) && wp[i] ; i += 1) {
	                if (uint wch ; (wch = (uint) wp[i]) > UCHAR_MAX) {
			    if (cc *ss ; (ss = straltwchar(wch)) != nullptr) {
	                        rs = m.str(ss) ;
			    } else {
	                        rs = m.chr(ch_sub) ;
			    }
			} else {
			    if (isprintbad(wch)) {
				wch = ch_sub ;
			    }
	                    rs = m.chr(wch) ;
			} /* end if */
	            } /* end for */
	            if (rs >= 0) dl += i ;
	        } /* end if (ok) */
	        wsl -= intconv((wp + wl) - wsp) ;
	        wsp = (wp + wl) ;
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = m.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (strmgr) */
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpywidehdr) */



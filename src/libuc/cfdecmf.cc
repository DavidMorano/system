/* cfdecmf SUPPORT */
/* lang=C++20 */

/* Convert-From-Decimal-Multiply-Factor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	cfdecmfi
	cfdecmfl
	cfdecmfll
	cfdecmfui
	cfdecmful
	cfdecmfull

	Description:
	These are subroutines to convert decimal strings to binary
	integer value but with optional multiplying factors (letters)
	attached to the end of the given string.  The value represented
	by the string (without the multiplying factor) is multiplied
	by the multiplying factor.

	multiply-factor		value
	-------------------------------

	b			512
	k			1024
	m			(1024*1024)
	g			(1024*1024*1024)
	kb			(1024*512)
	mb			(1024*1024*512)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<char.h>
#include	<ascii.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"cfdecmf.h"


/* local defines */

#undef	UBLOCKSIZE
#define	UBLOCKSIZE	512

#define	OUR_ISWHITE(c)	(CHAR_ISWHITE(c) || ((c) == CH_NBSP))
#define	TOLC(ch)	CHAR_TOLC(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getmf(cchar *,int,ulong *) noex ;


/* local variables */


/* local subroutine-templates */

template<typename T>
int cfdecmfx(int (*cfdecx)(cchar *,int,T *),cchar *sp,int sl,T *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    ulong	mf{} ;
	    int		ml = getmf(sp,sl,&mf) ;
	    T		v ;
	    if ((rs = cfdecx(sp,ml,&v)) >= 0) {
	        rs = int((v * mf) & INT_MAX) ;
	        if (rp) *rp = (v * mf) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (cfdecmfx) */


/* exported subroutines */

int cfdecmfi(cchar *sbuf,int slen,int *rp) noex {
	return cfdecmfx(cfdeci,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmfi) */

int cfdecmfui(cchar *sbuf,int slen,uint *rp) noex {
	return cfdecmfx(cfdecui,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmfui) */

int cfdecmfl(cchar *sbuf,int slen,long *rp) noex {
	return cfdecmfx(cfdecl,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmfl) */

int cfdecmful(cchar *sbuf,int slen,ulong *rp) noex {
	return cfdecmfx(cfdecul,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmful) */

int cfdecmfll(cchar *sbuf,int slen,longlong *rp) noex {
	return cfdecmfx(cfdecll,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmfll) */

int cfdecmfull(cchar *sbuf,int slen,ulonglong *rp) noex {
	return cfdecmfx(cfdecull,sbuf,slen,rp) ;
}
/* end subroutine (cfdecmfull) */


/* local subroutines */

static int getmf(cchar *sbuf,int slen,ulong *rp) noex {
	ulong		mf = 1 ;
	int		sl = strnlen(sbuf,slen) ;
	uchar		*ubuf = (uchar *) sbuf ;
	while ((sl > 0) && OUR_ISWHITE(ubuf[sl - 1])) {
	    sl -= 1 ;
	}
	if (sl > 0) {
	    if (TOLC(ubuf[sl-1]) == 'b') {
	        mf = UBLOCKSIZE ;
	    } else if (TOLC(ubuf[sl-1]) == 'k') {
	        mf = 1024 ;
	    } else if (TOLC(ubuf[sl-1]) == 'm') {
	        mf = 1024 * 1024 ;
	    } else if (TOLC(ubuf[sl-1]) == 'g') {
	        mf = 1024 * 1024 * 1024 ;
	    } else if (sl > 1) {
	        if ((TOLC(ubuf[sl-2]) == 'k') && (ubuf[sl-1] == 'b')) {
	            mf = 1024 * UBLOCKSIZE ;
	            sl -= 1 ;
	        } else if ((TOLC(ubuf[sl-2]) == 'm') && (ubuf[sl-1] == 'b')) {
	            mf = 1024 * 1024 * UBLOCKSIZE ;
	            sl -= 1 ;
		} /* end if */
	    } /* end if */
	    if (mf > 1) {
	        sl -= 1 ;
		while ((sl > 0) && OUR_ISWHITE(ubuf[sl-1])) {
	    	    sl -= 1 ;
		}
	    } /* end if (adjusting buffer length) */
	} /* end if */
	*rp = mf ;
	return sl ;
}
/* end subroutine (getmf) */



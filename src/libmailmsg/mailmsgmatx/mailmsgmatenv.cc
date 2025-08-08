/* mailmsgmatenv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to match on a UNIX® mail-message envelope */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailmsgmatenv

	Description:
	This subroutine detects if a mail-message "environment"
	line is present in the caller-supplied bufer.

	Note: 
	The following strings are considered critical to
	parsing and otherwise understanding the envelope format.
	These are:

		"From"
		"remote from"
		"forwarded by"

	Currently, we do not allow any whitespace after the initial
	"From" other than a SPACE character; and many other
	implementations only consider "From " to be a valid introducer.
	Producers of envelopes should consider always putting a
	SPACE character there rather than some other whitespace
	character.

	What is a proper representative (example) envelope format?

	> From rightcore.com!dam Wed Dec 8 11:44:30 EDT 1993 -0400

	What are the optional parts?

	From rightcore.com!dam Wed Dec 8 11:44[:30] [EDT] [[19]93] [-0400]

	Name:
	mailmsgmatenv

	Description:
	Check it the given test string is the start of a mail message.

	Synopsis:
	int mailmsgmatenv(mailmsgmatenv *mep,char *mbuf,int mlen) noex

	Arguments:
	mep		pointer to message environment information
	mbuf		buffer holding data to be checked
	mlen		length of buffer to check

	Returns:
	>0		match and this is the length of the address-part
	==0		no match
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<tmstrs.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"mailmsgmatenv.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mmenvdat_ema(mmenvdat *,cchar *,int) noex ;
static int	mmenvdat_date(mmenvdat *,cchar *,int) noex ;
static int	mmenvdat_datefin(mmenvdat *,cchar *,int) noex ;
static int	mmenvdat_remote(mmenvdat *,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	exts[] = {
	"remote from",
	"forwarded by",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int mailmsgmatenv(mmenvdat *mep,cchar *sp,int sl) noex {
    	MMENVDAT	*hop = mep ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mep && sp) {
	    bool	f_start = true ;
	    rs = memclear(hop) ;		/* dangerous */
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl && iseol(sp[sl-1])) sl -= 1 ;
	    if ((sl > 0) && (*sp == '>')) {
	        sp += 1 ;
	        sl -= 1 ;
	        f_start = false ;
	    }
	    if ((sl > 5) && (sp[0] == 'F')) {
	        int	si = 5 ;
	        if (strncmp("From ",sp,si) == 0) {
	            mep->rt = -1 ;
	            sp += si ;
	            sl -= si ;
		    if ((si = mmenvdat_ema(mep,sp,sl)) > 0) {
	                sp += si ;
	                sl -= si ;
		        if ((si = mmenvdat_date(mep,sp,sl)) > 0) {
	                    sp += si ;
	                    sl -= si ;
			    if ((si = mmenvdat_remote(mep,sp,sl)) >= 0) {
			        len = mep->a.el ;
			        mep->fl.start = f_start ;
			    }
		        }
		    }
	        }
	    } /* end if (hit) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailmsgmatenv) */


/* local subroutines */

static int mmenvdat_ema(mmenvdat *mep,cchar *sp,int sl) noex {
	int		skip = 0 ;
	cchar		*cp ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    mep->a.ep = cp ;
	    mep->a.el = cl ;
	    skip = intconv((cp + cl) - sp) ;
	}
	return skip ;
}
/* end subroutine (mmenvdat_ema) */

/* > From rightcore.com!dam Wed Dec 8 11:44:30 EDT 1993 -0400 */
static int mmenvdat_date(mmenvdat *mep,cchar *sp,int sl) noex {
	int		skip = 0 ;
	cchar		*cp ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    int		rl ;
	    cchar	*beginp = cp ;
	    cchar	*rp ;
	    if (tmstrsday(cp,cl) >= 0) {
		int	si = intconv((cp + cl) - sp) ;
		rl = (sl-si) ;
		rp = (sp+si) ;
	        mep->d.ep = beginp ;
		mep->d.el = intconv((sp + sl) - beginp) ;
	        if ((cl = sfnext(rp,rl,&cp)) > 0) {
		    if (tmstrsmonth(cp,cl) >= 0) {
		        si = intconv((cp + cl) - sp) ;
		        rp = (sp+si) ;
		        rl = (sl-si) ;
	        	if ((cl = sfnext(rp,rl,&cp)) > 0) {
			    if (hasalldig(cp,cl)) {
		                si = intconv((cp + cl) - sp) ;
		                rp = (sp + si) ;
		                rl = (sl-si) ;
				if ((si = sichr(rp,rl,':')) >= 0) {
			    	    skip = sl ;
		            	    rp += (si+1) ;
		            	    rl -= (si+1) ;
				    si = mmenvdat_datefin(mep,rp,rl) ;
				    if (si >= 0) {
					skip = intconv(rp + si - beginp) ;
					mep->d.el = intconv(rp + si - beginp) ;
				    }
				}
			    }
			}
		    }
		}
	    }
	} /* end if (sfnext) */
	return skip ;
}
/* end subroutine (mmenvdat_date) */

static int mmenvdat_datefin(mmenvdat *mep,cchar *rp,int rl) noex {
	int		i ; /* used afterwards */
	int		si = -1 ;
	for (i = 0 ; exts[i] != nullptr ; i += 1) {
	    if ((si = sisub(rp,rl,exts[i])) >= 0) break ;
	} /* end for */
	if (si >= 0) {
	    mep->rt = i ;
	}
	return si ;
}
/* end subroutine (mmenvdat_datefin) */

static int mmenvdat_remote(mmenvdat *mep,cchar *sp,int sl) noex {
	int		skip = 0 ;
	if ((sl > 0) && (mep->rt >= 0)) {
	    cint	el = lenstr(exts[mep->rt]) ;
	    int		cl ;
	    cchar	*cp ;
	    sp += el ;
	    sl -= el ;
	    if ((cl = sfnext(sp,sl,&cp)) > 0) {
		mep->r.ep = cp ;
		mep->r.el = cl ;
		skip = intconv((cp + cl) - sp) ;
	    }
	}
	return skip ;
}
/* end subroutine (mmenvdat_remote) */



/* mallcompactstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* memocy-allocate a de-quoted and compacted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

	= 2025-08-01, David A­D­ Morano
	This code was located in with the EMA (E-Mail-Address)
	object, but it was not being used there (determined after
	some investigation).  So I grabbed it and moved it to here
	(currently LIBDAM).  I am pretty sure that no current code
	in any of the present active repositories is using (calling
	into) this code, but there could be some code in the in-active
	("obsolete" repository or others) that could (I guess) be
	using this.  So I am not deleting it as yet.  Actually, it
	would be moved to either "abeyance" or the "obsolete"
	repository if I wanted to delete it, in order to give someone
	some time (like possibly a few years) to see if they really
	used this code or not.  Also, there originally were two
	implementations of the actual main body of this code (the
	remaining implementation being inside the |mallcompactstr_reg|
	subroutine below).  But I removed the alternative implementation
	since it was not being used, and has not been used in
	who-knows-how-many-decades now.  The starting date for this
	code above says 1998, but most all of that code really dates
	from possibly as early as 1992 or so when I picked up
	maintenance on the AT&T Bell Laboratories PCS software
	distribution and started the process of cleanint it up.  So
	I actually do not know how old this code really is.  I also,
	swapped out |strlen(3c)| for |lenstr(3u)| (which required
	introducing a C++20 module into the mix).

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mallcompactstr

	Description:
	This subroutine allocates (in regular heap memory) a de-quoted
	(naively) and compacted c-string.  This has the identical
	call signature of |uc_mallocstrw(3uc)| but perform the
	dequoting and compaction in addition.  The "compaction"
	means no white-space at all, as compared with meaning
	no-redundant white-space.  This kind of thing is suitable
	for EMAIL addresses (not considering the comments) or EMAIL
	route addresses, where white-space is not significant.

	Synopsis:
	int mallcompactstr(cchar *sp,int sl,char **rpp) noex

	Arguments:
	sp		source c-string pointer
	sl		source c-string length
	rpp		pointer to result pointer

	Returns:
	>=0		number of characters in resulting store string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mallcompactstr.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int getlen(cchar *sp,int sl) noex {
        int             rs = SR_FAULT ;
        if (sp) {
            if (sl < 0) sl = lenstr(sp) ;
        }
        return (rs >= 0) ? sl : rs ;
} /* end subroutine (getlen) */

local int mallcompactstr_reg(cchar *,int,char **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mallcompactstr(cchar *sp,int sl,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (sp && rpp) {
	    if ((rs = getlen(sp,sl)) >= 0) {
		rs = mallcompactstr_reg(sp,sl,rpp) ;
		len = rs ;
	    }  /* end if (getlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mallcompactstr) */


/* local subroutines */

local int mallcompactstr_reg(cchar *sp,int sl,char **rpp) noex {
        cint		sz = (sl + 1) ;
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
        bool    	f_quote = false ;
        if (char *buf ; (rs = uc_malloc(sz,&buf)) >= 0) {
            char        *bp = buf ;
            while (sl > 0) {
                switch (cint ch = mkchar(*sp) ; ch) {
                case CH_DQUOTE:
                    f_quote = (! f_quote) ;
                     fallthrough ;
                    /* FALLTHROUGH */
                default:
                    if (f_quote || (! CHAR_ISWHITE(ch))) {
                        *bp++ = char(ch) ;
                    }
                    break ;
                } /* end switch */
                sp += 1 ;
                sl -= 1 ;
            } /* end while */
            *bp = '\0' ;
            len = intconv(bp - buf) ;
            *rpp = (rs >= 0) ? buf : nullptr ;
        } /* end if (memory-allocation) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mallcompactstr_reg) */



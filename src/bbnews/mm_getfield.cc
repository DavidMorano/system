/* mm_getfield SUPPORT */
/* lang=C++20 */

/* get the value of the specified header within specified message */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These are subroutines to aid in handling parts of a message.

	IMPORTANT NOTE:
	This subroutine is NOT RFC-822 compliant!

	This routine searches a mail message (MM) for a given header
	and returns the value of the header to the caller.  If the
	header appears in the message more than once, all found
	header value strings are returned separated by a comma.

	Arguments:
		mfp	Basic I-O file pointer to mail message file
		offset	byte offset from the beginning of the mail message
			file to the start of the message.
		mlen	message length (length of message itself in file)
		h	header string to search for
		fvalue	user specified buffer to hold the resulting header
			value (if found)
		buflen	user specifled length of supplied buffer
	

	return 'length-of-field'
		if header was found ('fvalue' is the returned field)

	return 'BAD' 
		if that header is not found in the message

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<baops.h>
#include	<char.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* external subroutines */

extern int	hmatch(cchar *,cchar *) ;


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mm_getfield(mfp,moff,mlen,h,fvalue,buflen)
bfile		*mfp ;
off_t	moff ;
int		mlen ;
const char	h[] ;
char		fvalue[] ;
int		buflen ;
{
	int		rs ;
	int		flen = 0 ;
	int		f_bol, f_eol ;
	int		f_lookmore = false ;
	int		f_boh = false ;

	if ((rs = bseek(mfp,moff,SEEK_SET)) >= 0) {
	    const int	llen = LINEBUFLEN ;
	    int		i, l, ml, len ;
	    cchar	*cp ;
	    char	lbuf[LINEBUFLEN + 1] ;

	buflen -= 3 ;
	fvalue[0] = '\0' ;
	len = 0 ;
	f_bol = true ;
	while ((len < mlen) && (flen < buflen) &&
	    ((l = breadln(mfp,lbuf,llen)) > 0)) {

	    len += l ;
	    if (lbuf[0] == '\n') 
		break ;

	    f_eol = false ;
	    if (lbuf[l - 1] == '\n')
	        ((f_eol = true), (l -= 1)) ;

	    lbuf[l] = '\0' ;

	    ml = 0 ;
	    if (f_bol) {

	        if (f_lookmore && CHAR_ISWHITE(lbuf[0])) {

	            cp = lbuf ;
	            while (CHAR_ISWHITE(*cp)) 
			cp += 1 ;

	            ml = MIN((lbuf + l - cp),(buflen - flen)) ;

	        } else if ((i = hmatch(h,lbuf)) > 0) {

	            f_lookmore = true ;
	            f_boh = true ;
	            cp = lbuf + i ;
	            while (CHAR_ISWHITE(*cp)) 
			cp += 1 ;

	            ml = MIN((lbuf + l - cp),(buflen - flen)) ;

	        } else {

	            f_lookmore = false ;
	            f_boh = false ;
	        }

	    } else if (f_lookmore) {

	        cp = lbuf ;
	        ml = MIN(l,(buflen - flen)) ;

	    }

	    if (ml > 0) {

	        if (flen > 0) {

	            if (f_boh) 
			fvalue[flen++] = ',' ;

	            fvalue[flen++] = ' ' ;
	        }

	        f_boh = false ;
	        strncpy(fvalue + flen,cp,ml) ;

	        flen += ml ;
	        fvalue[flen] = '\0' ;

	    } /* end if */

	    f_bol = f_eol ;
	} /* end while */

	fvalue[flen] = '\0' ;
	} /* end if (bseek) */

	return (flen > 0) ? flen : rs ;
}
/* end subroutine (mm_getfield) */



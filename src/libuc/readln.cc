/* readln SUPPORT */
/* lang=C++20 */

/* read characters from Standard-Input (STDIN) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	readln

	Description:
	This subroutine tries to mimic the old normal 'read' function
	from the old days.  The "modern" |istread::getline(3c++)|
	and |std::getline(3c++)| want to be in the middle of a
	sequence of input operations (like reading various typed
	fields from the input file).  So they return something like
	the type |istream &| so that they can be chained together.
	But I want to have the older style semantic where either
	the number of characters read is returned or an |errno(3)|
	error code is returned.

	Synopsis:
	int readln(istream &cin,char *ibuf,int ilen,int chdelim = eol) noex

	Arguments:
	cin		input |istream(3c++)| to read from
	ibuf		input character buffer to store read characteds
	ilen		input character buffer length
	chdelim		optional delimiter character

	Returns:
	>=0		number characters read, including terminating char
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string>
#include	<istream>
#include	<usystem.h>
#include	<storebuf.h>		/* <- not used! */
#include	<sncpyx.h>		/* <- not used! */
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"readln.hh"


/* local defines */


/* local namespaces */

using std::istream ;			/* type */
using std::string ;			/* type */
using std::getline ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int readln(istream &is,char *ibuf,int ilen,int dch) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dch == 0) dch = eol ;
	if (ibuf) {
	    try {
	        string	iline ;
		rs = SR_BADFMT ;
	        if (bool(getline(is,iline,char(dch)))) {
		    cint	ll = iline.length() ;
		    cchar	*lp = iline.c_str() ;
		    if ((rs = snwcpy(ibuf,ilen,lp,ll)) >= 0) {
			char	*ip = (ibuf+rs) ;
			cint	il = (ilen-rs) ;
			len += rs ;
			if (il > 0) {
			    *ip++ = dch ;
			    *ip = '\0' ;
			    len += 1 ;
			} else {
			    rs = SR_OVERFLOW ;
			}
		    } /* end if (copy to destination) */
		} else {
		    cbool	feof	= is.eof() ;
		    cbool	ffail	= is.fail() ;
		    cbool	fbad	= is.bad() ;
		    if (feof) {
		        rs = SR_OK ;
		    } else if (ffail) {
		        rs = SR_IO ;
		    } else if (fbad) {
		        rs = SR_IO ;
	            }
	        } /* end block */
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (readln) */



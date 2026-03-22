/* dprintf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dprintline

	Descrition:
	Print out to debugging a line of text that will be limited in
	length.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>		/* |FILE| + |stderr| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>		/* |rmeol(3u)| */
#include	<strnul.hh>
#include	<localmisc.h>

#include	"dprintf.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */


using libu::rmeol ;			/* subroutine */


static FILE	*efp = stderr ;


int dprintline(cc *funcname,cc *inbuf,int len) noex {
	cint	rl = rmeol(inbuf,len) ;
	int	retlen = 0 ; /* return-value */
	{
	    cint pl = lenstrline(inbuf,rl) ;
	    {
		strnul ps(inbuf,pl) ;
		retlen = pl ;
		fprintf(efp,"%s: ",funcname) ;
		fprintf(efp,"len=%d inbuf=>%s<\n",len,ccp(ps)) ;
	    }
	}
	return retlen ;
} /* end subroutine (dprintline) */



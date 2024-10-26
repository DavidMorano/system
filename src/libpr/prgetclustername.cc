/* prgetclustername SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prgetclustername

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int prgetclustername(cc *pr,char *rbuf,int rlen,cc *nn) noex

	Arguments:
	pr		program root
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	nn		nodename used to find associated cluster

	Returns:
	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error

	Design note:
	If there is no entry in the NODE DB file for the given
	nodename, then we proceed on to lookup the nodename in the
	CLUSTER DB. We use the CLUSTERDB object to interface with
	the CLUSTER DB itself. Generally, results (key-value pairs)
	are returned in a random order. If the idea was to return
	the => first <= cluster with the given node as a member,
	this will not always give predictable results. This is just
	something to keep in mind, and another reason to have an
	entry for the given node in the NODE DB if deterministic
	results need to be returned for a cluster name lookup by
	nodename.

	Q. Is this (mess) multi-thread safe?
	A. Duh!

	Q. Does this need to be so complicated?
	A. Yes.

	Q. Was the amount of complication supported by the need?
	A. Looking at it now ... maybe not.

	Q. Were there any alternatives?
	A. Yes; the predecessor to this present implementation was an 
	   implementation that was quite simple, but it had a lot
	   of static memory storage.  It was the desire to eliminate
	   the static memory storage that led to this present
	   implementation.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks like I have already done more to this 
	   simple function than may have been ever warranted to begin with!

	Q. Did this subroutine have to be Asyc-Signal-Safe?
	A. Not really.

	Q. Then why did you do it?
	A. The system-call |uname(2)| is Async-Signal-Safe.  Since this
	   subroutine (|prgetclustername(3dam)|) sort of looks like
	   |uname(2)| (of a sort), I thought it was a good idea.

	Q. Was it really a good idea?
	A. I guess not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getclustername.h>
#include	<nodedb.h>
#include	<clusterdb.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<ids.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"prgetclustername.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int prgetclustername(cchar *pr,char *rbuf,int rlen,cchar *nn) noex {
    	int		rs = SR_FAULT ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	        rs = libuc::prgetclustername(pr,rbuf,rlen,nn) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (prgetclustername) */



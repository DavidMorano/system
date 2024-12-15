/* snxtilook SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make string version of the XTI |t_look(3nsl)| codes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snxtilook

	Description:
	Ths subroutine creates in the result string a symbolic
	representation of the XTI |t_look(3nsl)| return codes.

	Synopsis:
	int snxtilook(char *dbuf,int dlen,int flags) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	flags		open-flags

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<xti.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sncpy.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct val {
	int		v ;		/* value */
	cchar		*n ;		/* name */
	cchar		*msg ;		/* message */
} ;


/* forward references */

static int	findent(int) noex ;


/* local variables */

constexpr val	vals[] = {
	{ T_LISTEN, "LISTEN", "connection indication received" },
	{ T_CONNECT, "CONNECT", "connect confirmation received" },
	{ T_DATA, "DATA", "normal data received" },
	{ T_EXDATA, "EXDATA", "expedited data received" },
	{ T_DISCONNECT, "DISCONNECT", "disconnect received" },
	{ T_UDERR, "UDERR", "data gram error indication" },
	{ T_ORDREL, "ORDREL", "orderly release indication" },
	{ T_GODATA, "GODATA", "sending normal data is again possible" },
	{ T_GOEXDATA, "GOEXDATA", "sending expedited data is again possible" },
	{ -1, NULL }
} ; /* end struct (val) */


/* exported variables */


/* exported subroutines */

int snxtilook(char *dbuf,int dlen,int v) noex {
	cint		i = findent(v) ;
	int		rs = SR_FAULT ;
	if (dbuf) {
	    ccha	*n = (i >= 0) ? vals[i].n : "UNKNOWN" ;
	    rs = sncpy1(dbuf,dlen,n) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snxtilook) */


/* local subroutines */

static int findent(int v) noex {
	int		i ; /* <- used afterwards */
	bool		f = false ;
	for (i = 0 ; vals[i].v >= 0 ; i += 1) {
	    f = (v == vals[i].v) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (findent) */



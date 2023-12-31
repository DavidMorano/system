/* snxtilook */
/* lang=C20 */

/* make string version of the XTI |t_look(3nsl)| codes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A-D- Morano.  All rights reserved. */

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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<xti.h>
#include	<string.h>
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


/* foward references */

static int	findent(int) noex ;


/* local variables */

static const struct val	vals[] = {
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
} ;


/* exported subroutines */

int snxtilook(char *dbuf,int dlen,int v) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    int		i = findent(v) ;
	    cchar	*n ;
	    n = (i >= 0) ? vals[i].n : "UNKNOWN" ;
	    rs = sncpy1(dbuf,dlen,n) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snxtilook) */


/* local subroutines */

static int findent(int v) noex {
	int		i ;
	bool		f = false ;
	for (i = 0 ; vals[i].v >= 0 ; i += 1) {
	    f = (v == vals[i].v) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (findent) */



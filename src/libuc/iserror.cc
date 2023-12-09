/* iserror */
/* lang=C++20 */

/* determine if a return-status value is some kind of error */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	isFailOpen
	isFailConn
	isBadSend
	isBadRecv
	isBadMsg
	isIOError

	Description:
	This subroutine determines if an operation resulted in a
	bad message.

	Synopsis:
	extern int isFailOpen(int rs) noex
	extern int isFailConn( rs) noex 
	extern int isBadSend(int rs) noex 
	extern int isBadRecv(int rs) noex 
	extern int isBadMsg(int rs) noex 
	extern int isIOError(int rs) noex

	Arguments:
	rs		value to check

	Returns:
	>0		present (condition found)
	==0		no (condition not present)
	<0		should not happen

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"isoneof.h"
#include	"isnot.h"
#include	"iserror.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr int	rfailopen[] = {
	SR_BADF,
	0	
} ;

static constexpr int	rfailconn[] = {
	SR_NETDOWN,
	SR_NETUNREACH,
	SR_HOSTDOWN,
	SR_HOSTUNREACH,
	SR_CONNREFUSED,
	SR_NOTCONN,
	SR_CONNRESET,
	SR_NOENT,
	SR_PIPE,
	SR_TIMEDOUT,
	0	
} ;

static constexpr int	rbadsend[] = {
	SR_DESTADDRREQ,
	SR_NOENT,
	SR_CONNRESET,
	SR_NOTCONN,
	SR_PIPE,
	SR_NOTSOCK,
	0	
} ;

static constexpr int	rbadrecv[] = {
	SR_INVALID,
	SR_TIMEDOUT,
	0	
} ;

static constexpr int	rbadmsg[] = {
	SR_BADMSG,
	SR_DOM,
	SR_RANGE,
	0	
} ;

static constexpr int	rioerror[] = {
	SR_IO,
	SR_NXIO,
	SR_PIPE,
	0	
} ;


/* exported subroutines */

int isFailOpen(int rs) noex {
	int		f = false ;
	if (rs < 0) {
	    f = f || isNotPresent(rs) ;
	    f = f || isOneOf(rfailopen,rs) ;
	}
	return f ;
}
/* end subroutine (isFailOpen) */

int isFailConn(int rs) noex {
	int		f = false ;
	if (rs < 0) {
	    f = isOneOf(rfailconn,rs) ;
	}
	return f ;
}
/* end subroutine (isFailConn) */

int isBadSend(int rs) noex {
	return isOneOf(rbadsend,rs) ;
}
/* end subroutine (isBadSend) */

int isBadRecv(int rs) noex {
	return isOneOf(rbadrecv,rs) ;
}
/* end subroutine (isBadRecv) */

int isBadMsg(int rs) noex {
	return isOneOf(rbadmsg,rs) ;
}
/* end subroutine (isBadMsg) */

int isIOError(int rs) noex {
	return isOneOf(rioerror,rs) ;
}
/* end subroutine (isIOError) */



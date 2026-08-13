/* iserror SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if a return-status value is some kind of error */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

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

	Aliases:
	isFailIO

	Description:
	These subroutines determine if an error code (a system
	"return-status") is a part of a certain category of errors.

	Synopsis:
	extern int isFailOpen	(int rs) noex
	extern int isFailConn	(int rs) noex 
	extern int isBadSend	(int rs) noex 
	extern int isBadRecv	(int rs) noex 
	extern int isBadMsg	(int rs) noex 
	extern int isIOError	(int rs) noex

	Arguments:
	rs		value to check

	Returns:
	-		TRUE or FALSE

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU <- the money shot! */
#include	<localmisc.h>		/* LIBU */

#include	"isoneof.h"
#include	"isnot.h"
#include	"iserror.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rfailopen[] = {
	SR_BADF,
	SR_ILSEQ,
	SR_NOTDIR,
	SR_TXTBSY,
	SR_ROFS,
	SR_OVERFLOW,
	SR_OPNOTSUPP,
	SR_NXIO,
	SR_NOSPC,
	SR_LOOP,
	SR_NFILE,
	SR_NAMETOOLONG,
	SR_ISDIR,
	SR_IO,
	SR_INVALID,
	SR_FAULT,
	SR_EXIST,
	SR_DQUOT,
	0	
} ; /* end array */

constexpr int		rfailconn[] = {
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
} ; /* end array */

constexpr int		rbadsend[] = {
	SR_DESTADDRREQ,
	SR_NOENT,
	SR_CONNRESET,
	SR_NOTCONN,
	SR_PIPE,
	SR_NOTSOCK,
	0	
} ; /* end array */

constexpr int		rbadrecv[] = {
	SR_INVALID,
	SR_TIMEDOUT,
	0	
} ; /* end array */

constexpr int		rbadmsg[] = {
	SR_BADMSG,
	SR_DOM,
	SR_RANGE,
	0	
} ; /* end array */

constexpr int		rioerror[] = {
	SR_IO,
	SR_NXIO,
	SR_PIPE,
	0	
} ; /* end array */


/* exported variables */


/* exported subroutines */

bool isFailOpen(int rs) noex {
	int		f = false ;
	if (rs < 0) {
	    f = f || isNotPresent(rs) ;
	    f = f || isonebad(rfailopen,rs) ;
	}
	return f ;
} /* end subroutine (isFailOpen) */

bool isFailConn(int rs) noex {
	return isonebad(rfailconn,rs) ;
} /* end subroutine (isFailConn) */

bool isBadSend(int rs) noex {
	return isonebad(rbadsend,rs) ;
} /* end subroutine (isBadSend) */

bool isBadRecv(int rs) noex {
	return isonebad(rbadrecv,rs) ;
} /* end subroutine (isBadRecv) */

bool isBadMsg(int rs) noex {
	return isonebad(rbadmsg,rs) ;
} /* end subroutine (isBadMsg) */

bool isIOError(int rs) noex {
	return isonebad(rioerror,rs) ;
} /* end subroutine (isIOError) */


/* local subroutines */



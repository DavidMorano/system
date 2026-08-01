/* openportmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutine to open a priviledged network port */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENPORTMSG_INCLUDE
#define	OPENPORTMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sockaddress.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |USERNAMELEN| */


#define	OPENPORTMSG_REQ		struct openportmsg_request
#define	OPENPORTMSG_RES		struct openportmsg_response
#define	OPENPORTMSG_UNLEN	USERNAMELEN


struct openportmsg_request {
	sockaddress	sa ;
	int		pf ;		/* suitable for |socket(3xnet)| */
	int		ptype ;		/* suitable for |socket(3xnet)| */
	int		proto ;		/* suitable for |socket(3xnet)| */
	uint		msglen ;
	uchar		msgtype ;
	char		username[OPENPORTMSG_UNLEN + 1] ;
} ; /* end struct */

struct openportmsg_response {
	int		rs ;
	uint		msglen ;
	uchar		msgtype ;
} ; /* end struct */

enum openportmsgtypes {
	openportmsgtype_request,
	openportmsgtype_response,
	openportmsgtype_overlast
} ; /* end struct */

typedef	OPENPORTMSG_REQ		openportmsg_req ;
typedef	OPENPORTMSG_RES		openportmsg_res ;

EXTERNC_begin

extern int openportmsg_msgrequest(openportmsg_req *,int,char *,int) noex ;
extern int openportmsg_msgresponse(openportmsg_res *,int,char *,int) noex ;

EXTERNC_end


#endif /* OPENPORTMSG_INCLUDE */



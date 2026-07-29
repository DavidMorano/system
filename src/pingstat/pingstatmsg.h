/* pingstatmsg HEADER */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* create and parse PINGSTAT (internal) messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	PINGSTATMSG_INCLUDE
#define	PINGSTATMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


struct pingstatmsg_update {
	uint	msglen ;
	uint	timestamp ;
	short	hostnamelen ;
	uchar	msgtype ;		/* message type */
	char	hostname[MAXHOSTNAMELEN + 1] ;
} ; /* end struct */

struct pingstatmsg_uptime {
	uint	msglen ;
	uint	timestamp ;
	uint	timechange ;
	uint	count ;
	short	hostnamelen ;
	uchar	msgtype ;		/* message type */
	char	hostname[MAXHOSTNAMELEN + 1] ;
} ; /* end struct */

/* unknown */
struct pingstatmsg_unknown {
	uint	msglen ;
	uchar	msgtype ;		/* message type */
} ; /* end struct */

/* request types */
enum pingstatmsgtypes {
	pingstatmsgtype_update,
	pingstatmsgtype_uptime,
	pingstatmsgtype_unknown,
	pingstatmsgtype_overlast
} ; /* end enum */

/* response codes */
enum pingstatmsgrcs {
	pingstatmsgrc_ok,
	pingstatmsgrc_invalid,
	pingstatmsgrc_notavail,
	pingstatmsgrc_done,
	pingstatmsgrc_goingdown,
	pingstatmsgrc_overlast
} ; /* end enum */

/* message sizes */
#define	PINGSTATMSG_SUPDATE		sizeof(struct pingstatmsg_update) ;
#define	PINGSTATMSG_SUPTIME		sizeof(struct pingstatmsg_uptime) ;

EXTERNC_begin

extern int pingstatmsg_update(struct pingstatmsg_update *,int,char *,int) ;
extern int pingstatmsg_uptime(struct pingstatmsg_uptime *,int,char *,int) ;
extern int pingstatmsg_unknown(struct pingstatmsg_unknown *,int,char *,int) ;

EXTERNC_end


#endif /* PINGSTATMSG_INCLUDE */



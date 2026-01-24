/* sesmsg SIPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	SESMSG_INCLUDE
#define	SESMSG_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<limits.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>		/* constants */


/* service name */
#ifndef	SVCNAMELEN
#define	SVCNAMELEN		32
#endif
/* message objects */
#define	SESMSG_EXIT		struct sesmsg_exit
#define	SESMSG_NOOP		struct sesmsg_noop
#define	SESMSG_MBUF		struct sesmsg_mbuf
#define	SESMSG_GEN		struct sesmsg_gen
#define	SESMSG_BIFF		struct sesmsg_biff
#define	SESMSG_ECHO		struct sesmsg_echo
#define	SESMSG_RESPONSE		struct sesmsg_response
#define	SESMSG_PASSFD		struct sesmsg_passfd
#define	SESMSG_GETSYSMISC	struct sesmsg_getsysmisc
#define	SESMSG_SYSMISC		struct sesmsg_sysmisc
#define	SESMSG_GETLOADAVE	struct sesmsg_getloadave
#define	SESMSG_LOADAVE		struct sesmsg_loadave
#define	SESMSG_REPLOADAVE	struct sesmsg_reploadave
#define	SESMSG_GETLISTENER	struct sesmsg_getlistener
#define	SESMSG_LISTENER		struct sesmsg_listener
#define	SESMSG_GETHELP		struct sesmsg_gethelp
#define	SESMSG_HELP		struct sesmsg_help
#define	SESMSG_CMD		struct sesmsg_cmd
#define	SESMSG_MARK		struct sesmsg_mark
#define	SESMSG_UNKNOWN		struct sesmsg_unknown
/* lengths or sizes */
#define	SESMSG_USERLEN		USERNAMELEN
#define	SESMSG_NBUFLEN		MAXNAMELEN
#define	SESMSG_REASONLEN	100
#define	SESMSG_SVCLEN		MAX(SVCNAMELEN,32)
#define	SESMSG_CMDLEN		MAX(SVCNAMELEN,32)
#define	SESMSG_LNAMELEN		MAXNAMELEN
#define	SESMSG_LADDRLEN		(MAXPATHLEN + 20)


struct sesmsg_exit {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	reason[SESMSG_REASONLEN + 1] ;
} ; /* end struct */

struct sesmsg_noop {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
} ; /* end struct */

struct sesmsg_mbuf {
	time_t	stime ;
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	user[SESMSG_USERLEN+1] ;
	char	nbuf[SESMSG_NBUFLEN+ 1] ;
	uchar	rc ;
} ; /* end struct */

struct sesmsg_gen {
	time_t	stime ;
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	user[SESMSG_USERLEN+1] ;
	char	nbuf[SESMSG_NBUFLEN+ 1] ;
	uchar	rc ;
} ; /* end struct */

struct sesmsg_biff {
	time_t	stime ;
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	user[SESMSG_USERLEN+1] ;
	char	nbuf[SESMSG_NBUFLEN+ 1] ;
	uchar	rc ;
} ; /* end struct */

struct sesmsg_echo {
	time_t	stime ;
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	user[SESMSG_USERLEN+1] ;
	char	nbuf[SESMSG_NBUFLEN+ 1] ;
	uchar	rc ;
} ; /* end struct */

struct sesmsg_response {
	uint	msglen ;
	uint	tag ;
	uint	pid ;
	uchar	msgtype ;
	uchar	rc ;
} ; /* end struct */

struct sesmsg_passfd {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	char	svc[SESMSG_SVCLEN + 1] ;
	char	user[SESMSG_USERLEN+1] ;
} ; /* end struct */

/* request to parent server */
struct sesmsg_getsysmisc {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
} ; /* end struct */

/* response to sub-server */
struct sesmsg_sysmisc {
	uint	msglen ;
	uint	tag ;
	uint	la_1min ;
	uint	la_5min ;
	uint	la_15min ;
	uint	boottime ;
	uint	nproc ;
	uchar	rc ;
	uchar	msgtype ;
} ; /* end struct */

struct sesmsg_getloadave {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;		/* message type */
} ; /* end struct */

struct sesmsg_loadave {
	uint	msglen ;
	uint	tag ;
	uint	la_1min ;
	uint	la_5min ;
	uint	la_15min ;
	uchar	msgtype ;		/* message type */
	uchar	rc ;
} ; /* end struct */

struct sesmsg_reploadave {
	uint	msglen ;
	uint	tag ;			/* our tag (unused) */
	uint	utag ;			/* user's tag */
	uint	duration ;		/* turns on repetition (secs) */
	uint	interval ;		/* repetition interval (secs) */
	ushort	addrfamily ;		/* used for UDP response */
	ushort	addrport ;		/* used for UDP response */
	uint	addrhost[4] ;		/* used for UDP response */
	uchar	msgtype ;		/* message type */
} ; /* end struct */

/* request listener information */
struct sesmsg_getlistener {
	uint	msglen ;
	uint	tag ;
	uint	idx ;			/* listener index */
	uchar	msgtype ;		/* message type */
} ; /* end struct */

/* listener information (response) */
struct sesmsg_listener {
	uint	msglen ;
	uint	tag ;
	uint	idx ;			/* listener index */
	uint	pid ;
	uchar	msgtype ;		/* message type */
	uchar	rc ;
	uchar	ls ;			/* listener status */
	char	name[SESMSG_LNAMELEN + 1] ;
	char	addr[SESMSG_LADDRLEN + 1] ;
} ; /* end struct */

struct sesmsg_mark {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;		/* message type */
} ;

struct sesmsg_unknown {
	uint	msglen ;
	uchar	msgtype ;
} ; /* end struct */

/* request help information */
struct sesmsg_gethelp {
	uint	msglen ;
	uint	tag ;
	uint	idx ;			/* help index */
	uchar	msgtype ;		/* message type */
} ; /* end struct */

/* help information (response) */
struct sesmsg_help {
	uint	msglen ;
	uint	tag ;
	uint	idx ;			/* help index */
	uint	pid ;
	uchar	msgtype ;		/* message type */
	uchar	rc ;
	char	name[SESMSG_LNAMELEN + 1] ;
} ; /* end struct */

struct sesmsg_cmd {
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;		/* message type */
	char	cmd[SESMSG_CMDLEN + 1] ;
} ; /* end struct */

/* message types */
enum sesmsgtypes {
	sesmsgtype_exit,		/* by convention should be first */
	sesmsgtype_noop,
	sesmsgtype_gen,
	sesmsgtype_biff,
	sesmsgtype_echo,
	sesmsgtype_response,
	sesmsgtype_passfd,
	sesmsgtype_getsysmisc,
	sesmsgtype_sysmisc,
	sesmsgtype_getloadave,
	sesmsgtype_loadave,
	sesmsgtype_reploadave,
	sesmsgtype_getlistener,
	sesmsgtype_listener,
	sesmsgtype_mark,
	sesmsgtype_unknown,
	sesmsgtype_gethelp,
	sesmsgtype_help,
	sesmsgtype_cmd,	
	sesmsgtype_invalid,
	sesmsgtype_overlast
} ; /* end enum */

/* response codes */
enum sesmsgrcs {
	sesmsgrc_ok,
	sesmsgrc_invalid,
	sesmsgrc_nofd,
	sesmsgrc_notavail,
	sesmsgrc_error,
	sesmsgrc_overflow,
	sesmsgrc_overlast
} ; /* end enum */

EXTERNC_begin

extern int sesmsger_exit	(sesmsg_exit *,int,char *,int) noex ;
extern int sesmsger_noop	(sesmsg_noop *,int,char *,int) noex ;
extern int sesmsger_gen		(sesmsg_gen *,int,char *,int) noex ;
extern int sesmsger_biff	(sesmsg_biff *,int,char *,int) noex ;
extern int sesmsger_echo	(sesmsg_echo *,int,char *,int) noex ;
extern int sesmsger_response	(sesmsg_response *,int,char *,int) noex ;
extern int sesmsger_passfd	(sesmsg_passfd *,int,char *,int) noex ;
extern int sesmsger_getsysmisc	(sesmsg_getsysmisc *,int,char *,int) noex ;
extern int sesmsger_sysmisc	(sesmsg_sysmisc *,int,char *,int) noex ;
extern int sesmsger_getloadave	(sesmsg_getloadave *,int,char *,int) noex ;
extern int sesmsger_sysmisc	(sesmsg_sysmisc *,int,char *,int) noex ;
extern int sesmsger_reploadave	(sesmsg_reploadave *,int,char *,int) noex ;
extern int sesmsger_loadave	(sesmsg_loadave *,int,char *,int) noex ;
extern int sesmsger_getlistener	(sesmsg_getlistener *,int,char *,int) noex ;
extern int sesmsger_listener	(sesmsg_listener *,int,char *,int) noex ;
extern int sesmsger_mark	(sesmsg_mark *,int,char *,int) noex ;
extern int sesmsger_unknown	(sesmsg_unknown *,int,char *,int) noex ;
extern int sesmsger_gethelp	(sesmsg_gethelp *,int,char *,int) noex ;
extern int sesmsger_help	(sesmsg_help *,int,char *,int) noex ;
extern int sesmsger_cmd		(sesmsg_cmd *,int,char *,int) noex ;

EXTERNC_end


#endif /* __cplusplus (C++ only) */
#endif /* SESMSG_INCLUDE */



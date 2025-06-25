/* mcmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* create and parse mail-cluster IPC messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MCMSG_INCLUDE
#define	MCMSG_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<usystem.h>
#include	<localmisc.h>		/* |LOGNAMELEN| + ... */


#ifndef	LOGNAMELEN
#define	LOGNAMELEN	32
#endif

#ifndef	NODENAMELEN
#define	NODENAMELEN	257		/* System V */
#endif

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#define	MCMSG_REQ		struct mcmsg_req
#define	MCMSG_REP		struct mcmsg_rep
#define	MCMSG_ACK		struct mcmsg_ack
#define	MCMSG_LMAILUSER		LOGNAMELEN
#define	MCMSG_LCLUSTER		NODENAMELEN
#define	MCMSG_LMSGID		MAILADDRLEN
#define	MCMSG_LFROM		MAILADDRLEN


enum mcmsgmems {
    mcmsgmem_start,
    mcmsgmem_finish,
    mcmsgmem_overlast
} ;

struct mcmsg_req ;
struct mcmsg_rep ;
struct mcmsg_ack ;

template <typename T>
struct mcmsg_co {
    	T		*op = nullptr ;
	int		w = -1 ;
	void operator () (T *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ;

extern int mcmsg_coreq(mcmsg_req *,int) noex ;
extern int mcmsg_corep(mcmsg_rep *,int) noex ;
extern int mcmsg_coack(mcmsg_ack *,int) noex ;

template<>
mcmsg_co<mcmsg_req>::operator int () noex {
    	return mcmsg_coreq(op,w) ;
}

template<>
mcmsg_co<mcmsg_rep>::operator int () noex {
    	return mcmsg_corep(op,w) ;
}

template<>
mcmsg_co<mcmsg_ack>::operator int () noex {
    	return mcmsg_coack(op,w) ;
}

/* client request message */
struct mcmsg_req {
    	mcmsg_co<mcmsg_req>	start ;
    	mcmsg_co<mcmsg_req>	finish ;
	uint	msglen ;
	uint	tag ;
	uint	timestamp ;
	char	cluster[MCMSG_LCLUSTER + 1] ;
	char	mailuser[MCMSG_LMAILUSER + 1] ;
	uchar	msgtype ;
	uchar	seq ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	mcmsg_req() noex {
	    start(this,mcmsgmem_start) ;
	    finish(this,mcmsgmem_finish) ;
	} ;
	void dtor() noex ;
	~mcmsg_req() {
	    dtor() ;
	} ;
} ;

/* server report */
struct mcmsg_rep {
    	mcmsg_co<mcmsg_rep>	start ;
    	mcmsg_co<mcmsg_rep>	finish ;
	uint	msglen ;
	uint	tag ;
	uint	timestamp ;
	uint	mlen ;
	char	cluster[MCMSG_LCLUSTER + 1] ;
	char	mailuser[MCMSG_LMAILUSER + 1] ;
	char	msgid[MCMSG_LMSGID + 1] ;
	char	from[MCMSG_LFROM + 1] ;
	uchar	flags ;
	uchar	msgtype ;
	uchar	seq ;
	uchar	rc ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	mcmsg_rep() noex {
	    start(this,mcmsgmem_start) ;
	    finish(this,mcmsgmem_finish) ;
	} ;
	void dtor() noex ;
	~mcmsg_rep() {
	    dtor() ;
	} ;
} ;

/* general acknowledgement */
struct mcmsg_ack {
    	mcmsg_co<mcmsg_ack>	start ;
    	mcmsg_co<mcmsg_ack>	finish ;
	uint	msglen ;
	uint	tag ;
	uchar	msgtype ;
	uchar	seq ;
	uchar	rc ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	mcmsg_ack() noex {
	    start(this,mcmsgmem_start) ;
	    finish(this,mcmsgmem_finish) ;
	} ;
	void dtor() noex ;
	~mcmsg_ack() {
	    dtor() ;
	} ;
} ;

/* request types */
enum mcmsgtypes {
	mcmsgtype_request,
	mcmsgtype_report,
	mcmsgtype_ack,
	mcmsgtype_overlast
} ;

/* response codes */
enum mcmsgrcs {
	mcmsgrc_ok,
	mcmsgrc_invalid,
	mcmsgrc_notavail,
	mcmsgrc_goingdown,
	mcmsgrc_overlast
} ;

EXTERNC_begin

extern int	mcmsg_request(mcmsg_req *,int,char *,int) noex ;
extern int	mcmsg_report(mcmsg_rep *,int,char *,int) noex ;
extern int	mcmsg_acknowledge(mcmsg_ack *,int,char *,int) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* MCMSG_INCLUDE */



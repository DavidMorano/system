/* mailbox INCLUDE */
/* lang=C20 */

/* mailbox handling object */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-10, David A­D­ Morano
        This is being written from scratch to finally get an abstracted
        "mailbox" that is fast enough for interactive use.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILBOX_INCLUDE
#define	MAILBOX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<localmisc.h>


/* object defines */

#define	MAILBOX_MAGIC		0x31415926
#define	MAILBOX			struct mailbox_head
#define	MAILBOX_INFO		struct mailbox_info
#define	MAILBOX_MSGINFO		struct mailbox_msg
#define	MAILBOX_READ		struct mailbox_read
#define	MAILBOX_FLAGS		struct mailbox_flags
#define	MAILBOX_MFLAGS		struct mailbox_msgflags

/* options */

#define	MAILBOX_ORDONLY		O_RDONLY	/* open read-only */
#define	MAILBOX_ORDWR		O_RDWR		/* open read-write */
#define	MAILBOX_ONOCLEN		O_NOCTTY	/* !use "content-length" */
#define	MAILBOX_OUSECLINES	O_NDELAY	/* use 'content-clines" */


struct mailbox_info {
	int		nmsgs ;			/* messages total */
	int		nclens ;		/* messages with CLENs */
} ;

struct mailbox_flags {
	uint		readlock:1 ;
	uint		writelock:1 ;
	uint		readonly:1 ;		/* opened read-only */
	uint		useclen:1 ;
	uint		useclines:1 ;
	uint		rewrite:1 ;
} ;

struct mailbox_head {
	uint		magic ;
	vecobj		msgs ;
	cchar	*mailfname ;
	MAILBOX_FLAGS	f ;
	time_t		ti_mod ;	/* modification time */
	time_t		ti_check ;	/* check time */
	int		pagesize ;
	int		mfd ;		/* mail file-descriptor */
	int		mblen ;		/* mailbox file length */
	int		msgs_total ;
	int		msgs_clen ;
	int		msgs_del ;
	int		to_lock ;	/* lock timeout */
	int		to_read ;	/* read timeout */
	int		mflags ;	/* open-flags */
} ;

struct mailbox_msgflags {
	uint		env:1 ;		/* had an environment marker */
	uint		senv:1 ;	/* starting environment marker */
	uint		hdradds:1 ;	/* HDRADDS initialized */
	uint		clen:1 ;	/* CLEN header */
	uint		clines:1 ;	/* CLINES header */
	uint		lines:1 ;	/* LINES header */
	uint		xlines:1 ;	/* XLINES header */
	uint		status:1 ;	/* unused: STATUS header */
	uint		sem:1 ;		/* unused: SEM header */
	uint		mlen:1 ;
	uint		blen:1 ;
	uint		msgread:1 ;	/* unused: message has been 'read' */
	uint		msgold:1 ;	/* unused: message is 'old' */
	uint		delete:1 ;	/* marked for deletion */
	uint		addany:1 ;	/* anything (something) being added */
} ;

struct mailbox_msg {
	vecstr		hdradds ;	/* HDRs to be added */
	MAILBOX_MFLAGS	hdr, hdrval, hdradd, f, cmd ;
	uint		hash[8] ;	/* ?? */
	offset_t	moff ;		/* offset to start of message */
	offset_t	hoff ;		/* offset to top of headers */
	offset_t	soff ;		/* offset to semaphore */
	offset_t	boff ;		/* offset to message body */
	int		mlen ;		/* total "message" length */
	int		hlen ;		/* total header-area length */
	int		blen ;		/* total "body" length */
	int		clen ;		/* content-length (HDR) */
	int		clines ;	/* content-lines (HDR) */
	int		lines ;		/* lines (?) */
	int		xlines ;	/* xlines (?) */
	int		msgi ;		/* message index */
} ;

struct mailbox_read {
	char		*rbuf ;
	char		*rbp ;
	int		rsize ;
	int		rlen ;
	offset_t	foff ;		/* file offset */
	offset_t	roff ;		/* read offset */
} ;

#ifdef	COMMENT

/* processed values */
struct mailbox_msgvalues {
	cchar	*efrom ;	/* envelope FROM */
	cchar	*edate ;	/* envelope DATE */
	cchar	*eremote ;	/* envelope REMOTE */
	cchar	*received ;	/* headers "received" */
	cchar	*subject ;	/* header "subject" */
	cchar	*ctype ;	/* header "content-type" */
	cchar	*from ;		/* header "from" */
	cchar	*date ;		/* header "date" */
	cchar	*to ;		/* header "to" */
	cchar	*cc ;		/* header "cc" */
	cchar	*status ;	/* header "status" */
	cchar	*mid ;		/* message ID "message-id" */
} ;

#endif /* COMMENT */

#ifdef	COMMENT
struct mailbox_headkey {
	int		dummy ;
} ;
#endif /* COMMENT */

#ifdef	__cplusplus
extern "C" {
#endif

extern int mailbox_open(MAILBOX *,cchar *,int) noex ;
extern int mailbox_info(MAILBOX *,MAILBOX_INFO *) noex ;
extern int mailbox_count(MAILBOX *) noex ;
extern int mailbox_mbfile(MAILBOX *,char *,int) noex ;
extern int mailbox_check(MAILBOX *,time_t) noex ;
extern int mailbox_msgoff(MAILBOX *,int,offset_t *) noex ;
extern int mailbox_msginfo(MAILBOX *,int,MAILBOX_MSGINFO *) noex ;
extern int mailbox_msgdel(MAILBOX *,int,int) noex ;
extern int mailbox_msghdradd(MAILBOX *,int,cchar *,cchar *,int) noex ;
extern int mailbox_countdel(MAILBOX *) noex ;
extern int mailbox_readbegin(MAILBOX *,MAILBOX_READ *,offset_t,int) noex ;
extern int mailbox_readline(MAILBOX *,MAILBOX_READ *,char *,int) noex ;
extern int mailbox_readend(MAILBOX *,MAILBOX_READ *) noex ;
extern int mailbox_close(MAILBOX *) noex ;

#ifdef	COMMENT
extern int mailbox_msgread(MAILBOX *,int,char *,int) noex ;
extern int mailbox_msgreadline(MAILBOX *,int,char *,int) noex ;
#endif

#ifdef	__cplusplus
}
#endif


#endif /* MAILBOX_INCLUDE */



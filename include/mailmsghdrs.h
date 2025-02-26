/* mailmsghdrs HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* put all of the mail-message header values of a message into an array */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGHDRS_INCLUDE
#define	MAILMSGHDRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mailmsg.h>


/* object defines */
#define	MAILMSGHDRS_MAGIC	0x87987598
#define	MAILMSGHDRS		struct mailmsghdrs_head

enum his {
	hi_from,
	hi_to,
	hi_date,
	hi_subject,
	hi_title,
	hi_messageid,
	hi_articleid,
	hi_clength,
	hi_newsgroups,
	hi_inreplyto,
	hi_board,
	hi_lines,
	hi_replyto,
	hi_references,
	hi_ctype,
	hi_expires,
	hi_keywords,
	hi_control,
	hi_xlines,
	hi_path,
	hi_errorsto,
	hi_returnpath,
	hi_received,
	hi_xqueuespec,
	hi_xservice,
	hi_xjobid,
	hi_xorighost,
	hi_xoriguser,
	hi_xusername,
	hi_sender,
	hi_cc,
	hi_bcc,
	hi_status,
	hi_clines,
	hi_cencoding,
	hi_organization,
	hi_deliveredto,
	hi_xoriginalto,
	hi_xpriority,
	hi_priority,
	hi_xface,
	hi_xbbnews,
	hi_xuuid,
	hi_xuti,
	hi_xmcd,
	hi_xmailer,
	hi_xforwardedto,
	hi_subj,
	hi_overlast
} ; /* end enum (his) */

#define	HI_FROM		hi_from
#define	HN_FROM		mailmsghdrs_names[HI_FROM]
#define	HL_FROM		4

#define	HI_TO		hi_to
#define	HN_TO		mailmsghdrs_names[HI_TO]
#define	HL_TO		2

#define	HI_DATE		hi_date
#define	HN_DATE		mailmsghdrs_names[HI_DATE]
#define	HL_DATE		4

#define	HI_SUBJECT	hi_subject
#define	HN_SUBJECT	mailmsghdrs_names[HI_SUBJECT]
#define	HL_SUBJECT	7

#define	HI_TITLE	hi_title
#define	HN_TITLE	mailmsghdrs_names[HI_TITLE]
#define	HL_TITLE	5

#define	HI_MESSAGEID	hi_messageid
#define	HN_MESSAGEID	mailmsghdrs_names[HI_MESSAGEID]
#define	HL_MESSAGEID	10

#define	HI_ARTICLEID	hi_articleid
#define	HN_ARTICLEID	mailmsghdrs_names[HI_ARTICLEID]
#define	HL_ARTICLEID	10

#define	HI_CLEN		hi_clen
#define	HN_CLEN		mailmsghdrs_names[HI_CLEN]
#define	HL_CLEN		14

#define	HI_NEWSGROUPS	hi_newsgroups
#define	HN_NEWSGROUPS	mailmsghdrs_names[HI_NEWSGROUPS]
#define	HL_NEWSGROUPS	10

#define	HI_INREPLYTO	hi_inreplyto
#define	HN_INREPLYTO	mailmsghdrs_names[HI_INREPLYTO]
#define	HL_INREPLYTO	11

#define	HI_BOARD	hi_board
#define	HN_BOARD	mailmsghdrs_names[HI_BOARD]
#define	HL_BOARD	5

#define	HI_LINES	hi_lines
#define	HN_LINES	mailmsghdrs_names[HI_LINES]
#define	HL_LINES	5

#define	HI_REPLYTO	hi_replyto
#define	HN_REPLYTO	mailmsghdrs_names[HI_REPLYTO]
#define	HL_REPLYTO	8

#define	HI_REFERENCES	hi_references
#define	HN_REFERENCES	mailmsghdrs_names[HI_REFERENCES]
#define	HL_REFERENCES	10

#define	HI_CTYPE	hi_ctype
#define	HN_CTYPE	mailmsghdrs_names[HI_CTYPE]
#define	HL_CTYPE	12

#define	HI_EXPIRES	hi_expires
#define	HN_EXPIRES	mailmsghdrs_names[HI_EXPIRES]
#define	HL_EXPIRES	sizeof(HN_EXPIRES)

#define	HI_KEYWORDS	hi_keywords
#define	HN_KEYWORDS	mailmsghdrs_names[HI_KEYWORDS]
#define	HL_KEYWORDS	8

#define	HI_CONTROL	hi_control
#define	HN_CONTROL	mailmsghdrs_names[HI_CONTROL]
#define	HL_CONTROL	7

#define	HI_XLINES	hi_xlines
#define	HN_XLINES	mailmsghdrs_names[HI_XLINES]
#define	HL_XLINES	7

#define	HI_PATH		hi_path
#define	HN_PATH		mailmsghdrs_names[HI_PATH]
#define	HL_PATH		4

#define	HI_ERRORSTO	hi_errorsto
#define	HN_ERRORSTO	mailmsghdrs_names[HI_ERRORSTO]
#define	HL_ERRORSTO	9

#define	HI_RETURNPATH	hi_returnpath
#define	HN_RETURNPATH	mailmsghdrs_names[HI_RETURNPATH]
#define	HL_RETURNPATH	11

#define	HI_RECEIVED	hi_received
#define	HN_RECEIVED	mailmsghdrs_names[HI_RECEIVED]
#define	HL_RECEIVED	8

#define	HI_XQUEUESPEC	hi_xqueuespec
#define	HN_XQUEUESPEC	mailmsghdrs_names[HI_XQUEUESPEC]
#define	HL_XQUEUESPEC	11

#define	HI_XSERVICE	hi_xservice
#define	HN_XSERVICE	mailmsghdrs_names[HI_XSERVICE]
#define	HL_XSERVICE	9

#define	HI_XJOBID	hi_jobid
#define	HN_XJOBID	mailmsghdrs_names[HI_XJOBID]
#define	HL_XJOBID	7

#define	HI_XORIGHOST	hi_xorghost
#define	HN_XORIGHOST	mailmsghdrs_names[HI_XORIGHOST]
#define	HL_XORIGHOST	10

#define	HI_XORIGUSER	hi_xorguser
#define	HN_XORIGUSER	mailmsghdrs_names[HI_XORIGUSER]
#define	HL_XORIGUSER	10

#define	HI_XUSERNAME	hi_xusername
#define	HN_XUSERNAME	mailmsghdrs_names[HI_XUSERNAME]
#define	HL_XUSERNAME	10

#define	HI_SENDER	hi_sender
#define	HN_SENDER	mailmsghdrs_names[HI_SENDER]
#define	HL_SENDER	6

#define	HI_CC		hi_cc
#define	HN_CC		mailmsghdrs_names[HI_CC]
#define	HL_CC		2

#define	HI_BCC		hi_bcc
#define	HN_BCC		mailmsghdrs_names[HI_BCC]
#define	HL_BCC		3

#define	HI_STATUS	hi_status
#define	HN_STATUS	mailmsghdrs_names[HI_STATUS]
#define	HL_STATUS	6

#define	HI_CLINES	hi_clines
#define	HN_CLINES	mailmsghdrs_names[HI_CLINES]
#define	HL_CLINES	13		/* content-lines */

#define	HI_CENCODING	hi_cencoding
#define	HN_CENCODING	mailmsghdrs_names[HI_CENCODING]
#define	HL_CENCODING	16		/* content-encoding */

#define	HI_ORGANIZATION	hi_organization
#define	HN_ORGANIZATION	mailmsghdrs_names[HI_ORGANIZATION]
#define	HL_ORGANIZATION	12		/* organization */

#define	HI_DELIVEREDTO	hi_deliveredto
#define	HN_DELIVEREDTO	mailmsghdrs_names[HI_DELIVEREDTO]
#define	HL_DELIVEREDTO	sizeof(HN_DELIVEREDTO)

#define	HI_XORIGINALTO	hi_xoriginalto
#define	HN_XORIGINALTO	mailmsghdrs_names[HI_XORIGINALTO]
#define	HL_XORIGINALTO	sizeof(HN_XORIGINALTO)

#define	HI_XPRIORITY	hi_xpriority
#define	HN_XPRIORITY	mailmsghdrs_names[HI_XPRIORITY]
#define	HL_XPRIORITY	sizeof(HN_XPRIORITY)

#define	HI_PRIORITY	hi_priority
#define	HN_PRIORITY	mailmsghdrs_names[HI_PRIORITY]
#define	HL_PRIORITY	sizeof(HN_PRIORITY)

#define	HI_XFACE	hi_xface
#define	HN_XFACE	mailmsghdrs_names[HI_XFACE]
#define	HL_XFACE	sizeof(HN_XFACE)

#define	HI_XBBNEWS	hi_xbbnews
#define	HN_XBBNEWS	mailmsghdrs_names[HI_XBBNEWS]
#define	HL_XBBNEWS	sizeof(HN_XBBNEWS)

#define	HI_XUUID	hi_xuuid
#define	HN_XUUID	mailmsghdrs_names[HI_XUUID]
#define	HL_XUUID	sizeof(HN_XUUID)

#define	HI_XUTI		hi_xuti
#define	HN_XUTI		mailmsghdrs_names[HI_XUTI]
#define	HL_XUTI		sizeof(HN_XUTI)

#define	HI_XMCDATE	hi_xmcdate
#define	HN_XMCDATE	mailmsghdrs_names[HI_XMCDATE]
#define	HL_XMCDATE	sizeof(HN_XMXDATE)

#define	HI_XMAILER	hi_xmailer
#define	HN_XMAILER	mailmsghdrs_names[HI_XMAILER]
#define	HL_XMAILER	8

#define	HI_XFORWARDEDTO	hi_xforwardto
#define	HN_XFORWARDEDTO	mailmsghdrs_names[HI_XFORWARDEDTO]
#define	HL_XFORWARDEDTO	14

#define	HI_SUBJ		hi_subj
#define	HN_SUBJ		mailmsghdrs_names[HI_SUBJ]
#define	HL_SUBJ		4

/* put all new entries before this last (fake) one */
#define	HI_OVERLAST	hi_overlast
#define	HN_OVERLAST	NULL
#define	HL_OVERLAST	-1


struct mailmsghdrs_head {
	cchar		**v ;
	uint		magic ;
} ;

extern cpcchar		mailmsghdrs_names[] ;

typedef	MAILMSGHDRS	mailmsghdrs ;

EXTERNC_begin

extern int mailmsghdrs_start(mailmsghdrs *,mailmsg *) noex ;
extern int mailmsghdrs_finish(mailmsghdrs *) noex ;

EXTERNC_end


#endif /* MAILMSGHDRS_INCLUDE */



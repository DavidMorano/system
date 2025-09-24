/* mailmsghdrs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* put all of the mail-message header values of a message into an array */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsghdrs

	Description:
	This subroutine depends on a MAILMSG object to have already
	been instantiated (and initialized).  This present object
	should then be initialized with a pointer to the MAILMSG
	object.  This object will then put all of the message header
	values of the MAILMSG object into an array for quick (indexed)
	access.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mailmsg.h>
#include	<localmisc.h>

#include	"mailmsghdrs.h"


/* local defines */

#define	SPACETAB(c)	(((c) == ' ') || ((c) == '\t'))


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int mailmsghdrs_magic(mailmsghdrs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGHDRS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsghdrs_magic) */


/* local variables */


/* exported variables */

cpcchar		mailmsghdrs_names[] = {
	[hi_from] 		= "From",
	[hi_to] 		= "To",
	[hi_date] 		= "Date",
	[hi_subject] 		= "Subject",
	[hi_title] 		= "title",
	[hi_messageid] 		= "message-id",
	[hi_articleid] 		= "article-id",
	[hi_clength] 		= "content-length",
	[hi_newsgroups] 	= "newsgroups",
	[hi_inreplyto] 		= "in-reply-to",
	[hi_board] 		= "board",
	[hi_lines] 		= "lines",
	[hi_replyto] 		= "reply-to",
	[hi_references] 	= "references",
	[hi_ctype] 		= "content-type",
	[hi_expires] 		= "expires",
	[hi_keywords] 		= "keywords",
	[hi_control] 		= "control",
	[hi_xlines] 		= "x-lines",
	[hi_path] 		= "path",
	[hi_errorsto] 		= "errors-to",
	[hi_returnpath] 	= "return-path",
	[hi_received] 		= "received",
	[hi_xqueuespec] 	= "x-queuespec",
	[hi_xservice] 		= "x-service",
	[hi_xjobid] 		= "x-jobid",
	[hi_xorighost] 		= "x-orighost",
	[hi_xoriguser] 		= "x-origuser",
	[hi_xusername] 		= "x-username",
	[hi_sender] 		= "sender",
	[hi_cc] 		= "cc",
	[hi_bcc] 		= "bcc",
	[hi_status] 		= "status",
	[hi_clines] 		= "content-lines",
	[hi_cencoding] 		= "content-transfer-encoding",
	[hi_organization] 	= "organization",
	[hi_deliveredto] 	= "delivered-to",
	[hi_xoriginalto] 	= "x-original-to",
	[hi_xpriority] 		= "x-priority",
	[hi_priority] 		= "priority",
	[hi_xface] 		= "x-face",
	[hi_xbbnews] 		= "x-bbnews",
	[hi_xuuid] 		= "x-universally-unique-identifier",
	[hi_xuti] 		= "x-uniform-type-identifier",
	[hi_xmcd] 		= "x-mail-created-date",
	[hi_xmailer] 		= "x-mailer",
	[hi_xforwardedto] 	= "x-forwarded-to",
	[hi_subj] 		= "subj",
	[hi_overlast] 		= nullptr	
} ; /* end array (mailmsghdrs_names) */


/* exported subroutines */

int mailmsghdrs_start(mailmsghdrs *op,mailmsg *msgp) noex {
	cint		n = HI_OVERLAST ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op && msgp) {
	    cint	sz = (n + 1) * szof(char **) ;
	    if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	        int	i ; /* used-afterwards */
	        mainv	mhnames = mailmsghdrs_names ;
	        cchar	*hp{} ;
	        op->v = ccharpp(p) ;
	        for (i = 0 ; (i < n) && mhnames[i] ; i += 1) {
	            op->v[i] = nullptr ;
	            if ((rs = mailmsg_hdrval(msgp,mhnames[i],&hp)) >= 0) {
	                op->v[i] = hp ;
	                c += 1 ;
		    } else if (rs == SR_NOTFOUND) {
		        rs = SR_OK ;
	            } /* end if (message header search) */
	        } /* end for (looping over header names) */
	        op->v[i] = nullptr ;
	        if (rs >= 0) {
		    op->magic = MAILMSGHDRS_MAGIC ;
		}
		if (rs < 0) {
		    rs1 = libmem.free(p) ;
		    if (rs >= 0) rs = rs1 ;
		    op->v = nullptr ;
		    op->magic = 0 ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsghdrs_start) */

int mailmsghdrs_finish(mailmsghdrs *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsghdrs_magic(op)) >= 0) {
	    if (op->v) {
	        rs1 = libmem.free(op->v) ;
	        if (rs >= 0) rs = rs1 ;
	        op->v = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsghdrs_finish) */



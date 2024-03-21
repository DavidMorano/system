/* mailmsghdrs SUPPORT */
/* lang=C++20 */

/* put all of the header values of a message into an array */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine depends on a MAILMSG object to have already
	been instantiated (and initialized).  This present object
	should then be initialized with a pointer to the MAILMSG
	object.  This object will then put all of the message header
	values of the MAILMSG object into an array for quick (indexed)
	access.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<mailmsg.h>
#include	<localmisc.h>

#include	"mailmsghdrs.h"


/* local defines */

#define	SPACETAB(c)	(((c) == ' ') || ((c) == '\t'))


/* imported namespaces */


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
	"From",			/* 0 */
	"To",
	"Date",
	"Subject",
	"title",
	"message-id",		/* 5 */
	"article-id",
	"content-length",
	"newsgroups",
	"in-reply-to",
	"board",		/* 10 */
	"lines",
	"reply-to",
	"references",
	"content-type",
	"expires",		/* 15 */
	"keywords",
	"control",
	"x-lines",
	"path",
	"errors-to",		/* 20 */
	"return-path",
	"received",
	"x-queuespec",		/* 23 */
	"x-service",
	"x-jobid",
	"x-orighost",
	"x-origuser",
	"x-username",
	"sender",
	"cc",
	"bcc",
	"status",
	"content-lines",	/* 33 */
	"content-transfer-encoding",	/* 34 */
	"organization",		/* 35 */
	"delivered-to",		/* 36 */
	"x-original-to",	/* 37 */
	"x-priority",		/* 38 */
	"priority",		/* 39 */
	"x-face",		/* 40 */
	"x-bbnews",		/* 41 */
	"x-universally-unique-identifier",
	"x-uniform-type-identifier",
	"x-mail-created-date",
	"x-mailer",		/* 45 */
	"x-forwarded-to",	/* 46 */
	"subj",			/* 47 */
	nullptr			/* 48 */
} ;


/* exported subroutines */

int mailmsghdrs_start(mailmsghdrs *op,mailmsg *msgp) noex {
	cint		n = (HI_OVERLAST + 1) ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && msgp) {
	    int		sz = (n+1) * sizeof(char **) ;
	    void	*p{} ;
	    if ((rs = uc_malloc(sz,&p)) >= 0) {
	        int	i ; /* used-afterwards */
	        mainv	mhnames = mailmsghdrs_names ;
	        cchar	*hp ;
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
	        if (rs >= 0) op->magic = MAILMSGHDRS_MAGIC ;
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
	        rs1 = uc_free(op->v) ;
	        if (rs >= 0) rs = rs1 ;
	        op->v = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsghdrs_finish) */



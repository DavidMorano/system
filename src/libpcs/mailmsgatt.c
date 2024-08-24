/* mailmsgatt SUPPORT */
/* lang=C++20 */

/* mail-message attachment object processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

	= 2001-01-03, David A­D­ Morano
	I changed the |mailmsgattent_type()| subroutine slightly.
	I changed it so that when no content type if found, it will
	assume a binary content type rather than returning a
	SR_NOTFOUND error.

*/

/* Copyright © 1998,2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object processes and manipulates email mailmsgattment
	and address.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecitem.h>
#include	<localmisc.h>

#include	"mailmsgatt.h"


/* local defines */

#define	MAILMSGATT_DEFENTS	10


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgatt_start(mailmsgatt *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	ne = MAILMSGATT_DEFENTS ;
	    cint	vo = VECITEM_OCOMPACT ;
	    rs = vecitem_start(op,ne,vo) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_start) */

int mailmsgatt_finish(mailmsgatt *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        mailmsgattent	*ep ;
	        for (int i = 0 ; vecitem_get(op,i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                rs1 = mailmsgattent_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	    }
	    {
	        rs1 = vecitem_finish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_finish) */

/* add an attachment (w/ default content-type and content-encoding) */
int mailmsgatt_add(mailmsgatt *op,cc *ct,cc *ce,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && ct && sp) {
	    mailmsgattent	ve ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = mailmsgattent_start(&ve,ct,ce,sp,sl)) >= 0) {
	        cint	esize = sizeof(mailmsgattent) ;
	        rs = vecitem_add(op,&ve,esize) ;
	        if (rs < 0) {
	            mailmsgattent_finish(&ve) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_add) */

/* delete an mailmsgattment */
int mailmsgatt_del(mailmsgatt *alp,int i) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alp) {
	    mailmsgattent	*ep ;
	    if ((rs = vecitem_get(alp,i,&ep)) >= 0) {
	        if (ep) {
	            rs1 = mailmsgattent_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        rs1 = vecitem_del(alp,i) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_del) */

/* return the number of hosts seen so far */
int mailmsgatt_count(mailmsgatt *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = vecitem_count(op) ;
	}
	return rs ;
}
/* end subroutine (mailmsgatt_count) */

/* enumerate */
int mailmsgatt_enum(mailmsgatt *op,int i,mailmsgattent **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    rs = vecitem_get(op,i,epp) ;
	}
	return rs ;
}
/* end subroutine (mailmsgatt_enum) */

/* find content types for all of the mailmsgattments using a MIME-types DB */
int mailmsgatt_typeatts(mailmsgatt *op,MIMETYPES *mtp) noex {
	int		rs = SR_FAULT ;
	if (op && mtp) {
	    mailmsgattent	*ep ;
	    rs = SR_OK ;
	    for (int i = 0 ; mailmsgatt_enum(op,i,&ep) >= 0 ; i += 1) {
	        if (ep) {
	            rs = mailmsgattent_type(ep,mtp) ;
	            if (rs < 0) break ;
	        }
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_typeatts) */



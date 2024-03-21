/* mailmsgatt SUPPORT */
/* lang=C++20 */

/* mail-message attachment processing */
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
#include	<sys/param.h>
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

int mailmsgatt_start(MAILMSGATT *rhp) noex {
	int		rs = SR_FAULT ;
	if (rhp) {
	    cint	ne = MAILMSGATT_DEFENTS ;
	    cint	vo = VECITEM_PNOHOLES ;
	    rs = vecitem_start(rhp,nemvo) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_start) */


/* free up the mailmsgattments list object */
int mailmsgatt_finish(MAILMSGATT *rhp)
{
	MAILMSGATTENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vecitem_get(rhp,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs1 = mailmsgattent_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	rs1 = vecitem_finish(rhp) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (mailmsgatt_finish) */

/* add an attachment (w/ default content-type and content-encoding) */
int mailmsgatt_add(MAILMSGATT *rhp,cc *ct,cc *ce,cc *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	if (rhp && ct && nbuf) {
	    MAILMSGATTENT	ve ;
	    if (nlen < 0) nlen = strlen(nbuf) ;
	    if ((rs = mailmsgattent_start(&ve,ct,ce,nbuf,nlen)) >= 0) {
	        cint	esize = sizeof(MAILMSGATTENT) ;
	        rs = vecitem_add(rhp,&ve,esize) ;
	        if (rs < 0) {
	            mailmsgattent_finish(&ve) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgatt_add) */

/* delete an mailmsgattment */
int mailmsgatt_del(MAILMSGATT *alp,int i) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alp) {
	    MAILMSGATTENT	*ep ;
	    if ((rs = vecitem_get(alp,i,&ep)) >= 0) {
	        if (ep != NULL) {
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
int mailmsgatt_count(MAILMSGATT *rhp) noex {
	int		rs = SR_FAULT ;
	if (rhp) {
	    rs = vecitem_count(rhp) ;
	}
	return rs ;
}
/* end subroutine (mailmsgatt_count) */

/* enumerate */
int mailmsgatt_enum(MAILMSGATT *rhp,int i,MAILMSGATTENT **epp) noex {
	int		rs = SR_FAULT ;
	if (rhp) {
	    rs = vecitem_get(rhp,i,epp) ;
	}
	return rs ;
}
/* end subroutine (mailmsgatt_enum) */


/* find content types for all of the mailmsgattments using a MIME-types DB */
int mailmsgatt_typeatts(MAILMSGATT *rhp,MIMETYPES *mtp) noex {
	MAILMSGATTENT	*ep ;
	int		rs = SR_OK ;

	if (rhp == NULL) return SR_FAULT ;

	for (int i = 0 ; mailmsgatt_enum(rhp,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs = mailmsgattent_type(ep,mtp) ;
	        if (rs < 0) break ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mailmsgatt_typeatts) */


/* private subroutines */



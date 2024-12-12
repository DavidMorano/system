/* mkdisphdr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* create (in a buffer) a sort of nice mail address for display purposes */
/* version %I% last-modified %G% */

#define	CF_NONSTANDARD	1		/* assume non-standard addressing */

/* revision history:

	= 2005-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkdisphdr

	Description:
	This subroutine creates (in a buffer) a sort of nice mail
	address for display purposes.

	Synopsis:
	int mkdisphdr(char *mbuf,int mlen,cchar *ap,int al) noex

	Arguments:
	mbuf		buffer to hold result
	mlen		length of given buffer
	ap		argument
	al		argument-length

	Returns:
	>=0		length of created result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<field.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mkdisphdr.h"


/* local defines */

#ifndef	CF_NONSTANDARD
#define	CF_NONSTANDARD	1
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_nonstandard = CF_NONSTANDARD ;


/* exported variables */


/* exported subroutines */

int mkdisphdr(char *abuf,int alen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (abuf && sp) {
	    sbuf	b ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = sbuf_start(&b,abuf,alen)) >= 0) {
	        field	fsb ;
	        cint	flen = sl ;
	        char	*fbuf{} ;
	        if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            if ((rs = field_start(&fsb,sp,sl)) >= 0) {
			nullptr_t	np{} ;
	                cchar		*fp = fbuf ;
	                int		fl ;
	                int		c = 0 ;
	                while ((fl = field_sharg(&fsb,np,fbuf,flen)) >= 0) {
	                    if (fl > 0) {
			        if constexpr (f_nonstandard) {
	                            if (c++ > 0) {
	                                rs = sbuf_char(&b,' ') ;
			            }
			        } /* end if-constexpr (non-standard) */
	                        if (rs >= 0) {
	                            rs = sbuf_strw(&b,fp,fl) ;
			        }
			    }
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = field_finish(&fsb) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
	            rs1 = uc_free(fbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (memory allocation) */
	        len = sbuf_finish(&b) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkdisphdr) */

int mkaddrpart(char *abuf,int alen,cchar *sp,int sl) noex {
	return mkdisphdr(abuf,alen,sp,sl) ;
}
/* end subroutine (mkaddrpart) */

int mkdispaddr(char *abuf,int alen,cchar *sp,int sl) noex {
	return mkdisphdr(abuf,alen,sp,sl) ;
}
/* end subroutine (mkdispaddr) */



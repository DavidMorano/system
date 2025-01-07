/* received SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage a "received" object */
/* version %I% last-modified %G% */

#define	CF_SAFE		0		/* run in "safe" mode */
#define	CF_EMPTYVALUE	0		/* allow for empty values */
#define	CF_FIELDWORD	1		/* used 'field_word(3dam)' */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	received

	Description:
	This object module processes and stores the information in
	a "received" header field for mail messages.  Note that
	some (stupid) mailers put characters like quote characters
	and other weirdo characters into some of the value fields
	of the RECEIVED header.  I don't know how many of these
	weirdo characters are legal in an RFC-822 value (if any)
	but we are currently using 'field_word(3dam)' with only the
	semi-colon character as a terminator to try to handle these
	wirdo cases.  If quote characters are not allowed to represent
	a regular legal character (without any escape quoting),
	then maybe 'field_get(3dam)' should be used.

	Note:
	Note also that all RFC-822 comments are removed from the
	RECEIVED header value before trying to parse it out into
	components.  Doing full tokenization on these strings while
	preserving comments would make this crap as hard as it is
	for mail addresses (where we preserve comments associated
	with each address)!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<strings.h>
#include	<usystem.h>
#include	<field.h>
#include	<sbuf.h>
#include	<matstr.h>
#include	<matxstr.h>
#include	<localmisc.h>

#include	"mhcom.h"
#include	"received.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int received_bake(received *,int,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	received_keys[] = {
	"from",
	"by",
	"with",
	"id",
	"for",
	"date",
	"via",
	nullptr
} ;

/****

	  9	(tab)
	 10	(new line)
	 11	(vertical tab)
	 12	(form feed)
	 13	(carriage return)
	 32	(space)
	 59	;

****/

constexpr cchar		fterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* external variables */


/* exported subroutines */

int received_start(received *op,cchar *hbuf,int hlen) noex {
	int		rs ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (hbuf == nullptr) return SR_FAULT ;

	memclear(op) ;
	if (hlen < 0) hlen = strlen(hbuf) ;
	/* prepare a MHCOM object for comment parsing */
	mhcom		com ;
	if ((rs = mhcom_start(&com,hbuf,hlen)) >= 0) {
	    cchar	*sp ;
	    int		sl ;
	    if ((sl = mhcom_getval(&com,&sp)) > 0) {
	        cint	sz = (sl + 1) ;
	        if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	            op->a = charp(p) ;
	            if ((rs = received_bake(op,sz,sp,sl)) >= 0) {
	                c = rs ;
	                op->magic = RECEIVED_MAGIC ;
	            }
	            if (rs < 0) {
	                uc_free(op->a) ;
	                op->a = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (non-zero content) */
	    mhcom_finish(&com) ;
	} /* end if (mhcom) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (received_start) */

int received_finish(received *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != RECEIVED_MAGIC) return SR_NOTOPEN ;
#endif

	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (received_finish) */

int received_getkey(received *op,int ki,cchar **rpp) noex {
	int		cl = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != RECEIVED_MAGIC) return SR_NOTOPEN ;
#endif

	if (ki < 0) return SR_INVALID ;

	if (ki >= received_keyoverlast) return SR_NOENT ;

	cl = (op->key[ki] != nullptr) ? strlen(op->key[ki]) : 0 ;

	if (rpp != nullptr) {
	    *rpp = op->key[ki] ;
	}

	return cl ;
}
/* end subroutine (received_getkey) */

int received_getitem(received *op,int ki,cchar **rpp) noex {
	return received_getkey(op,ki,rpp) ;
}
/* end subroutine (received_getitem) */


/* private subroutines */

static int received_bake(received *op,int sz,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		ki = -1 ;
	int		wi = 0 ;
	int		c = 0 ;
	int		f_prevmatch = false ;

	if (sbuf sb ; (rs = sbuf_start(&sb,op->a,sz)) >= 0) {
	    if (field fsb ; (rs = field_start(&fsb,sp,sl)) >= 0) {
	        cchar	*fp ;
	        int	fl ;

	        while (rs >= 0) {

#if	CF_FIELDWORD
	            fl = field_word(&fsb,fterms,&fp) ;
#else
	            fl = field_get(&fsb,fterms,&fp) ;
#endif

	            if (fl <= 0)
	                break ;

	            rs1 = -1 ;
	            if (! f_prevmatch)
	                rs1 = matcasestr(received_keys,fp,fl) ;

#if	(! CF_EMPTYVALUE)
	            f_prevmatch = (rs1 >= 0) ;
#endif

	            if (rs1 >= 0) {

	                if (c > 0)
	                    sbuf_chr(&sb,'\0') ;

	                ki = rs1 ;
	                wi = 0 ;

	            } else if (ki >= 0) {

	                if ((wi == 0) && (ki >= 0)) {
	                    cchar	*cp ;

	                    sbuf_getpoint(&sb,&cp) ;

	                    op->key[ki] = cp ;
	                    c += 1 ;
	                }

	                if (wi > 0)
	                    sbuf_chr(&sb,' ') ;

	                rs = sbuf_strw(&sb,fp,fl) ;
	                wi += 1 ;

	            } /* end if */

	            if (fsb.term == ';') break ;
	        } /* end while */

	        if ((rs >= 0) && (fsb.term == ';')) {
	            cchar	*cp ;
	            int		cl ;
	            if (c > 0)
	                sbuf_chr(&sb,'\0') ;

	            sbuf_getpoint(&sb,&cp) ;

	            op->key[received_keydate] = cp ;
	            c += 1 ;

	            cp = (cchar *) fsb.lp ;
	            cl = fsb.ll ;
	            sbuf_strw(&sb,cp,cl) ;

	        } /* end if (had a date) */

	        field_finish(&fsb) ;
	    } /* end if (field) */

	    rs1 = sbuf_finish(&sb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (received_bake) */



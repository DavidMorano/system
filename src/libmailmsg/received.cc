/* received SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage a "received" object */
/* version %I% last-modified %G% */

#define	CF_EMPTYVALUE	0		/* allow for empty values */
#define	CF_FIELDWORD	1		/* used |field_word(3dam)| */

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
	of the RECEIVED header.  I do not know how many of these
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<field.h>
#include	<sbuf.h>
#include	<matstr.h>
#include	<matxstr.h>
#include	<localmisc.h>

#include	"mhcom.h"
#include	"received.h"

import libutil ;

/* local defines */

#ifndef	CF_EMPTYVALUE
#define	CF_EMPTYVALUE	0		/* allow for empty values */
#endif

#ifndef	CF_FIELDWORD
#define	CF_FIELDWORD	1		/* used |field_word(3dam)| */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int received_magic(received *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RECEIVED_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (received_magic) */

static int received_bake(received *,int,cchar *,int) noex ;


/* local variables */

/****

	  9	(tab)
	 10	(new line)
	 11	(vertical tab)
	 12	(form feed)
	 13	(carriage return)
	 32	(space)
	 59	;

****/

constexpr char		fterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr bool		f_emptyvalue = CF_EMPTYVALUE ;
constexpr bool		f_fieldword = CF_FIELDWORD ;


/* exported variables */

cpcchar	received_keys[] = {
	"from",
	"by",
	"with",
	"id",
	"for",
	"date",
	"via",
	nullptr
} ;


/* exported subroutines */

int received_start(received *op,cchar *hbuf,int hlen) noex {
    	RECEIVED	*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op && hbuf) {
	    memclear(hop) ;
	    if (hlen < 0) hlen = lenstr(hbuf) ;
	    /* prepare a MHCOM object for comment parsing */
	    if (mhcom com ; (rs = com.start(hbuf,hlen)) >= 0) {
	        if (cchar *sp ; (rs = com.getval(&sp)) > 0) {
		    cint	sl = rs ;
	            cint	sz = (rs + 1) ;
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
	        rs1 = com.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (received) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (received_start) */

int received_finish(received *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = received_magic(op)) >= 0) {
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (received_finish) */

int received_getkey(received *op,int ki,cchar **rpp) noex {
    	int		rs ;
	int		cl = 0 ;
	if ((rs = received_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (ki >= 0) {
	        rs = SR_NOENT ;
	        if ((ki >= 0) && (ki < received_keyoverlast)) {
		    rs = SR_OK  ;
		    cl = (op->key[ki]) ? lenstr(op->key[ki]) : 0 ;
		    if (rpp) {
	    		*rpp = op->key[ki] ;
		    }
		} /* end if ("found") */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? cl : rs ;
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
	int		c = 0 ;
	if (sbuf sb ; (rs = sb.start(op->a,sz)) >= 0) {
	    if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) {
	        int	ki = -1 ;
	        int	wi = 0 ;
	        bool	f_prevmatch = false ;
	        cchar	*fp ;
	        int	fl ;
	        while (rs >= 0) {
		    if_constexpr (f_emptyvalue) {
	                fl = fsb.word(fterms,&fp) ;
		    } else {
	                fl = fsb.get(fterms,&fp) ;
		    } /* end if_constexpr (f_emptyvalue) */
	            if (fl <= 0) break ;
	            rs1 = -1 ;
	            if (! f_prevmatch) {
	                rs1 = matcasestr(received_keys,fp,fl) ;
		    }
		    if_constexpr (! f_fieldword) {
	                f_prevmatch = (rs1 >= 0) ;
		    } /* end if_constexpr (! f_fieldword) */
	            if (rs1 >= 0) {
	                if (c > 0) {
	                    sb.chr('\0') ;
			}
	                ki = rs1 ;
	                wi = 0 ;
	            } else if (ki >= 0) {
	                if ((wi == 0) && (ki >= 0)) {
	                    cchar	*cp ;
	                    sb.getpoint(&cp) ;
	                    op->key[ki] = cp ;
	                    c += 1 ;
	                }
	                if (wi > 0) {
	                    sbuf_chr(&sb,' ') ;
			}
	                rs = sb.strw(fp,fl) ;
	                wi += 1 ;
	            } /* end if */
	            if (fsb.term == ';') break ;
	        } /* end while */
	        if ((rs >= 0) && (fsb.term == ';')) {
	            cchar	*cp ;
	            int		cl ;
	            if (c > 0) {
	                sb.chr('\0') ;
		    }
	            sb.getpoint(&cp) ;
		    {
	                op->key[received_keydate] = cp ;
	                c += 1 ;
		    }
	            cp = ccharp(fsb.lp) ;
	            cl = fsb.ll ;
	            sb.strw(cp,cl) ;
	        } /* end if (had a date) */
	        rs1 = fsb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (received_bake) */

int received::start(cchar *sp,int sl) noex {
	return received_start(this,sp,sl) ;
}

int received::getkey(int ki,cchar **rpp) noex {
	return received_getkey(this,ki,rpp) ;
}

int received::getitem(int ki,cchar **rpp) noex {
	return received_getitem(this,ki,rpp) ;
}

void received::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("received",rs,"fini-finish") ;
	}
}

received_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case receivedmem_finish:
	        rs = received_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (received_co::operator) */



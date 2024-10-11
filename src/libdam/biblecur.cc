/* biblecur SUPPORT */
/* lang=C++20 */

/* bible-cursor */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-01, David A­D­ Morano
	This subroutine was written as an enhancement for adding
	back-matter (end pages) to the output document.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	biblecur

	Description:
	This subroutine does something! :-)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<six.h>
#include	<cfdec.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"biblecur.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	bufprintf(char *,int,cchar *,...) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int biblecur_ctor(biblecur *op,Args ... args) noex {
    	BIBLECUR	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (biblecur_ctor) */

static int biblecur_dtor(biblecur *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (biblecur_dtor) */

template<typename ... Args>
static inline int biblecur_magic(biblecur *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == BIBLECUR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (biblecur_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int biblecur_start(biblecur *op) noex {
    	int		rs ;
	if ((rs = biblecur_ctor(op)) >= 0) {
	    op->book = -1 ;
	    op->chapter = -1 ;
	    op->verse = -1 ;
	    if (rs < 0) {
		biblecur_dtor(op) ;
	    }
	} /* end if (biblecur_ctor) */
	return rs ;
}
/* end subroutine (biblecur_start) */

int biblecur_finish(biblecur *op) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = biblecur_magic(op)) >= 0) {
	    op->book = -1 ;
	    op->chapter = -1 ;
	    op->verse = -1 ;
	    {
		rs1 = biblecur_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblecur_finish) */

int biblecur_check(biblecur *op,cchar *sqp,int sql) noex {
	int		rs ;
	int		rs1 ;
	int		si = 0 ;
	if ((rs = biblecur_magic(op,sqp)) >= 0) {
	    int		sl = sql ;
	    cchar	*sp = sqp ;
	    if (sl < 0) sl = strlen(sp) ;
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (sl > 0) {
	        /* start of new line */
	        int	ch = MKCHAR(*sp) ;
	        if (isdigitlatin(ch)) {
	            int		n_book = -1 ;
	            int		n_chapter = -1 ;
	            int		n_verse = -1 ;
	            char	nterms[32] ;
	            fieldterms(nterms,false,": \t") ;
	            if (field fsb ; (rs = fsb.start(sp,sl)) >= 0) {
	                int	nval = 0 ;
	                int	i = 0 ;
	                int	fl ;
	                cchar	*fp ;
	                while ((fl = fsb.word(nterms,&fp)) >= 0) {
	                    if (fl > 0) {
	                        si = ((fp + fl) - sqp) ;
	                        if ((rs = cfdeci(fp,fl,&nval)) >= 0) {
	                            switch (i) {
	                            case 0:
	                                n_book = nval ;
	                                break ;
	                            case 1:
	                                n_chapter = nval ;
	                                break ;
	                            case 2:
	                                n_verse = nval ;
	                                break ;
	                            } /* end switch */
	                        } /* end if (ctdeci) */
	                    } /* end if (non-zero) */
	                    i += 1 ;
	                    if (i == 3) break ;
	                    if (rs < 0) break ;
	                } /* end while (field_word) */
	                rs1 = fsb.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
	            if (rs >= 0) {
	                if ((n_book >= 0) && (n_book != op->book)) {
	                    op->f.newbook = true ;
	                    op->f.newchapter = true ;
	                    op->f.newverse = true ;
	                    op->book = n_book ;
	                }
	                if ((n_chapter >= 0) && (n_chapter != op->chapter)) {
	                    op->f.newchapter = true ;
	                    op->f.newverse = true ;
	                    op->chapter = n_chapter ;
	                }
	                if ((n_verse >= 0) && (n_verse != op->verse)) {
	                    op->f.newverse = true ;
	                    op->verse = n_verse ;
	                }
	                si += siskipwhite((sp + si),(sl - si)) ;
	            } /* end if (ok) */
	        } /* end if (proper character type) */
	    } /* end if (positive) */
	} /* end if (magic) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (biblecur_check) */

int biblecur_newbook(biblecur *op,biblebook *bbp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = biblecur_magic(op,bbp)) >= 0) {
	    if ((f = op->f.newbook) != 0) {
	        cint	blen = BIBLEBOOK_LEN ;
	        if ((rs = biblebook_get(bbp,op->book,op->bookname,blen)) >= 0) {
		    rs = SR_OK ;
	        } else if (isNotPresent(rs)) {
	            rs = bufprintf(op->bookname,blen,"Book %u",op->book) ;
	        }
	    }
	    op->f.newbook = false ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (biblecur_newbook) */

int biblecur_newchapter(biblecur *op) noex {
    	int		rs ;
	int		f = false ;
	if ((rs = biblecur_magic(op)) >= 0) {
	     f = op->f.newchapter ;
	     op->f.newchapter = false ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (biblecur_newchapter) */

int biblecur_newverse(biblecur *op,int sl) noex {
    	int		rs ;
	int		f = false ;
	if ((rs = biblecur_magic(op)) >= 0) {
	    if (op->f.newverse && (sl > 0)) {
	        f = true ;
	        op->f.newverse = false ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (biblecur_newverse) */



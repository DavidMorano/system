/* field */
/* lang=C++20 */

/* routine to parse a line into fields */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A� D� Morano
	This code module was originally written in VAX assembly
	(back around 1980 or so). It was then written into C-language
	perhaps around 1983 (at AT&T Bell Laboratories).  This
	(below) is a new incarnation based on that older C-language
	code.

	= 2023-11-04, David A� D� Morano
	I (finally) changed the code to initialize the default
	character terminator blocks using the C++14 'constexpr'
	keyword.  Previously, the character terminator blocks were
	generated by an external program and then copy-and-pasted
	into here. :-)  Yes, we have finally come a long way. For
	the record, the 'constexpr' code limitations of C++11 were
	too restrictive to really comfortably do this type of
	initialization previously.

*/

/* Copyright � 1998,2023 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_get

	Description:
	This object parses lines of c-strings for 'fields'.
	Terminator characters are used to delineate fields.

	Synopsis:
	int field_get(field *fsbp,cchar *terms,cchar **fpp) noex

	Arguments:
	- address of return status block
	- address of terminator block

	Returns:
	>=0	length of field just parsed out
	<0	error or not-found

	The return status block outputs are:
	- length remaining in string
	- address of reminaing string
	- len of substring
	- address of substring
	- terminator character


	Name:
	field_sharg

	Description:
	This object retrieves the next shell-like argument (if there
	is one).

	Arguments:
	fsbp		field status block pointer
	terms		bit array of terminating characters
	fbuf		buffer to store result
	flen		length of buffer to hold result

	Returns:
	>=0	length of field just parsed out
	<0	error or not-found


	Notes:
	This is a classic, ported forward from the old VAX-11/70
	days. This was translated from VAX assembly language.

	Note:
	This code was messy when it was assembly language.  And it
	is still messy to this very day (here in C-lnaguage). I had
	to get this statement out as a sort of disclaimer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<ascii.h>
#include	<baops.h>
#include	<char.h>
#include	<mkchar.h>
#include	<strop.h>
#include	<localmisc.h>

#include	"field.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* local structures */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

namespace {
    struct terminit {
	char		terms[termsize] = {} ;
	constexpr terminit(cchar *s) noex {
	    while (*s) {
		cint	ch = mkchar(*s++) ;
		baset(terms,ch) ;
	    }
	} ;
    } ; /* end struct (terminit) */
}


/* local variables */

constexpr terminit	quotes("\"\'") ;
constexpr terminit	doubles("\"$\\`") ;
constexpr terminit	dterms("\r!#$%&,:;<=>") ;
constexpr terminit	shterms(" #") ;


/* local inline-subroutines */

static inline bool istermorquote(cchar *terms,int ch) noex {
	return (batst(terms,ch) || batst(quotes.terms,ch)) ;
}

static inline bool istermandnotquote(cchar *terms,int ch) noex {
	return batst(terms,ch) && (! batst(quotes.terms,ch)) ;
}

static inline bool isquoteanddouble(int sl,int ch) noex {
	return ((ch == CH_SQUOTE) && (sl > 1) && batst(doubles.terms,ch)) ;
}


/* exported subroutines */

int fieldterms(char *terms,int f_retain,cchar *s) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms && s) {
	    if (! f_retain) {
		memclear(terms,termsize) ;
	    } /* end if */
	    while (*s) {
		cint	ch = mkchar(*s++) ;
	        baset(terms,ch) ;
	        c += 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fieldterms) */

int fieldtermsx(char *terms,int f,int na,...) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms) {
	    rs = SR_OK ;
	    if (! f) {
		memclear(terms,termsize) ;
	    } /* end if */
	    if (na > 0) {
	        va_list	ap ;
		va_begin(ap,na) ;
	        for (int i = 0 ; i < na ; i += 1) {
		    int	ch = (int) va_arg(ap,int) ;
		    ch &= UCHAR_MAX ;
	            baset(terms,ch) ;
	            c += 1 ;
		} /* end for */
		va_end(ap) ;
	    } /* end if (non-zero-positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fieldtermx) */

int field_start(field *fsbp,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	if (fsbp && lp) {
	    rs = SR_OK ;
	    fsbp->fp = nullptr ;
	    fsbp->fl = 0 ;
	    fsbp->term = '\0' ;
	    if (ll < 0) ll = strlen(lp) ;
	    while ((ll > 0) && char_iswhite(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    } /* end while */
	    if (*lp == '\0') ll = 0 ;
	    fsbp->lp = lp ;
	    fsbp->ll = ll ;
	} /* end if (non-null) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (field_start) */

int field_finish(field *fsbp) noex {
	int		rs = SR_FAULT ;
	if (fsbp) {
	    rs = SR_NOTOPEN ;
	    if (fsbp->lp) {
		rs = SR_OK ;
		fsbp->ll = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (field_finish) */

int field_get(field *fsbp,cchar *terms,cchar **fpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fl = 0 ;
	cchar		*fp = nullptr ;
	if (fsbp && fpp) {
	    strop	so ;
	    if (terms == nullptr) terms = dterms.terms ;
	    if ((rs = so.start(fsbp->lp,fsbp->ll)) > 0) {
		int		ch ;
		int		chterm = '\0' ;
		so.white() ;
		fp = so.sp ;	/* <- first field? */
	        if (so.sl > 0) {
	            ch = mkchar(*so.sp) ;
	            if (batst(quotes.terms,ch)) {
	                cint	qe = ch ;	/* set default quote end */
			so.inc() ;
	        	fp = so.sp ; 		/* save field address */
			so.findchr(qe) ;
	        	fl = (so.sp - fp) ;
			so.inc() ;
	    	    } else if (! batst(terms,ch)) {
	        	fp = so.sp ;		/* save field address */
			so.inc() ;
	        	while (so.sl > 0) {
	            	    ch = mkchar(*so.sp) ;
	            	    if (istermorquote(terms,ch)) break ;
			    if (char_iswhite(ch)) break ;
			    so.inc() ;
	        	} /* end while */
	        	fl = (so.sp - fp) ;
		    } /* end if (processing a field) */
	    	    if ((so.sl > 0) && char_iswhite(*so.sp)) {
	                chterm = ' ' ;
			so.white() ;
	            } /* end if */
	            ch = mkchar(*so.sp) ;
	            if ((so.sl > 0) && istermandnotquote(terms,ch)) {
	                chterm = ch ;
			so.inc() ;
	            } /* end if */
		} else {
		    fl = SR_NOTFOUND ;
	        } /* end if (positive) */
		fsbp->ll = so.sl ;
		fsbp->lp = so.sp ;
		fsbp->fl = fl ;
		fsbp->fp = (fl >= 0) ? fp : nullptr ;
		fsbp->term = chterm ;
		*fpp = fsbp->fp ;
		rs1 = so.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (so) */
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_get) */

int field_term(field *fsbp,cchar *terms,cchar **fpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fl = 0 ;
	cchar		*fp = nullptr ;
	if (fsbp && fpp) {
	    strop	so ;
	    if (terms == nullptr) terms = dterms.terms ;
	    if ((rs = so.start(fsbp->lp,fsbp->ll)) > 0) {
		int		ch ;
		int		chterm = '\0' ;
		so.white() ;
		fp = so.sp ;	/* <- first field? */
	        if (so.sl > 0) {
	            ch = mkchar(*so.sp) ;
	            if (! batst(terms,ch)) {
	        	fp = so.sp ; 		/* save field address */
			so.inc() ;
			so.findterm(terms) ;
	        	fl = (so.sp - fp) ;
	            } /* end if (processing a field) */
	            ch = mkchar(*so.sp) ;
	            if ((so.sl > 0) && batst(terms,ch)) {
	        	chterm = ch ;
			so.inc() ;
	            } /* end if */
		} else {
		    fl = SR_NOTFOUND ;
	        } /* end if (positive) */
	        fsbp->ll = so.sl ;
	        fsbp->lp = so.sp ;
	        fsbp->fl = fl ;
	        fsbp->fp = (fl >= 0) ? fp : nullptr ;
	        fsbp->term = chterm ;
	        *fpp = fsbp->fp ;
		rs1 = so.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (so) */
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_term) */

int field_sharg(field *fsbp,cchar *terms,char *fbuf,int flen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fl = 0 ;
	if (fsbp && fbuf) {
	    strop	so ;
	    if (terms == nullptr) terms = dterms.terms ;
	    if ((rs = so.start(fsbp->lp,fsbp->ll)) > 0) {
		int		chterm = '\0' ;
		so.white() ;
	        if (so.sl > 0) {
		    int		ch ;
		    char	*bp = fbuf ;
	            while (so.sl > 0) {
		        int	qe ;
	                ch = mkchar(*so.sp) ;
		        if (istermandnotquote(terms,ch)) break ;
	                if (char_iswhite(ch)) break ;
	                if (ch == CH_DQUOTE) {
	                    qe = ch ;
		            so.inc() ;
	                    while (so.sl > 0) {
			        int	nch = 0 ;
	                        ch = mkchar(*so.sp) ;
	                        if (so.sl > 1) nch = mkchar(so.sp[1]) ;
			        if (isquoteanddouble(so.sl,nch)) {
			            so.inc() ;
	                            ch = mkchar(*so.sp) ;
	                            if (flen > 0) {
	                                *bp++ = ch ;
	                                flen -= 1 ;
	                            }
			            so.inc() ;
	                        } else if (ch == qe) {
			            so.inc() ;
	                            break ;
	                        } else {
	                            if (flen > 0) {
	                                *bp++ = ch ;
	                                flen -= 1 ;
	                            }
			            so.inc() ;
	                        } /* end if */
	                    } /* end while (processing the quoted portion) */
	                } else if (ch == CH_SQUOTE) {
	                    qe = ch ;
		            so.inc() ;
	                    while (so.sl > 0) {
	                        ch = mkchar(*so.sp) ;
	                        if (ch == qe) {
			            so.inc() ;
	                            break ;
	                        } else {
	                            if (flen > 0) {
	                                *bp++ = ch ;
	                                flen -= 1 ;
	                            }
			            so.inc() ;
	                        } /* end if */
	                    } /* end while (processing the quoted portion) */
	                } else if ((ch == CH_SQUOTE) && (so.sl > 1)) {
		            so.inc() ;
	                    ch = mkchar(*so.sp) ;
	                    if (flen > 0) {
	                        *bp++ = ch ;
	                        flen -= 1 ;
	                    }
			    so.inc() ;
	                } else {
	                    if (flen > 0) {
	                        *bp++ = ch ;
	                        flen -= 1 ;
	                    }
			    so.inc() ;
	                } /* end if */
	            } /* end while (main loop) */
		    so.white() ;
	            if (so.sl > 0) {
	                chterm = ' ' ;
	                ch = mkchar(*so.sp) ;
		        if (istermandnotquote(terms,ch)) {
	                    chterm = ch ;	/* save terminator */
		            so.inc() ;
	                } /* end if */
	            } /* end if (positive) */
	            fl = (bp - fbuf) ;
	            *bp = '\0' ;
	        } else {
		    fl = SR_NOTFOUND ;
	        } /* end if (positive) */
	        fsbp->ll = so.sl ;
	        fsbp->lp = so.sp ;
	        fsbp->fl = fl ;
	        fsbp->fp = fbuf ;
	        fsbp->term = chterm ;
		rs1 = so.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (so) */
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_sharg) */

int field_remaining(field *fsbp,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		ll = 0 ;
	if (fsbp) {
	    rs = SR_NOTOPEN ;
	    if (fsbp->lp) {
		rs = SR_OK ;
		ll = fsbp->ll ;
	        if (lpp) {
	            *lpp = (cchar *) fsbp->lp ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (field_remaining) */



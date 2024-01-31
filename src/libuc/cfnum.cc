/* cfnum SUPPORT */
/* lang=C++20 */

/* convert from a number to an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-17, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfnum

	Description:
	These subroutines convert numeric strings in a variety of
	formats or bases into the integer-binary form.  These
	subroutines handle some cases that the |atox(3c)| family
	do not.  Also, we handle Latin-1 characters correctly;
	specifically with our use of:

		tolc(3uc)
		isdigitlatin(3uc)
		isalphalatin(3uc)

	instead of:

		tolower(3c)
		isdigit(3c)
		islapha(3c)

	The standard subroutines often go crazy when confronted with Latin-1
	characters with values >= 128.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* for |strnlen(3c)| */
#include	<concepts>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<cfbin.h>
#include	<cfoct.h>
#include	<cfdec.h>
#include	<cfhex.h>
#include	<ischarx.h>
#include	<char.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* subroutine-templaces */

template<typename T>
int cfnumx(cchar *bp,int bl,T *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 1) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdec(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhex(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoct(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbin(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdec(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhex(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoct(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbin(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfocti((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdec(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdec(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumx) */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cfnumi(cchar *bp,int bl,int *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 1) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdeci(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexi(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfocti(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbini(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdeci(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexi(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfocti(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbini(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfocti((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdeci(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdeci(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumi) */

int cfnumui(cchar *bp,int bl,uint *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 0) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdecui(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexui(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoctui(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbinui(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdecui(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexui(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoctui(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbinui(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */

	            } else if (bp[0] == '0') {
	                rs = cfoctui((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdecui(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdecui(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumui) */

int cfnuml(cchar *bp,int bl,long *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 1) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdecl(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexl(bp,bl,rp) ;
	                break ;

	            case 'o':
	                rs = cfoctl(bp,bl,rp) ;
	                break ;

	            case 'b':
	                rs = cfbinl(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdecl(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexl(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoctl(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbinl(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfoctl((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdecl(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdecl(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnuml) */

int cfnumul(cchar *bp,int bl,ulong *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 0) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdecul(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexul(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoctul(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbinul(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdecul(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexul(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoctul(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbinul(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfoctul((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdecul(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdecul(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumul) */

int cfnumll(cchar *bp,int bl,longlong *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 1) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdecll(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexll(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoctll(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbinll(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdecll(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexll(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoctll(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbinll(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfoctll((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdecll(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdecll(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumll) */

int cfnumull(cchar *bp,int bl,ulonglong *rp) noex {
	int		rs = SR_DOM ;
	int		ch ;
	int		f_negative = FALSE ;
	bl = strnlen(bp,bl) ;
	while ((bl > 0) && CHAR_ISWHITE(*bp)) {
	    bp += 1 ;
	    bl -= 1 ;
	}
	if ((bl > 0) && ((*bp == '+') || (*bp == '-'))) {
	    f_negative = (*bp == '-') ;
	    bp += 1 ;
	    bl -= 1 ;
	}
	if (bl > 0) {
	    if (*bp == '\\') {
	        bp += 1 ;
	        bl -= 1 ;
	        if (bl > 0) {
	            ch = tolc(bp[0]) ;
	            bp += 1 ;
	            bl -= 1 ;
	            switch (ch) {
	            case 'd':
	                rs = cfdecull(bp,bl,rp) ;
	                break ;
	            case 'x':
	                rs = cfhexull(bp,bl,rp) ;
	                break ;
	            case 'o':
	                rs = cfoctull(bp,bl,rp) ;
	                break ;
	            case 'b':
	                rs = cfbinull(bp,bl,rp) ;
	                break ;
	            default:
	                rs = SR_INVALID ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
		}
	    } else if (isdigitlatin(mkchar(*bp))) {
	        if (bl > 1) {
	            ch = tolc(bp[1]) ;
	            if (isalphalatin(ch)) {
	                bp += 2 ;
	                bl -= 2 ;
	                switch (ch) {
	                case 'd':
	                    rs = cfdecull(bp,bl,rp) ;
	                    break ;
	                case 'x':
	                    rs = cfhexull(bp,bl,rp) ;
	                    break ;
	                case 'o':
	                    rs = cfoctull(bp,bl,rp) ;
	                    break ;
	                case 'b':
	                    rs = cfbinull(bp,bl,rp) ;
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */
	            } else if (bp[0] == '0') {
	                rs = cfoctull((bp+1),(bl-1),rp) ;
	            } else {
	                rs = cfdecull(bp,bl,rp) ;
		    }
	        } else {
	            rs = cfdecull(bp,bl,rp) ;
		}
	    } /* end if */
	    if (f_negative) *rp = (- *rp) ;
	} /* end if */
	return rs ;
}
/* end subroutine (cfnumull) */



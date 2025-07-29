
static int mkclean(char *outbuf,int outlen,cchar *bufp,int bufl) noex {
	int		i ; /* used-afterwards */
	for (i = 0 ; (i < outlen) && (i < bufl) ; i += 1) {
	    outbuf[i] = bufp[i] ;
	    if (isourbad(bufp[i] & UCHAR_MAX)) {
	        outbuf[i] = 0xBF ;	/* upside-down question-mark */
	    }
	} /* end for */
	return i ;
}
/* end subroutine (mkclean) */

static bool hasourbad(cchar *sp,int sl) noex {
	int		f = false ;
	if (sp) {
	    while (sl && (sp[0] != '\0')) {
	        cint	ch = mkchar(sp[0]) ;
	        f = isourbad(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end if */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasourbad) */

static bool isourbad(int c) noex {
	bool		f = true ;
	switch (c) {
	case CH_SO:
	case CH_SI:
	case CH_SS2:
	case CH_SS3:
	case '\t':
	    f = false ;
	    break ;
	default:
	    f = (! isprintlatin(c)) ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isourbad) */



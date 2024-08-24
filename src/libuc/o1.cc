
static inline bool keyend(cint ch) noex {
	return (((ch) == '\0') || ((ch) == '=')) ;
}


/* exported subroutines */

int matkeystr(cchar **a,cchar *sp,int sl) noex {
	cint		sch = sp[0] ; /* ok: everything promotes the same */
	int		i ;
	int		f = FALSE ;
	if (sl >= 0) {
	    int		m ;
	    for (i = 0 ; a[i] != NULL ; i += 1) {
		m = (sch == a[i][0]) ;
		if (m > 0) {
		    m = nleadkeystr(a[i],sp,sl) ;
		}
		f = (((m == sl) || keyend(sp[m])) && keyend(a[i][m])) ;
		if (f) break ;
	    } /* end for */
	} else {
	    for (i = 0 ; a[i] != NULL ; i += 1) {
	        if (sch == a[i][0]) {
		    f = (strkeycmp(a[i],sp) == 0) ;
		} else {
		    f = (keyend(sp[0]) && keyend(a[i][0])) ;
		}
		if (f) break ;
	    } /* end for */
	} /* end if */
	return (f) ? i : -1 ;
}
/* end subroutine (matkeystr) */



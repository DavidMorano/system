void keyvals::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("keyvals",rs,"fini-finish") ;
	}
}

keyvals::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (n > 0) {
	    rs = c ;
	}
	return rs ;
}

keyvals_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case keyvalsmem_finish:
	        rs = keyvals_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (keyvals_co::operator) */



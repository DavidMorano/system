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
	    case keyvalsmem_addcspath:
	        rs = keyvals_addcspath(op) ;
	        break ;
	    case keyvalsmem_count:
	        rs = keyvals_count(op) ;
	        break ;
	    case keyvalsmem_delall:
	        rs = keyvals_delall(op) ;
	        break ;
	    case keyvalsmem_strsize:
	        rs = keyvals_strsize(op) ;
	        break ;
	    case keyvalsmem_recsize:
	        rs = keyvals_recsize(op) ;
	        break ;
	    case keyvalsmem_cksize:
	        rs = keyvals_cksize(op) ;
	        break ;
	    case keyvalsmem_audit:
	        rs = keyvals_audit(op) ;
	        break ;
	    case keyvalsmem_finish:
	        rs = keyvals_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (keyvals_co::operator) */

bool keyvals_iter::operator == (const keyvals_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool keyvals_iter::operator != (const keyvals_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
}
/* end method (keyvals_iter::operator) */



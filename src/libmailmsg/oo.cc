
void comparse::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("comparse",rs,"fini-finish") ;
	}
}

comparse_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case comparsemem_finish:
	        rs = comparse_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (comparse_co::operator) */



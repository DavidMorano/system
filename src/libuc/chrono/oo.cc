int dayspec::start(time_t t,cchar *sp,int sl) noex {
	return dayspec_start(this,t,sp,sl) ;
}

void dayspec::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("dayspec",rs,"fini-finish") ;
	}
}

dayspec_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case dayspecmem_finish:
	        rs = dayspec_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (dayspec_co::operator) */




void bfile::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("bfile",rs,"fini-finish") ;
	}
}

bfile_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case bfilemem_size:
	        rs = bfile_size(op) ;
	        break ;
	    case bfilemem_getc:
	        rs = bfile_getc(op) ;
	        break ;
	    case bfilemem_isterm:
	        rs = bfile_isterm(op) ;
	        break ;
	    case bfilemem_flush:
	        rs = bfile_flush(op) ;
	        break ;
	    case bfilemem_rewind:
	        rs = bfile_rewind(op) ;
	        break ;
	    case bfilemem_close:
	        rs = bfile_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (bfile_co::operator) */



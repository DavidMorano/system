	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTFOUND ;
	    if (vcf == nullptr) vcf = vstrcmp ;
	    if (op->va) ylikely {
		int	i ; /* used-afterwards */
	        for (i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                if ((*vcf)(&sp,(op->va + i)) == 0) break ;
	            }
	        } /* end for */
	        if (i < op->i) rs = i ;
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::finder) */


	            char	*lbuf{} ;
	            if ((rs = malloc_ml(&lbuf)) >= 0) {
		        cint	llen = rs ;
	                shio	cf ;
	                if ((rs = shio_open(&cf,cfname,"r",0666)) >= 0) {
	                    while ((rs = shio_read(&cf,lbuf,llen)) > 0) {
	                        rs = shio_write(op->ofp,lbuf,rs) ;
	                        wlen += rs ;
	                        if (rs < 0) break ;
	                    } /* end while (reading lines) */
	                    rs1 = shio_close(&cf) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (copy-content-file) */
		        rs1 = uc_free(lbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */

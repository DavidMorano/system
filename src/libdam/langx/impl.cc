    class widebuf {
	vector<wchar_t>	b ;
	int		oi ;		/* output index */
    public:
	widebuf() noex : oi(0) { } ;
	wchar_t operator [] (int i) const noex {
	    wchar_t	rch = 0 ;
	    csize	bsize = b.size() ;
	    {
	        cint	n = intconv(bsize) ;
	        if ((oi+i) < n) {
		    rch = b[oi+i] ;
	        }
	    }
	    return rch ;
	} ; /* end method (operator) */
	int add(int ch) noex {
	    int		rs ;
	    try {
	        b.push_back(ch) ;
		{
		    csize	bsize = b.size() ;
		    int		sz ;
		    {
		        sz = intconv(bsize) ;
		        rs = (sz - oi) ;
		    }
		}
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	    return rs ;
	} ;
	int add(cchar *sp,int µsl = -1) noex {
	    int		rs ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        try {
	            for (int i = 0 ; i < sl ; i += 1) {
		        b.push_back(sp[i]) ;
	            }
		    {
		        csize	bsize = b.size() ;
		        {
			    cint	sz = intconv(bsize) ;
	        	    rs = (sz - oi) ;
		        }
		    }
	        } catch (...) {
		    rs = SR_NOMEM ;
	        }
	    } /* end if (getlenstr) */
	    return rs ;
	} ; /* end method (add) */
	int count() const noex {
	    csize	bsize = b.size() ;
	    int		rs ;
	    {
		cint	sz = intconv(bsize) ;
	    	rs = (sz - oi) ;
	    }
	    return rs ;
	} ;
	int len() const noex {
	    return count() ;
	} ;
	wchar_t at(int i) const noex {
	    csize	bsize = b.size() ;
	    int		rch = 0 ;
	    {
	       cint	n = intconv(bsize) ;
	       if ((oi+i) < n) {
		   rch = b[oi+i] ;
	       }
	    }
	    return rch ;
	} ;
	int adv(int al) noex ;
    } ; /* end structure (widebuf) */

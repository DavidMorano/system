    struct filerec_iter {
	cchar		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	filerec_iter() = default ;
	filerec_iter(cchar **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	filerec_iter(const filerec_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	filerec_iter &operator = (const filerec_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const filerec_iter &) noex ;
	bool operator == (const filerec_iter &) noex ;
	cchar *operator * () noex {
	    cchar	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	filerec_iter operator + (int) const noex ;
	filerec_iter operator += (int) noex ;
	filerec_iter operator ++ () noex ; /* pre */
	filerec_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
    } ; /* end struct filerec_iter) */

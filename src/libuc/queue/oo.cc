#ifdef	__cplusplus
enum fifostrmems {
	fifostrmem_start,
	fifostrmem_headlen,
	fifostrmem_count,
	fifostrmem_finish,
	fifostrmem_overlast
} ;
struct fifostr_iter {
	cchar		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	fifostr_iter() = default ;
	fifostr_iter(cchar **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	fifostr_iter(const fifostr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	fifostr_iter &operator = (const fifostr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const fifostr_iter &) noex ;
	bool operator == (const fifostr_iter &) noex ;
	cchar *operator * () noex {
	    cchar	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	fifostr_iter operator + (int) const noex ;
	fifostr_iter operator += (int) noex ;
	fifostr_iter operator ++ () noex ; /* pre */
	fifostr_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct fifostr_iter) */
struct fifostr ;
struct fifostr_co {
	fifostr		*op = nullptr ;
	int		w = -1 ;
	void operator () (fifostr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (fifostr_co) */
struct fifostr : fifostr_head {
	fifostr_co	start ;
	fifostr_co	headlen ;
	fifostr_co	count ;
	fifostr_co	finish ;
	fifostr() noex {
	    start(this,fifostrmem_start) ;
	    headlen(this,fifostrmem_headlen) ;
	    count(this,fifostrmem_count) ;
	    finish(this,fifostrmem_finish) ;
	} ;
	fifostr(const fifostr &) = delete ;
	fifostr &operator = (const fifostr &) = delete ;
	int add(cchar *,int = -1) noex ;
	int headread(char *,int) noex ;
	int entread(har *,int,int) noex ;
	int entlen(int) noex ;
	int rem(char *,int) noex ;
	int curbegin(fifostr_cur *) noex ;
	int curend(fifostr_cur *) noex ;
	int curenum(fifostr_cur *,char *,int) noex ;
	int curdel(fifostr_cur *) noex ;
	fifostr_iter begin() noex {
	    fifostr_iter		it(va,0,i) ;
	    return it ;
	} ;
	fifostr_iter end() noex {
	    fifostr_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	~fifostr() noex {
	    dtor() ;
	} ;
} ; /* end struct (fifostr) */
#else	/* __cplusplus */
typedef FIFOSTR		fifostr ;
#endif /* __cplusplus */

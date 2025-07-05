#ifdef	__cplusplus
enum keyvalsmems {
    	keyvalsmem_addcspath,
	keyvalsmem_count,
	keyvalsmem_delall,
	keyvalsmem_strsize,
	keyvalsmem_recsize,
	keyvalsmem_cksize,
	keyvalsmem_audit,
	keyvalsmem_finish,
	keyvalsmem_overlast
} ;
struct keyvals_iter {
	cchar		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	keyvals_iter() = default ;
	keyvals_iter(cchar **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	keyvals_iter(const keyvals_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	keyvals_iter &operator = (const keyvals_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const keyvals_iter &) noex ;
	bool operator == (const keyvals_iter &) noex ;
	cchar *operator * () noex {
	    cchar	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	keyvals_iter operator + (int) const noex ;
	keyvals_iter operator += (int) noex ;
	keyvals_iter operator ++ () noex ; /* pre */
	keyvals_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct keyvals_iter) */
struct keyvals ;
struct keyvals_co {
	keyvals		*op = nullptr ;
	int		w = -1 ;
	void operator () (keyvals *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (keyvals_co) */
struct keyvals : keyvals_head {
	keyvals_co	addcspath ;
	keyvals_co	count ;
	keyvals_co	delall ;
	keyvals_co	strsize ;
	keyvals_co	recsize ;
	keyvals_co	cksize ;
	keyvals_co	audit ;
	keyvals_co	finish ;
	keyvals() noex {
	    addcspath(this,keyvalsmem_addcspath) ;
	    count(this,keyvalsmem_count) ;
	    delall(this,keyvalsmem_delall) ;
	    strsize(this,keyvalsmem_strsize) ;
	    recsize(this,keyvalsmem_recsize) ;
	    cksize(this,keyvalsmem_cksize) ;
	    audit(this,keyvalsmem_audit) ;
	    finish(this,keyvalsmem_finish) ;
	} ;
	keyvals(const keyvals &) = delete ;
	keyvals &operator = (const keyvals &) = delete ;
	int start(int = 0,int = 0) noex ;
	int add(cchar *,int = -1) noex ;
	int adduniq(cchar *,int = -1) noex ;
	int addsyms(cchar *,mainv) noex ;
	int addpath(cchar *,int = -1) noex ;
	int insert(int,cchar *,int = -1) noex ;
	int store(cchar *,int,cchar **) noex ;
	int get(int,cchar **) noex ;
	int getlast(cchar **) noex ;
	int getvec(mainv *) noex ;
	int envadd(cchar *,cchar *,int = -1) noex ;
	int envset(cchar *,cchar *,int = -1) noex ;
	int envfile(cchar *) noex ;
	int find(cchar *) noex ;
	int findn(cchar *,int = -1) noex ;
	int search(cchar *,keyvals_f,cchar ** = nullptr) noex ;
	int finder(cchar *,keyvals_f,cchar ** = nullptr) noex ;
	int del(int = -1) noex ;
	operator int () noex ;
	keyvals_iter begin() noex {
	    keyvals_iter		it(va,0,i) ;
	    return it ;
	} ;
	keyvals_iter end() noex {
	    keyvals_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct keyvals() {
	    dtor() ;
	} ;
} ; /* end struct (keyvals) */
#else	/* __cplusplus */
typedef VECSTR		keyvals ;
#endif /* __cplusplus */


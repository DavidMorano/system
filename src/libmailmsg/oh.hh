#ifdef	__cplusplus
enum comparsemems {
	comparsemem_finish,
	comparsemem_overlast
} ;
struct comparse ;
struct comparse_co {
	comparse	*op = nullptr ;
	int		w = -1 ;
	void operator () (comparse *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (comparse_co) */
struct comparse : comparse_head {
	comparse_co	finish ;
	comparse() noex {
	    finish(this,comparsemem_finish) ;
	} ;
	comparse(const comparse &) = delete ;
	comparse &operator = (const comparse &) = delete ;
	int start(int = 0,int = 0) noex ;
	void dtor() noex ;
	~comparse() {
	    dtor() ;
	} ;
} ; /* end struct (comparse) */
#else	/* __cplusplus */
typedef COMPARSE	comparse ;
#endif /* __cplusplus */


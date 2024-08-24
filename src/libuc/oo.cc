#ifdef	__cplusplus
enum ptmamems {
	ptmamem_create,
	ptmamem_destroy,
	ptmamem_lockbegin,
	ptmamem_lockend,
	ptmamem_overlast
} ;
struct ptma ;
struct ptma_creater {
	ptma		*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (ptma *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex {
	    return (*this)() ;
	} ;
	int operator () (ptmaa * = nullptr) noex ;
} ; /* end struct (ptma_creater) */
struct ptma_co {
        ptma             *op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ptma *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex {
            return (*this)() ;
        } ;
        int operator () (int = -1) noex ;
} ; /* end struct (ptma_co) */
struct ptma : pthread_mutexattr_t {
	ptma_creater	create ;
	ptma_co		destroy ;
	ptma_co		lockbegin ;
	ptma_co		lockend ;
	constexpr ptma() noex {
	    create(this,ptmamem_create) ;
	    destroy(this,ptmamem_destroy) ;
	    lockbegin(this,ptmamem_lockbegin) ;
	    lockend(this,ptmamem_lockend) ;
	} ;
	ptma(const ptma &) = delete ;
	ptma &operator = (const ptma &) = delete ;
	void dtor() noex ;
	~ptma() noex {
	    dtor() ;
	} ; /* end dtor (ptma) */
} ; /* end class (ptma) */
#else
typedef PTMA	ptma ;
#endif /* __cplusplus */


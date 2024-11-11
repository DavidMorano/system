#ifdef	__cplusplus
enum vechandmems {
	vechandmem_count,
	vechandmem_setsorted,
	vechandmem_issorted,
	vechandmem_delall,
	vechandmem_extent,
	vechandmem_audit,
	vechandmem_finish,
	vechandmem_overlast
} ;
struct vechand ;
struct vechand_co {
	vechand		*op = nullptr ;
	int		w = -1 ;
	void operator () (vechand *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vechand_co) */
struct vechand : vechand_head {
	vechand_co	count ;
	vechand_co	setsported ;
	vechand_co	issorted ;
	vechand_co	delall ;
	vechand_co	extent ;
	vechand_co	audit ;
	vechand_co	finish ;
	vechand() noex {
	    count(this,vechandmem_count) ;
	    setsorted(this,vechandmem_setsorted) ;
	    issorted(this,vechandmem_issorted) ;
	    delall(this,vechandmem_delall) ;
	    extent(this,vechandmem_extent) ;
	    audit(this,vechandmem_audit) ;
	    finish(this,vechandmem_finish) ;
	} ;
	vechand(const vechand &) = delete ;
	vechand &operator = (const vechand &) = delete ;
	int start(int = 0,int = 0) noex ;
	int add(cvoid *) noex ;
	int sort(int,vechand_f = nullptr) noex ;
	int get(int,void **) noex ;
	int getlast(int,void **) noex ;
	int getvec(void *) noex ;
	int search(cvoid *,vechand_f,void **) noex ;
	int del(int = -1) noex ;
	int delhand(cvoid *) noex ;
	vechand_iter begin() noex {
	    vechand_iter		it(va,0,i) ;
	    return it ;
	} ;
	vechand_iter end() noex {
	    vechand_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	~vechand() noex {
	    dtor() ;
	} ;
} ; /* end struct (vechand) */
#else	/* __cplusplus */
typedef VECHAND		vechand ;
#endif /* __cplusplus */

typedef	VECHAND_FL	vechand_fl ;


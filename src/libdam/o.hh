#ifdef	__cplusplus
enum outlinemems {
	outlinemem_start,
	outlinemem_count,
	outlinemem_incr,
	outlinemem_finish,
	outlinemem_overlast
} ;
struct outline ;
struct outline_co {
	outline		*op = nullptr ;
	int		w = -1 ;
	void operator () (outline *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (outline_co) */
struct outline : outline_head {
	outline_co	start ;
	outline_co	count ;
	outline_co	incr ;
	outline_co	finish ;
	outline() noex {
	    start(this,outlinemem_start) ;
	    count(this,outlinemem_count) ;
	    incr(this,outlinemem_incr) ;
	    finish(this,outlinemem_finish) ;
	} ;
	outline(const outline &) = delete ;
	outline &operator = (const outline &) = delete ;
	void dtor() noex ;
	operator int () noex ;
	destruct outline() {
	    if (dbuf) dtor() ;
	} ;
} ; /* end struct (outline) */
#else	/* __cplusplus */
typedef OUTLINE		outline ;
#endif /* __cplusplus */


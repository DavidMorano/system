#ifdef	__cplusplus
enum outlinemems {
	outlinemem_start,
	outlinemem_clear,
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
	outline_co	clear ;
	outline_co	finish ;
	outline() noex {
	    start(this,outlinemem_start) ;
	    clear(this,outlinemem_clear) ;
	    finish(this,outlinemem_finish) ;
	    dbuf = nullptr ;
	} ;
	outline(const outline &) = delete ;
	outline &operator = (const outline &) = delete ;
	int strw(cchar *,int = -1) noex ;
	int str(cchar *,int = -1) noex ;
	int get(cchar **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct outline() {
	    if (dbuf) dtor() ;
	} ;
} ; /* end struct (outline) */
#else	/* __cplusplus */
typedef OUTLINE		outline ;
#endif /* __cplusplus */


#ifdef	__cplusplus
enum zdbmems {
	zdbmem_count,
	zdbmem_overlast
} ;
struct zdb ;
struct zdb_co {
	zdb		*op = nullptr ;
	int		w = -1 ;
	void operator () (zdb *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (zdb_co) */
struct zdb : zdb_head {
	zdb_co	count ;
	zdb() noex {
	    count(this,zdbmem_count) ;
	} ;
	zdb(const zdb &) = delete ;
	zdb &operator = (const zdb &) = delete ;
	int start(time_t) noex ;
	int load(time_t *,cchar *,int = -1) noex ;
} ; /* end struct (zdb) */
#else	/* __cplusplus */
typedef ZDB		zdb ;
#endif /* __cplusplus */


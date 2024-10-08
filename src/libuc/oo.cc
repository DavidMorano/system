#ifdef	__cplusplus
enum filemapmems {
	filemapmem_rewind,
	filemapmem_finish,
	filemapmem_overlast
} ;
struct filemap ;
struct filemap_co {
	filemap		*op = nullptr ;
	int		w = -1 ;
	void operator () (filemap *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (filemap_co) */
struct filemap : filemap_head {
	filemap_co	rewind ;
	filemap_co	finish ;
	filemap() noex {
	    rewind(this,filemapmem_rewind) ;
	    finish(this,filemapmem_finish) ;
	} ;
	filemap(const filemap &) = delete ;
	filemap &operator = (const filemap &) = delete ;
	int open(cchar *,size_t = 0uz) noex ;
	int stat(USTAT *) noex ;
	int read(int,void *) noex ;
	int getln(cchar **) noex ;
	int seek(off_t,int = 0) noex ;
	int tell(off_t * = nullptr) noex ;
	void dtor() noex ;
	~filemap() noex {
	    dtor() ;
	} ;
} ; /* end struct (filemap) */
#else	/* __cplusplus */
typedef FILEMAP		filemap ;
#endif /* __cplusplus */

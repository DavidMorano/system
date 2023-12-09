/* libuc INCLUDE */
/* lang=C20 */


#ifndef	LIBUC_INCLUDE
#define	LIBUC_INCLUDE


#ifndef	UCMALLOC_INCLUDE
#define	UCMALLOC_INCLUDE

#ifdef	__cplusplus
extern "C" {
#endif

extern int uc_malloc(int,void *) noex ;
extern int uc_mallocstrw(const char *,int,const char **) noex ;
extern int uc_mallocbuf(const void *,int,const void **) noex ;

#ifdef	__cplusplus
}
#endif

#endif /* UCMALLOC_INCLUDE */


#endif /* LIBUC_INCLUDE */




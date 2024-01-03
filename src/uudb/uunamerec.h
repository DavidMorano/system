/* uunamerec HEADER */
/* lang=C20 */

/* string uunamerec object */
/* version %I% last-modified %G% */


#ifndef	UUNAMEREC_INCLUDE
#define	UUNAMEREC_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	UUNAMEREC			struct uunamerec_head


struct uunamerec_head {
	int	*rectab ;	/* an entry is a single unsigned int */
	int	i ;		/* current length (including last NULL) */
	int	e ;		/* current buffer extent */
	int	c ;		/* count */
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	uunamerec_init(UUNAMEREC *,int) ;
extern int	uunamerec_free(UUNAMEREC *) ;
extern int	uunamerec_add(UUNAMEREC *,int) ;
extern int	uunamerec_already(UUNAMEREC *,int) ;
extern int	uunamerec_count(UUNAMEREC *) ;
extern int	uunamerec_gettab(UUNAMEREC *,uint **) ;
extern int	uunamerec_indexlen(UUNAMEREC *) ;
extern int	uunamerec_indexsize(UUNAMEREC *) ;
extern int	uunamerec_mkindex(UUNAMEREC *,int [][2],int) ;

#ifdef	__cplusplus
}
#endif


#endif /* UUNAMEREC_INCLUDE */



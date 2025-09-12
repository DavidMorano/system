/* recsvc HEADER */



#ifndef	RECSVC_INCLUDE
#define	RECSVC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	RECSVC			struct recsvc_head
#define	RECSVC_ENT		struct recsvc_entry
#define	RECSVC_STARTNUM	100	/* starting number records */
/* branch types */
#define	RECSVC_BTFWD		1	/* forward */
#define	RECSVC_BTSSH		2	/* Simple Single-sided Hammock */


struct recsvc_entry {
	uint	canon ;		/* cannonical name */
	uint	name ;		/* service name */
	uint	svc ;		/* service code */
} ;

struct recsvc_head {
	RECSVC_ENT	*rectab ;
	uint		magic ;
	int	i ;		/* current length */
	int	e ;		/* current buffer extent */
	int	c ;		/* count */
} ;

EXTERNC_begin

extern int	recsvc_init(recsvc *,int) noex ;
extern int	recsvc_free(recsvc *) noex ;
extern int	recsvc_add(recsvc *,uint,int,int) noex ;
extern int	recsvc_already(recsvc *,uint,int,int) noex ;
extern int	recsvc_gettab(recsvc *,uint **) noex ;
extern int	recsvc_rtlen(recsvc *) noex ;
extern int	recsvc_count(recsvc *) noex ;
extern int	recsvc_countindex(recsvc *) noex ;
extern int	recsvc_sizeindex(recsvc *) noex ;
extern int	recsvc_mkindex(recsvc *,uint [][2],int) noex ;

EXTERNC_end


#endif /* RECSVC_INCLUDE */



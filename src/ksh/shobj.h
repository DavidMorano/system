/* shobj HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	SHOBJ_INCLUDE
#define	SHOBJ_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	SHOBJ		struct shobj_head
#define	SHOBJ_CA	struct shobj_calls


struct shobj_module {
	void		*handle ;
} ;

struct shobj_calls {
	int		(*init)() ;
	int		(*check)() ;
	int		(*free)() ;
} ;

struct shobj_ent {
	void		*op ;
	SHOBJ_CALLS	c ;
	int		osize ;
} ;

struct shobj_head {
	vecobj		entries ;
	uint		magic ;
} ;

typedef	SHOBJ		shobj ;
typedef	SHOBJ_CA	shobj_ca ;


#endif /* SHOBJ_INCLUDE */



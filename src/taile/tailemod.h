/* tailemod HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	TAILEMOD_INCLUDE
#define	TAILEMOD_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<vecstr.h>


#define	TAILEMOD_MAGIC	31815927
#define	TAILEMOD	struct tailemod_head
#define	TAILEMOD_CALLS	struct tailemod_calls
#define	TAILEMOD_ENT	struct tailemod_ent
#define	TAILEMOD_MODULE	struct tailemod_module
#define	TAILEMOD_INFO	struct tailemod_i
/* module flags */
#define	TAILEMOD_MFULL		0x0001
#define	TAILEMOD_MHALFOUT	0x0002
#define	TAILEMOD_MHALFIN	0x0004
#define	TAILEMOD_MCOR		0x0008
#define	TAILEMOD_MCO		0x0010
#define	TAILEMOD_MCL		0x0020
#define	TAILEMOD_MARGS		0x0040


struct tailemod_flags {
	uint		vsprs:1 ;
	uint		vsdirs:1 ;
} ;

struct tailemod_head {
	mainv		dirs ;
	char		*pr ;
	vecobj		modules ;		/* shared objects */
	vecobj		entries ;		/* name entries */
	vecstr		dirlist ;
	TAILEMOD_FL	f ;
	uint		magic ;
} ;

struct tailemod_calls {
	int	(*init)() ;
	int	(*store)() ;
	int	(*check)() ;
	int	(*getline)() ;
	int	(*summary)() ;
	int	(*finish)() ;
	int	(*free)() ;
} ;

struct tailemod_module {
	void		*dhp ;
	ino_t		ino ;
	dev_t		dev ;
	int		count ;
} ;

struct tailemod_ent {
	cchar		*name ;
	struct tailemod_module	*mp ;
	struct tailemod_calls	c ;
	int		size ;		/* object size */
	int		flags ;
} ;

struct tailemod_i {
	cchar		*name ;		/* module name */
	int		size ;		/* module object size */
	int		flags ;		/* module flags */
} ;

EXTERNC_begin

extern int tailemod_start(TAILEMOD *,cchar *,cchar **) noex ;
extern int tailemod_load(TAILEMOD *,cchar *) noex ;
extern int tailemod_unload(TAILEMOD *,cchar *) noex ;
extern int tailemod_store(TAILEMOD *,cchar *,int) noex ;
extern int tailemod_check(TAILEMOD *,time_t) noex ;
extern int tailemod_getline(TAILEMOD *,char *,int) noex ;
extern int tailemod_summary(TAILEMOD *,char *,int) noex ;
extern int tailemod_finish(TAILEMOD *,char *,int) noex ;
extern int tailemod_finish(TAILEMOD *) noex ;

EXTERNC_end


#endif /* TAILEMOD_INCLUDE */



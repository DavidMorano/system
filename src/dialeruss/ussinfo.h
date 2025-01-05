/* ussinfo HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */


#ifndef	USSINFO_INCLUDE
#define	USSINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<expcook.h>
#include	<userinfo.h>
#include	<keyopt.h>

#include	"sysdialer.h"


#define	USSINFO		struct ussinfo_head
#define	USSINFO_FL	struct ussinfo_flags
#define	USSINFO_A	struct ussinfo_allocs


struct ussinfo_flags {
	uint		stores:1 ;
	uint		progdash:1 ;
	uint		ids:1 ;
	uint		userinfo:1 ;
	uint		ignore:1 ;
	uint		log:1 ;
} ;

struct ussinfo_allocs {
	cchar		*node ;
	cchar		*svc ;
	cchar		*pr ;
	cchar		*portspec ;
} ;

struct ussinfo_head {
	cchar		**argv ;
	cchar		**envv ;
	cchar		*pr ;
	cchar		*prn ;
	cchar		*searchname ;
	cchar		*version ;
	cchar		*afspec ;
	cchar		*hostname ;
	cchar		*portspec ;
	cchar		*svcname ;
	cchar		*pvfname ;
	cchar		*dfname ;
	cchar		*xfname ;
	cchar		*efname ;
	cchar		*architecture ;		/* machine architecture */
	cchar		*umachine ;		/* UNAME machine name */
	cchar		*usysname ;		/* UNAME OS system-name */
	cchar		*urelease ;		/* UNAME OS release */
	cchar		*uversion ;		/* UNAME OS version */
	cchar		*hz ;			/* OS HZ */
	cchar		*nodename ;		/* USERINFO */
	cchar		*domainname ;		/* USERINFO */
	cchar		*username ;		/* USERINFO */
	cchar		*homedname ;		/* USERINFO */
	cchar		*shell ;		/* USERINFO */
	cchar		*organization ;		/* USERINFO */
	cchar		*gecosname ; 		/* USERINFO */
	cchar		*realname ;		/* USERINFO */
	cchar		*name ;			/* USERINFO */
	cchar		*tz ;			/* USERINFO */
	cchar		*groupname ;
	cchar		*tmpdname ;
	cchar		*maildname ;
	cchar		*hfname ;
	cchar		*lfname ;
	cchar		*paramfname ;
	cchar		*logid ;
	cchar		*defprog ;
	void		*op ;
	SYSDIALER_ARGS	*ap ;
	ids		id ;
	vecstr		aenvs ;
	vecstr		stores ;
	userinfo	u ;
	USSINFO_A	a ;
	USSINFO_FL	f, init, open ;
	uid_t		uid ;
	gid_t		gid ;
	int		argc ;
	int		argi ;
	int		ncpu ;
	int		af ;
	int		to ;
} ;

typedef	USSIFO		ussifo ;

EXTERNC_begin

extern int	ussinfo_start(ussinfo *,void *,SYSDIALER_INFO *,
			SYSDIALER_ARGS *,cchar *,cchar *) noex ;
extern int	ussinfo_finish(ussinfo *) noex ;
extern int	ussinfo_procargs(ussinfo *) noex ;
extern int	ussinfo_procopts(ussinfo *,keyopt *) noex ;
extern int	ussinfo_defaults(ussinfo *) noex ;
extern int	ussinfo_addrparse(ussinfo *) noex ;
extern int	ussinfo_logfile(ussinfo *) noex ;

EXTERNC_end


#endif /* USSINFO_INCLUDE */



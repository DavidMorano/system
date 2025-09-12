/* smservices HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	SMSERVICES_INCLUDE
#define	SMSERVICES_INCLUDE

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecelem.h>


#define	SMSERVICES		struct smservices_head
#define	SMSERVICES_CUR		struct smservices_cursor
#define	SMSERVICES_ENT		struct smservices_entry
#define	SMSERVICES_SVCLEN	32


struct smservices_head {
	char		*fname ;
	vecelem		entries ;
	uint		magic ;
} ;

struct smservices_enrty {
	uint		svc ;
	char		canonname[SMSERVICES_SVCLEN + 1] ;
	char		svcname[SMSERVICES_SVCLEN + 1] ;
} ;

struct smservices_cursor {
	int		i ;
} ;

typedef	SMSERVICES	smservices ;
typedef	SMSERVICES_CUR	smservices_cur ;
typedef	SMSERVICES_ENT	smservices_ent ;

EXTERNC_begin

extern int smservices_open(SMSERVICES *,char *) noex ;
extern int smservices_close(SMSERVICES *) noex ;
extern int smservices_curbegin(SMSERVICES *,SMSERVICES_CUR *) noex ;
extern int smservices_curend(SMSERVICES *,SMSERVICES_CUR *) noex ;
extern int smservices_fetch(SMSERVICES *,SMSERVICES_CUR *,
		SMSERVICES_ENT *) noex ;

EXTERNC_end


#endif /* SMSERVICES_INCLUDE */



/* initinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	INITINFO_INCLUDE
#define	INITINFO_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	INITINFO	struct initinfo_head
#define	INITINFO_FL	struct initinfo_flags
#define	INITINFO_CUR	struct initinfo_cursor


struct initinfo_cursor {
	int	i ;
} ;

struct initinfo_flags {
	uint	init : 1 ;
} ;

struct initinfo_head {
	cchar		*pr ;
	vecstr		entries ;
	time_t		ti_check ;
	time_t		ti_pread ;
	INITINFO_FL	fl ;
	uint		magic ;
} ;

typedef	INITINFO	initinfo ;
typedef	INITINFO_FL	initinfo_fl ;
typedef	INITINFO_CUR	initinfo_cur ;

EXTERNC_begin

extern int	initinfo_open(initinfo *,cchar *) noex ;
extern int	initinfo_curbegin(initinfo *,initinfo_cur *) noex ;
extern int	initinfo_curend(initinfo *,initinfo_cur *) noex ;
extern int	initinfo_query(initinfo *,cchar *,char *,int) noex ;
extern int	initinfo_enum(initinfo *,initinfo_cur *,
			char *,int,char *,int) noex ;
extern int	initinfo_close(initinfo *) noex ;

EXTERNC_end


#endif /* INITINFO_INCLUDE */




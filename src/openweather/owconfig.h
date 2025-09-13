/* owconfig HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	OWCONFIG_INCLUDE
#define	OWCONFIG_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<paramfile.h>
#include	<expcook.h>

#include	"ow.h"


#define	OWCONFIG	struct owconfig_head
#define	OWCONFIG_FL	struct owconfig_flags


struct owconfig_flags {
	uint		p:1 ;
	uint		lockinfo:1 ;
} ;

struct owconfig_head {
	OW		*lip ;
	cchar		*cfname ;
	paramfile	p ;
	expcool		cooks ;
	OWCONFIG_FL	fl ;
} ; /* end struct (owconfig_head) */

typedef	OWCONFIG	owconfig ;
typedef	OWCONFIG_FL	owconfig_fl ;

EXTERNC_begin

extern int owconfig_start(OWCONFIG *,OW *,cchar *) noex ;
extern int owconfig_check(OWCONFIG *) noex ;
extern int owconfig_read(OWCONFIG *) noex ;
extern int owconfig_finish(OWCONFIG *) noex ;

EXTERNC_end


#endif /* OWCONFIG */



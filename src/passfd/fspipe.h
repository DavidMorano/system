/* fspipe HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	FSPIPE_INCLUDE
#define	FSPIPE_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecelem.h>


#define	FSPIPE_MAGIC		0x98712365
#define	FSPIPE			struct fspipe_head
#define	FSPIPE_FL		struct fspipe_flags
#define	FSPIPE_PA		union fspipe_param
#define	FSPIPE_ENT		struct fspipe_entry

#define	FSPIPE_VERSION		0
#define	FSPIPE_TRANSLEN		32
#define	FSPIPE_PROTOLEN		32
#define	FSPIPE_MAGICSTR		"FSPIPE"
#define	FSPIPE_MAGICSTRLEN	6


struct fspipe_flags {
	uint		listen:1 ;	/* listen mode */
} ;

struct fspipe_tunix {
	char		*nodename ;
	char		*filepath ;
} ;

struct fspipe_tinet {
	char		*hostname ;
	char		*portspec ;
} ;

struct fspipe_tlocal {
	char		*nodename ;
	char		*filepath ;	/* server pipe or FIFO */
	char		*file2path ;	/* client FIFO if used */
} ;

union fspipe_param {
	struct fspipe_tlocal	t_local ;
	struct fspipe_tunix	t_unix ;
	struct fspipe_tinet	t_inet ;
} ;

struct fspipe_entry {
	FSPIPE_PARAM	p ;
	int		type ;
	int		fd ;
	char		transport[FSPIPE_TRANSLEN + 1] ;
	char		protocol[FSPIPE_PROTOLEN + 1] ;
} ;

struct fspipe_head {
	vecelem		e ;
	FSPIPE_FL	fl ;
	uint		magic ;
	int		version ;
	int		fd ;
} ;

typedef	FSPIPE		fspipe ;
typedef	FSPIPE_FL	fspipe_fl ;
typedef	FSPIPE_PA	fspipe_pa ;
typedef	FSPIPE_ENT	fspipe_ent ;

#endif /* FSPIPE_INCLUDE */



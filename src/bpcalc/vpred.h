/* vpred HEADER */
/* lang=C20 */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

#ifndef	VPRED_INCLUDE
#define	VPRED_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	VPRED			struct vpred_head
#define	VPRED_ENT		struct vpred_ent
#define	VPRED_STATS		struct vpred_stats

/* more important defines */
#define	VPRED_NOPS		5	/* number of operands */
#define	VPRED_COUNTBITS		2	/* counter bits */


struct vpred_stats {
	ULONG	in_lu, in_hit ;
	ULONG	in_up, in_update, in_replace ;
	ULONG	op_lu, op_hit ;
	ULONG	op_up, op_update, op_replace ;
	uint	tablen ;
} ;

struct vpred_operand {
	int	last ;
	int	stride ;
	uint	counter : VPRED_COUNTBITS ;
} ;

struct vpred_ent {
	struct vpred_operand	ops[VPRED_NOPS] ;
	uint	tag ;
	uint	hits, replaces ;
	uint	f_valid : 1 ;
} ;

struct vpred_head {
	struct vpred_ent	*table ;
	struct vpred_stats	s ;		/* statistics */
	uint			magic ;
	uint			tablen ;
	uint			tagshift ;
	uint			ncount ;	/* number of states */
	uint			stridemask ;
	int			nops ;
} ;

EXTERNC_begin

extern int	vpred_init(VPRED *,int,int,int) ;
extern int	vpred_lookup(VPRED *,uint,uint *,int) ;
extern int	vpred_update(VPRED *,uint,uint *,int) ;
extern int	vpred_zerostats(VPRED *) ;
extern int	vpred_stats(VPRED *,VPRED_STATS *) ;
extern int	vpred_free(VPRED *) ;

EXTERNC_end


#endif /* VPRED_INCLUDE */



/* fdt HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	FDT_INCLUDE
#define	FDT_INCLUDE


#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<sys/types.h>
#include	<vecobj.h>

#include	"ucb.h"


#define	FDT		struct fdt_head
#define	FDT_ENT		struct fdt_ent
#define	FDT_MAGIC	0x77336556


struct fdt_ent {
	int		fd ;
	UCB		ucbp ;
} ;

struct fdt_head {
	ulong		magic ;
	vecobj		entries ;
} ;

typedef FDT		fdt ;
typedef FDT_ENT		fdt_ent ;

EXTERNC_begin

extern int	fdt_alloc(FDT *,int,UCB **) noex ;
extern int	fdt_get(FDT *,int,UCB **) noex ;
extern int	fdt_free(FDT *,int) noex ;
extern int	fdt_getentry(FDT *,int,FDT_ENT *) noex ;

EXTERNC_end


#endif /* FDT_INCLUDE */




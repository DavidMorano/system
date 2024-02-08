/* memtrack HEADER */
/* lang=C++20 */

/* track memory chunks */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This subroutine was originally written. This is a sort of
	test to replace the previous memory tracking implementation
	inside of the |ucmemalloc(3uc)| facility (so loved).

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	memtrack::start
	memtrack::count
	memtrack::finish
	memtrack::ins
	memtrack::rem
	memtrack::present
	memtrack::get
	memtrack::istart
	memtrack::icount
	memtrack::ifinish

	Description:
	Reack memory blocks.

*******************************************************************************/

#ifndef	MEMTRACK_INCLUDE
#define	MEMTRACK_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdint>
#include	<usysdefs.h>
#include	<clanguage.h>
#include	<mapblock.hh>


#define	MEMTRACK_MAGIC	0x65821293


enum memtrackmems {
	memtrackmem_count,
	memtrackmem_finish,
	memtrackmem_overlast
} ;

struct memtrack_ent {
	cvoid		*addr ;
	int		size ;
} ;

struct memtrack ;

struct memtrack_co {
	memtrack	*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (memtrack *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (memtrack_co) */

struct memtrack_start {
	memtrack	*op = nullptr ;
	constexpr void operator () (memtrack *p) noex {
	    op = p ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex ;
} ; /* end struct (memtrack_start) */

struct memtrack {
	typedef memtrack_ent	ent ;
	mapblock<uintptr_t,memtrack_ent>	t ; /* <- the "tracker" */
	memtrack_start	start ;
	memtrack_co	count ;
	memtrack_co	finish ;
	uint		magic = 0 ;
	constexpr memtrack() noex {
	    start(this) ;
	    count(this,memtrackmem_count) ;
	    finish(this,memtrackmem_finish) ;
	} ; /* end ctor */
	memtrack(const memtrack &) = delete ;
	memtrack &operator = (const memtrack &) = delete ;
	int	ins(cvoid *,int) noex ;
	int	rem(cvoid *) noex ;
	int	present(cvoid *) noex ;
	int	get(cvoid *,ent *) noex ;
	int	istart(int = 0) noex ;
	int	icount() noex ;
	int	ifinish() noex ;
	void	dtor() noex ;
	~memtrack() noex {
	    dtor() ;
	} ; /* end if (dtor) */
} ; /* end struct (memtrack) */


#endif	/* __cplusplus */
#endif /* MEMTRACK_INCLUDE */



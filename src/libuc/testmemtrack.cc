/* testlambda */
/* C++20 */

#include	<envstandards.h>
#include	<iostream>
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"memtrack.hh"


/* local defines */


/* local namespaces */

using namespace	std ;


/* external subroutines */


/* global variables */


/* local variables */

static const memtrack_ent	ents[] = {
	{ (void *) 0x1, 100 },
	{ (void *) 0x2, 200 },
	{ (void *) 0x0, 0 }
} ;


/* exported subroutines */

int main(int,const char **,const char **) {
	memtrack	t ;
	int		rs ;
	int		rs1 ;
	cout << "ent\n" ;
	if ((rs = t.start) >= 0) {
	    for (int i = 0 ; (rs >= 0) && ents[i].addr ; i += 1) {
		cvoid	*addr = ents[i].addr ;
		rs = t.ins(addr,ents[i].size) ;
	    } /* end for */
	    if (rs >= 0) {
		cout << "present phase\n" ;
	        for (int i = 0 ; (rs >= 0) && ents[i].addr ; i += 1) {
		    const uintptr_t	a = uintptr_t(ents[i].addr) ;
		    cvoid		*addr = ents[i].addr ;
		    rs = t.present(addr) ;
		    cout << "addr=" << a << " rs=" << rs << "\n" ;
		} /* end for */
		cout << "present phase rs=" << rs << "\n" ;
		if (rs >= 0) {
	            for (int i = 0 ; (rs >= 0) && ents[i].addr ; i += 1) {
		        const uintptr_t	ea = uintptr_t(ents[i].addr) ;
		        cvoid		*addr = ents[i].addr ;
			memtrack_ent	e ;
		        if ((rs = t.get(addr,&e)) >= 0) {
			    const uintptr_t	a = uintptr_t(e.addr) ;
			    if (a == ea) {
		                cout << "a=" << a << " s=" << e.size << "\n" ;
			    }
			}
		    } /* end for */
		    cout << "get phase rs=" << rs << "\n" ;
		    if (rs >= 0) {
			cvoid	*addr = (cvoid *) 0x01 ;
			if ((rs = t.rem(addr)) >= 0) {
		    	    const uintptr_t	a = uintptr_t(addr) ;
			    rs1 = t.present(addr) ;
		            cout << "addr=" << a << " rs=" << rs1 << "\n" ;
			}
		    }
		} /* end if (get phase) */
	    } /* end if (present phase) */
	    rs1 = t.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end block */
	cout << "exiting rs=" << rs << '\n' ;
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (main) */


/* local subroutines */



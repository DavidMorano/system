/* mapblock HEADER */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This subroutine was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

#ifndef	MAPBLOCK_INCLUDE
#define	MAPBLOCK_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<new>
#include	<utility>
#include	<unordered_map>
#include	<functional>
#include	<usysrets.h>
#include	<clanguage.h>


template<typename K,typename Block>
class mapblock {
	typedef typename std::unordered_map<K,Block>	maptype ;
	std::unordered_map<K,Block>	*mp = nullptr ;
public:
	mapblock(const mapblock &) = delete ;
	mapblock &operator = (const mapblock &) = delete ;
	constexpr mapblock() noex { } ;
	~mapblock() noex {
	    if (mp) {
	        delete mp ;
		mp = nullptr ;
	    }
	} ; /* end dtor */
	int	start(int = 0) noex ;
	int	ins(K,const Block &) noex ;
	int	rem(K) noex ;
	int	present(K &) noex ;
	int	get(K,Block *) noex ;
	int	count() noex ;
	int	finish() noex ;
} ; /* end class (mapblock) */

template<typename K,typename Block>
int mapblock<K,Block>::start(int n) noex {
	typedef decltype(std::nothrow)	nothrow_t ;
	int		rs = SR_INVALID ;
	if (n >= 0) {
	    rs = SR_NOMEM ;
	    try {
		const nothrow_t		nt = std::nothrow ;
		const nullptr_t		np{} ;
	        if ((mp = new(nt) maptype(n)) != np) {
	            rs = SR_OK ;
	        }
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	}
	return rs ;
}

template<typename K,typename Block>
int mapblock<K,Block>::ins(K k,const Block &v) noex {
	typedef	typename maptype::iterator	iterator ;
	int		rs = SR_OK ;
	try {
	    std::pair<iterator,bool>	r = mp->insert({k,v}) ;
	    if (!r.second) rs = SR_EXISTS ;
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}

template<typename K,typename Block>
int mapblock<K,Block>::rem(K k) noex {
	typedef typename maptype::size_type	size_type ;
	int		rs = SR_OK ;
	try {
	    size_type	r = mp->erase(k) ;
	    if (!r) rs = SR_NOENT ;
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}

template<typename K,typename Block>
int mapblock<K,Block>::present(K &k) noex {
	typedef typename maptype::iterator	iterator ;
	const iterator	it_end = mp->end() ;
	iterator	it = mp->find(k) ;
	return (it != it_end) ? SR_OK : SR_NOENT ;
}

template<typename K,typename Block>
int mapblock<K,Block>::get(K k,Block *vp) noex {
	typedef typename maptype::iterator	iterator ;
	int		rs = SR_FAULT ;
	if (mp && vp) {
	    typedef typename maptype::value_type	v_t ;
	    const iterator	it_end = mp->end() ;
	    const iterator	it = mp->find(k) ;
	    rs = SR_NOENT ;
	    if (it != it_end) {
		const v_t	r = *it ;
		*vp = r.second ;
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}

template<typename K,typename Block>
int mapblock<K,Block>::count() noex {
	int		rs = SR_BADFMT ;
	if (mp) {
	    rs = mp->size() ;
	}
	return rs ;
}

template<typename K,typename Block>
int mapblock<K,Block>::finish() noex {
	int		rs = SR_BADFMT ;
	if (mp) {
	    delete mp ;
	    mp = nullptr ;
	    rs = SR_OK ;
	}
	return rs ;
}


#endif	/* __cplusplus */
#endif /* MAPBLOCK_INCLUDE */



/* mapblock HEADER */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mapblock

	Description:
	This object implements a map from a "key" type (|K|)
	to a given "block" type (|B|).

	Symopsis:
	int mapblock_start<typename K,typename B>(int n = 0) noex

	Arguments:
	n		suggested starting possible number of elements

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

#ifndef	MAPBLOCK_INCLUDE
#define	MAPBLOCK_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<new>
#include	<utility>
#include	<unordered_map>
#include	<functional>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clanguage.h>


template<typename K,typename B> class mapblock ;

template <typename K,typename B>
class mapblock_iter {
	typedef mapblock<K,B>				mb ;
	typedef typename std::unordered_map<K,B>::iterator	map_it ;
	map_it			mit ;
public:
	mapblock_iter() noex { } ;
	mapblock_iter(const mapblock_iter<K,B> &oit) noex {
	    if (this != &oit) {
	        mit = oit.mit ;
	    }
	} ;
	mapblock_iter(mapblock_iter<K,B> &&oit) noex {
	    if (this != &oit) {
	        mit = oit.mit ;
	    }
	} ;
	mapblock_iter(mb *mbp,bool fend) noex {
	    if (mbp->mp) {
	        if (fend) {
	            mit = mbp->mp->end() ;
	        } else {
	            mit = mbp->mp->begin() ;
	        }
	    }
	} ;
	mapblock_iter<K,B> &operator = (const mapblock_iter<K,B> &oit) noex {
	    if (this != &oit) {
	        mit = oit.mit ;
	    }
	    return *this ;
	} ;
	mapblock_iter<K,B> &operator = (mapblock_iter<K,B> &&oit) noex {
	    if (this != &oit) {
	        mit = oit.mit ;
	    }
	    return *this ;
	} ;
	mapblock_iter<K,B> &operator = (const mapblock_iter<K,B> *oip) noex {
	    if (this != oip) {
	        mit = oip->mit ;
	    }
	    return *this ;
	} ;
	std::pair<K,B> &operator * () const noex {
	    return *mit ;
	} ;
	mapblock_iter<K,B> &operator ++ () noex { /* pre-increment */
	    ++mit ;
	    return *this ;
	} ;
	mapblock_iter<K,B> operator ++ (int) noex { /* post-increment */
	    mapblock_iter<K,B>	tmp = *this ;
	    mit++ ;
	    return tmp ; /* returns previous PRVALUE */
	} ;
	mapblock_iter<K,B> &operator += (int inc) noex {
	    mit += inc ;
	    return *this ;
	} ;
	mapblock_iter<K,B> &operator + (int inc) const noex {
	    mapblock_iter<K,B>	tmp = *this ;
	    tmp.mit + inc ;
	    return tmp ;
	} ;
	friend bool operator == (const mapblock_iter<K,B> &i1,
		const mapblock_iter<K,B> &i2) noex {
	    return (i1.mit == i2.mit) ;
	} ;
	friend mapblock<K,B> ;
} ; /* end class (mapblock_iter) */

template<typename K,typename B>
class mapblock {
	typedef mapblock_iter<K,B>			iterator ;
	typedef typename std::unordered_map<K,B>	maptype ;
	std::unordered_map<K,B>	*mp = nullptr ;
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
	int	ins(K,const B &) noex ;
	int	rem(K) noex ;
	int	present(K &) noex ;
	int	get(K,B *) noex ;
	int	count() noex ;
	int	finish() noex ;
	iterator begin() noex {
	    iterator	it(this,false) ;
	    return it ;
	} ;
	iterator end() noex {
	    iterator	it(this,true) ;
	    return it ;
	} ;
} ; /* end class (mapblock) */

template<typename K,typename B>
int mapblock<K,B>::start(int n) noex {
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

template<typename K,typename B>
int mapblock<K,B>::ins(K k,const B &v) noex {
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

template<typename K,typename B>
int mapblock<K,B>::rem(K k) noex {
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

template<typename K,typename B>
int mapblock<K,B>::present(K &k) noex {
	typedef typename maptype::iterator	iterator ;
	const iterator	it_end = mp->end() ;
	iterator	it = mp->find(k) ;
	return (it != it_end) ? SR_OK : SR_NOENT ;
}

template<typename K,typename B>
int mapblock<K,B>::get(K k,B *vp) noex {
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

template<typename K,typename B>
int mapblock<K,B>::count() noex {
	int		rs = SR_BADFMT ;
	if (mp) {
	    rs = mp->size() ;
	}
	return rs ;
}

template<typename K,typename B>
int mapblock<K,B>::finish() noex {
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



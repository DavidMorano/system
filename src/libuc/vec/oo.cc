int vecstr::start(int an,int ao) noex {
	return vecstr_start(this,an,ao) ;
}

int vecstr::add(cchar *sp,int sl) noex {
	return vecstr_add(this,sp,sl) ;
}

int vecstr::adduniq(cchar *sp,int sl) noex {
	return vecstr_adduniq(this,sp,sl) ;
}

int vecstr::addsyms(cchar *on,mainv sv) noex {
	return vecstr_addsyms(this,on,sv) ;
}

int vecstr::addpath(cchar *sp,int sl) noex {
	return vecstr_addpath(this,sp,sl) ;
}

int vecstr::insert(int ai,cchar *sp,int sl) noex {
	return vecstr_insert(this,ai,sp,sl) ;
}

int vecstr::get(int ai,cchar **rpp) noex {
	return vecstr_get(this,ai,rpp) ;
}

int vecstr::envadd(cchar *kp,cchar *valp,int vall) noex {
	return vecstr_envadd(this,kp,valp,vall) ;
}

int vecstr::envset(cchar *kp,cchar *valp,int vall) noex {
	return vecstr_envset(this,kp,valp,vall) ;
}

int vecstr::envfile(cchar *fn) noex {
	return vecstr_envfile(this,fn) ;
}

int vecstr::getvec(mainv *rppp) noex {
	return vecstr_getvec(this,rppp) ;
}

int vecstr::del(int ai) noex {
	if (ai < 0) ai = 0 ;
	return vecstr_del(this,ai) ;
}

void vecstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecstr",rs,"fini-finish") ;
	}
}

vecstr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecstrmem_addcspath:
	        rs = vecstr_addcspath(op) ;
	        break ;
	    case vecstrmem_count:
	        rs = vecstr_count(op) ;
	        break ;
	    case vecstrmem_delall:
	        rs = vecstr_delall(op) ;
	        break ;
	    case vecstrmem_strsize:
	        rs = vecstr_strsize(op) ;
	        break ;
	    case vecstrmem_recsize:
	        rs = vecstr_recsize(op) ;
	        break ;
	    case vecstrmem_cksize:
	        rs = vecstr_cksize(op) ;
	        break ;
	    case vecstrmem_audit:
	        rs = vecstr_audit(op) ;
	        break ;
	    case vecstrmem_finish:
	        rs = vecstr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vecstr_co::operator) */


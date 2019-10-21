/*
 * iterator_zip2.h -- combine two iterators to a "zipped" iterator which
 * 	contains elements from both
 * 
 * examples:
 * std::vector<int> v1;
 * std::vector<double> v2;
 * ... // fill in vectors
 * 
 * 
 * auto itbg = make_zip_it(v1.begin(),v2.begin());
 * auto itend = make_zip_it(v1.end(),v2.end());
 * std::sort(itbg,itend,[](const auto& x, const auto& y){return x.first < y.first;});
 * 
 * // the following hold
 * itbg.first() == *(v1.begin());
 * itbg.second() == *(v2.begin());
 * 
 * // assignment
 * std::pair<int,double> val = *itbg;
 * *itbg = std::make_pair(1,2.0);
 * 
 * Copyright 2018 Daniel Kondor <kondor.dani@gmail.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


#include <iterator>
#include <type_traits>


namespace zi {

/* 1. "reference type": an std::pair storing references */
template<class T1, class T2>
struct refpair : std::pair<T1&,T2&> {
	using std::pair<T1&,T2&>::first;
	using std::pair<T1&,T2&>::second;
	
	/* convert to regular pair -- this makes a copy of the elements */
	operator std::pair<T1,T2>() const { return std::pair<T1,T2>(first,second); }
	//~ refpair() = delete;
	refpair(T1& x1, T2& x2):std::pair<T1&,T2&>(x1,x2) { }
	refpair(const refpair<T1,T2>& pair):std::pair<T1&,T2&>(pair.first,pair.second) { }
	refpair(const std::pair<T1&,T2&>& pair):std::pair<T1&,T2&>(pair.first,pair.second) { }
	
	refpair<T1,T2>& operator = (const std::pair<T1,T2>& pair) {
		first = pair.first;
		second = pair.second;
		return *this;
	}
	refpair<T1,T2>& operator = (std::pair<T1,T2>&& pair) {
		first = std::move(pair.first);
		second = std::move(pair.second);
		return *this;
	}
	
	void swap(refpair<T1,T2>& pair) {
		using std::swap;
		swap(first,pair.first);
		swap(second,pair.second);
	}
};

template<class T1,class T2>
void swap(refpair<T1,T2>&& p1, refpair<T1,T2>&& p2) {
	p1.swap(p2);
}




/* 2. iterator class -- this derives from the previous reference class */
template<class It1, class It2> /* , bool cmp_both = true> */
struct zip_it : std::iterator< typename std::iterator_traits<It1>::iterator_category,
		std::pair< typename std::iterator_traits<It1>::value_type, typename std::iterator_traits<It2>::value_type > > {
	
	protected:
		It1 it1;
		It2 it2;
	
	public:
		static_assert( std::is_same< typename std::iterator_traits<It1>::iterator_category,
			typename std::iterator_traits<It2>::iterator_category>::value ,"Iterators have to have be the same kind!\n");
		
		typedef typename std::iterator_traits<It1>::value_type T1;
		typedef typename std::iterator_traits<It2>::value_type T2;
		typedef refpair<T1,T2> reference;
		typedef std::pair<T1,T2> value_type;
		typedef ssize_t difference_type;
		
		/* constructor -- takes two iterators */
		zip_it() = delete;
		zip_it(const It1& it1_, const It2& it2_) : it1(it1_),it2(it2_) { }
		
		/* dereferencing */
		reference operator*() { return refpair<T1,T2>(*it1,*it2); }
		value_type operator*() const { return std::pair<T1,T2>(*it1,*it2); }
		/* note: operator -> cannot work in this case, there is nothing to return
		zip_ref<It1,It2,cmp_both>* operator ->() { return this; }
		* alternative solution: element access functions:
		* so instead of writing e.g. zit->first, one could use zit.first() */
		T1& first() { return *it1; }
		T2& second() { return *it2; }
		T1 first() const { return *it1; }
		T2 second() const { return *it2; }
		
		/* array access -- only works for random access iterators */
		reference operator [] (size_t i) { return refpair<T1,T2>(it1[i],it2[i]); }
		value_type operator [] (size_t i) const { return std::pair<T1,T2>(it1[i],it2[i]); }
		
		
		/* increment / decrement */
		zip_it<It1,It2> operator++(int) { auto tmp = *this; it1++; it2++; return tmp; }
		zip_it<It1,It2>& operator++() { it1++; it2++; return *this; }
		zip_it<It1,It2> operator--(int) { auto tmp = *this; it1--; it2--; return tmp; }
		zip_it<It1,It2>& operator--() { it1--; it2--; return *this; }
		zip_it<It1,It2>& operator+=(ssize_t x) { it1+=x; it2+=x; return *this; }
		zip_it<It1,It2>& operator-=(ssize_t x) { it1-=x; it2-=x; return *this; }
		zip_it<It1,It2> operator+(ssize_t x) const { auto r(*this); r += x; return r; }
		zip_it<It1,It2> operator-(ssize_t x) const { auto r(*this); r -= x; return r; }
		/* distance -- return the smallest of distance among the two iterator pairs */
		//~ template<bool cmp2>
		ssize_t operator-(const zip_it<It1,It2>& r) const {
			ssize_t d1 = it1 - r.it1;
			ssize_t d2 = it2 - r.it2;
			if(d2 < d1) d1 = d2;
			return d1;
		}
		
		/* comparisons */
		/* note: two zipped iterators are equal if either of the iterator
		 * pairs compares equal -- the motivation is that it can be used to
		 * test if we reach the end of the shorter of two ranges if not equal
		 * 
		 * similarly, all other comparisons will behave similarly, e.g. 
		 * i1 < i2 if both it1 and it2 are less than their pairs, etc.
		 * 
		 * note: this changes the logic of the comparison operators, e.g.
		 * it can be that both i1 < i2 and i2 < i1 are false or both
		 * i1 > i2 and i2 > i1 are true (but i1 < i2 iff i2 > i1)
		 * to avoid this, only compare zipped iterators where both stored
		 * iterators compare similarly (e.g. create zipped iterator pairs from
		 * ranges of the same size)
		 */
		//~ template<bool cmp2> 
		bool operator==(const zip_it<It1,It2>& r) const {
			return (it1 == r.it1 || it2 == r.it2);
		}
		//~ template<bool cmp2>
		bool operator!=(const zip_it<It1,It2>& r) const {
			return (it1 != r.it1 && it2 != r.it2);
		}
		//~ template<bool cmp2>
		bool operator< (const zip_it<It1,It2>& r) const {
			return (it1 <  r.it1 && it2 <  r.it2);
		}
		//~ template<bool cmp2>
		bool operator<=(const zip_it<It1,It2>& r) const {
			return (it1 <= r.it1 && it2 <= r.it2);
		}
		//~ template<bool cmp2>
		bool operator> (const zip_it<It1,It2>& r) const {
			return (it1 >  r.it1 || it2 >  r.it2);
		}
		//~ template<bool cmp2>
		bool operator>=(const zip_it<It1,It2>& r) const {
			return (it1 >= r.it1 || it2 >= r.it2);
		}
};


/* template function to create zip iterator deducing types automatically */
template<class It1, class It2> /*, bool cmp_both = true>*/
zip_it<It1,It2> make_zip_it(It1 it1, It2 it2) {
	return zip_it<It1,It2>(it1,it2);
}

}




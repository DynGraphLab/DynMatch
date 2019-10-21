/*
 * tsv_iterator.h -- helper class to read std::pair<unsinged int,unsigned int>
 * 	from a file and present it as an "iterator"
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
 * 
 */

#ifndef TSV_ITERATOR_H
#define TSV_ITERATOR_H

#include "read_table.h"

/* throw exception, but make sure it's displayed on stderr as well */
static inline void handle_error(const char* msg) {
	fprintf(stderr,"%s",msg);
	throw new std::runtime_error(msg);
}




/* helper class to read data from a TSV file and present it as an "iterator"
 * this way the functions creating a graph can be made more generic, accepting
 * an iterator of edges */
class tsv_iterator : std::iterator<std::input_iterator_tag, std::pair<unsigned int,unsigned int> > {
protected:
	read_table2 rt;
	uint64_t lines_max;
	uint64_t header_skip;
	bool is_end;
	bool skip_overflow; /* skip lines on overflow / underflow */
	std::pair<unsigned int,unsigned int> r;
	
	//~ tsv_iterator() { is_end = true; line = 0; lines_max = 0; }
	
	bool read_next() {
		if(lines_max && rt.get_line() >= lines_max + header_skip) { is_end = true; return false; }
		unsigned int n1,n2;
		while(true) {
			if(!rt.read_line()) { is_end = true; return false; }
			if(!rt.read(n1,n2)) {
				/* potentiall skip to next line in case of overflow */
				if(skip_overflow && rt.get_last_error() == T_OVERFLOW) continue;
				/* otherwise write error message
				 * note: this will NOT throw an exception, but the is_error()
				 * function can be used to detect this */
				fprintf(stderr,"tsv_iterator::read_next(): ");
				rt.write_error(stderr);
				is_end = true;
				return false;
			}
			break;
		}
		r = std::make_pair(n1,n2);
		return true;
	}
	
public:
	tsv_iterator(FILE* in_, uint64_t header_skip_ = 0, uint64_t lines_max_ = 0, bool skip_overflow_ = false):rt(in_) {
		header_skip = header_skip_;
		skip_overflow = skip_overflow_;
		for(uint64_t j=0;j<header_skip;j++) rt.read_line(false);
		lines_max = lines_max_;
		is_end = false;
		read_next();
	}
	tsv_iterator(read_table2& rt_, uint64_t header_skip_ = 0, uint64_t lines_max_ = 0, bool skip_overflow_ = false):rt(rt_) {
		header_skip = header_skip_;
		skip_overflow = skip_overflow_;
		for(uint64_t j=0;j<header_skip;j++) rt.read_line(false);
		lines_max = lines_max_;
		is_end = false;
		read_next();
	}
	
	/* trying to dereference an iterator after the end will throw an exception */
	const std::pair<unsigned int,unsigned int> operator *() const {
		if(is_end) handle_error("tsv_iterator(): iterator used after reaching the end!\n");
		return r;
	}
	const std::pair<unsigned int,unsigned int>* operator ->() const {
		if(is_end) handle_error("tsv_iterator(): iterator used after reaching the end!\n");
		return &r;
	}
	void operator++() {
		read_next();
	}
	
	/* note: not comparison operator; when used in loops, compare to the sentinel
	 * class below or use the following function */
	bool is_it_end() const {
		return is_end;
	}
	
	/* more conventional error checking for read and parse errors */
	bool is_error() const {
		if(rt.get_last_error() != T_EOF && rt.get_last_error() != T_OK) return true;
		return false;
	}
};

/* sentinel class to supply as end of iteration */
class tsv_iterator_sentinel {
	/* empty class just used in the comparison functions */
};

static inline bool operator==(const tsv_iterator_sentinel& s, const tsv_iterator& i) {
	return i.is_it_end(); // i == s if i is at the end (can be used in a C++ style loop)
}
static inline bool operator==(const tsv_iterator& i, const tsv_iterator_sentinel& s) {
	return i.is_it_end(); // i == s if i is at the end (can be used in a C++ style loop)
}

static inline bool operator!=(const tsv_iterator_sentinel& s, const tsv_iterator& i) {
	return !(i.is_it_end()); // i == s if i is at the end (can be used in a C++ style loop)
}
static inline bool operator!=(const tsv_iterator& i, const tsv_iterator_sentinel& s) {
	return !(i.is_it_end()); // i == s if i is at the end (can be used in a C++ style loop)
}


#endif


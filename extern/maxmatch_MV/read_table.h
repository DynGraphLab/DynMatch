/*
 * read_table.h -- simple and robust general methods for reading numeric data
 * 	from text files, e.g. TSV or CSV
 * 
 * simple: should be usable in a few lines of code
 * robust: try to detect and signal errors (in format, overflow, underflow etc.)
 * 	especially considering cases that would be silently ignored with
 * 	scanf / iostreams or similar
 * 
 * version with both C and C++ interface; it requires the POSIX C getline()
 * function which is not available on all systems (most notably on Windows)
 * 
 * note that the C++ interface requires C++11
 * 
 * Copyright 2018 Daniel Kondor <kondor.dani@gmail.com>
 * 
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

 * 
 * example usage in C:

FILE* f = ... // open input file or create pipe or use stdin
read_table* r = read_table_new(f);
while(read_table_line(r) == 0) {
	int32_t id1,id2;
	double d1;
	uint64_t id3;
	if(read_table_int32(r,&id1) || read_table_double(r,&d1) ||
		read_table_uint64(r,&id3) || read_table_int32(r,&id2) ) break;
	... // do something with the values read
}
if(read_table_get_last_error(r) != T_EOF) { // handle error
	fprintf(stderr,"Error reading input:\n");
	read_table_write_error(r,stderr);
}
read_table_free(r);
fclose(f); // close file separately -- could be pclose() or nothing if using stdin

 * 
 * example usage in C++

FILE* f = ... // open input file or create pipe or use stdin
read_table2 r(f);
while(r.read_line()) {
	int32_t id1,id2;
	double d1;
	uint64_t id3;
	if(!r.read(id1,d1,id3,id2)) break; // returning false indicates error
	... // do something with the values read
}
if(r.get_last_error() != T_EOF) { // handle error
	fprintf(stderr,"Error reading input:\n");
	r.write_error(stderr);
}
f.close(); // close file separately -- r should not be used after this point

 */

#ifndef _READ_TABLE_H
#define _READ_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

#ifdef __cplusplus
#include <cmath>
static inline bool _isnan(double x) { return std::isnan(x); }
static inline bool _isinf(double x) { return std::isinf(x); }
#else
#include <math.h>
static inline int _isnan(double x) { return isnan(x); }
static inline int _isinf(double x) { return isinf(x); }
#endif


/* possible error codes */
enum read_table_errors {T_OK = 0, T_EOF = 1, T_EOL, T_MISSING, T_FORMAT,
	T_OVERFLOW, T_NAN, T_TYPE, T_COPIED, T_ERROR_FOPEN, T_READ_ERROR};
static const char * const error_desc[] = {"No error", "End of file", "Unexpected end of line",
		"Missing value", "Invalid value", "Overflow or underflow", "NaN or infinity read",
		"Unknown conversion requested", "Invalidated instance", "Error opening file",
		"Error reading input"};

/* convert error code to string description */
static const char* get_error_desc(enum read_table_errors err) {
	const static char unkn[] = "Unknown error";
	switch(err) {
		case T_OK:
			return error_desc[0];
		case T_EOF:
			return error_desc[1];
		case T_EOL:
			return error_desc[2];
		case T_MISSING:
			return error_desc[3];
		case T_FORMAT:
			return error_desc[4];
		case T_OVERFLOW:
			return error_desc[5];
		case T_NAN:
			return error_desc[6];
		case T_TYPE:
			return error_desc[7];
		case T_COPIED:
			return error_desc[8];
		case T_ERROR_FOPEN:
			return error_desc[9];
		case T_READ_ERROR:
			return error_desc[10];
		default:
			return unkn;
	}
}

/* struct containing main parameters for processing text */
typedef struct read_table_s {
	FILE* f; /* file to be read from -- either supplied by the caller, or opened / closed in the constuctor / destructor */
	char* buf; /* buffer to hold the current line */
	const char* fn; /* file name, stored optionally for error output */
	size_t buf_size; /* size of the previous buffer */
	size_t line_len; /* length of the current line (in the buffer) */
	uint64_t line; /* current line (count starts from 1) */
	size_t pos; /* current position in line */
	size_t col; /* current field (column) */
	int base; /* base for integer conversions */
	enum read_table_errors last_error; /* error code of the last operation */
	char delim; /* delimiter to use; 0 means any blank (space or tab) note: cannot be newline */
	char comment; /* character to indicate comments; 0 means none */
	uint8_t flags; /* further flags: whether reading a NaN or INF for double values is considered and error */
} read_table;

/* flags used above */
#define READ_TABLE_ALLOW_NAN_INF 1
#define READ_TABLE_CLOSE_FILE 2

/* allocate new read_table struct, fill in the necessary fields */
static void read_table_init(read_table* r, FILE* f_) {
	r->f = f_;
	r->buf = 0;
	r->buf_size = 0;
	r->line_len = 0;
	r->line = 0;
	r->pos = 0;
	r->col = 0;
	r->last_error = T_OK;
	r->delim = 0;
	r->comment = '#';
	r->fn = 0;
	r->base = 10;
	r->flags = READ_TABLE_ALLOW_NAN_INF;
}


/* read a new line (discarding any remaining data in the current line)
 * returns 0 if a line was read, 1 on failure
 * note that failure can mean end of file, which should be checked separately
 * if skip == 1, empty lines are skipped (i.e. reading continues until a
 * nonempty line is found); otherwise, empty lines are read and stored as well,
 * which will probably result in errors if data is tried to be parsed from it */
static int read_table_line_skip(read_table* r, int skip) {
	if(!r) return 1;
	if(r->last_error == T_EOF || r->last_error == T_COPIED ||
		r->last_error == T_ERROR_FOPEN) return 1;
	while(1) {
		ssize_t len = getline(&(r->buf),&(r->buf_size),r->f);
		if(len < 0) {
			r->last_error = T_EOF;
			r->line_len = 0; /* ensure the buffer will never be accessed */
			return 1;
		}
		r->line_len = len; /* note: in this case, len >= 0 */
		r->line++; 
		
		/* check that there is actual data in the line, empty lines are skipped */
		r->pos = 0;
		if(skip) {
			for(; r->pos < r->line_len; r->pos++)
				if( ! (r->buf[r->pos] == ' ' || r->buf[r->pos] == '\t') ) break;
			if(r->comment) if(r->buf[r->pos] == r->comment) continue; /* check for comment character first */
			if(r->pos < r->line_len) break; /* there is some data in the line */
		}
		else break; /* if empty lines should not be skipped */
	}
	r->col = 0; /* reset the counter for columns */
	r->last_error = T_OK;
	return 0;
}
/* simplified version, always skipping empty lines (default behavior) */
static inline int read_table_line(read_table* r) {
	return read_table_line_skip(r,1);
}

/* checks to be performed before trying to convert a field */
static int read_table_pre_check(read_table* r) {
	if(!r) return 1;
	if(r->last_error == T_EOF || r->last_error == T_EOL ||
		r->last_error == T_READ_ERROR || r->last_error == T_ERROR_FOPEN) return 1;
	/* 1. skip any blanks */
	for(;r->pos<r->line_len;r->pos++)
		if( ! (r->buf[r->pos] == ' ' || r->buf[r->pos] == '\t') ) break;
	/* 2. check for end of line or comment */
	if(r->pos == r->line_len || r->buf[r->pos] == '\n' || (r->comment && r->buf[r->pos] == r->comment) ) {
		r->last_error = T_EOL;
		return 1;
	}
	/* 3. check for field delimiter (if we have any) */
	if(r->delim && r->buf[r->pos] == r->delim) {
		r->last_error = T_MISSING;
		return 1;
	}
	return 0;
}

/* perform checks needed after number conversion */
static int read_table_post_check(read_table* r, char* c2) {
	/* 0. check for format errors and overflow as indicated by strto* */
	if(errno == EINVAL || c2 == r->buf + r->pos) {
		r->last_error = T_FORMAT;
		return 1;
	}
	if(errno == ERANGE) {
		r->last_error = T_OVERFLOW;
		return 1;
	}
	/* 1. skip the converted number and any blanks */
	int have_blank = 0;
	for(r->pos = c2 - r->buf;r->pos<r->line_len;r->pos++)
		if( ! (r->buf[r->pos] == ' ' || r->buf[r->pos] == '\t') ) break;
		else have_blank = 1;
	r->last_error = T_OK;
	/* 2. check for end of line -- this is not a problem here */
	if(r->pos == r->line_len || r->buf[r->pos] == '\n' ||
		(r->comment && r->buf[r->pos] == r->comment) ) return 0;
	if(r->delim == 0 && have_blank == 0) {
		/* if there is no explicit delimiter, then there need to be at least
		 * one blank after the converted number if it is not the end of line */
		r->last_error = T_FORMAT;
		return 1;
	}
	/* 3. otherwise, check for proper delimiter, if needed */
	if(r->delim) {
		if(r->buf[r->pos] != r->delim) {
			r->last_error = T_FORMAT;
			return 1;
		}
		r->pos++; /* in this case, advance position further, past the delimiter */
	}
	r->col++; /* advance column counter as well */
	return 0; /* everything OK */
}


/* skip next field, ignoring any content
 * if we have a delimiter, this means advancing until the next delimiter and
 * 	then one more position
 * if no delimiter, this means skipping any blanks, than any nonblanks and
 * 	ending at the next blank */
static int read_table_skip(read_table* r) {
	if(!r) return 1;
	
	if(r->delim) {
		/* if there is a delimiter, just advance until after the next one */
		for(;r->pos<r->line_len;r->pos++) if(r->buf[r->pos] == r->delim) break;
		if(r->pos == r->line_len) {
			r->last_error = T_EOL;
			return 1;
		}
		r->pos++; /* note: we do not care what is after the delimiter */
	}
	else {
		/* no delimiter, skip any blanks, then skip all non-blanks */
		for(;r->pos<r->line_len;r->pos++)
			if( ! (r->buf[r->pos] == ' ' || r->buf[r->pos] == '\t') ) break;
		if(r->pos == r->line_len || r->buf[r->pos] == '\n' || (r->comment && r->buf[r->pos] == r->comment)) {
			r->last_error = T_EOL;
			return 1;
		}
		for(;r->pos<r->line_len;r->pos++) if(r->buf[r->pos] == ' ' ||
			r->buf[r->pos] == '\t' || r->buf[r->pos] == '\n' ||
			(r->comment && r->buf[r->pos] == r->comment)) break;
		/* we do not care what is after the field, now we are either at a
		* 	blank or line end */
	}
	
	r->col++;
	r->last_error = T_OK;
	return 0;
}

/* try to convert the next value to integer
 * check explicitely that it is within the limits provided
 * (note: the limits are inclusive, so either min or max is OK)
 * return 0 on success, 1 on error */
static int read_table_int32_limits(read_table* r, int32_t* i, int32_t min, int32_t max) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	long res = strtol(r->buf + r->pos, &c2, r->base);
	/* check that result fits in 32-bit integer -- long might be 64-bit */
	if(res > (long)max || res < (long)min) {
		r->last_error = T_OVERFLOW;
		return 1;
	}
	*i = res; /* store potential result */
	/* advance position after the number, check if there is proper field separator */
	return read_table_post_check(r,c2);
}
/* default behavior for the previous, whole range is OK */
static inline int read_table_int32(read_table* r, int32_t* i) {
	return read_table_int32_limits(r,i,INT32_MIN,INT32_MAX);
}

/* try to convert the next value to 64-bit integer
 * return 0 on success, 1 on error */
static int read_table_int64_limits(read_table* r, int64_t* i, int64_t min, int64_t max) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	/* note: try to determine if to use long or long long */
	long res;
	long long res2;
	if(LONG_MAX >= INT64_MAX && LONG_MIN <= INT64_MIN) {
		res = strtol(r->buf + r->pos, &c2, r->base);
		/* note: this check might be unnecessary */
		if(res > (long)max || res < (long)min) {
			r->last_error = T_OVERFLOW;
			return 1;
		}
		*i = res; /* store potential result */
	}
	else {
		res2 = strtoll(r->buf + r->pos, &c2, r->base);
		if(res2 > (long long)max || res2 < (long long)min) {
			r->last_error = T_OVERFLOW;
			return 1;
		}
		*i = res2; /* store potential result */
	}
	/* advance position after the number, check if there is proper field separator */
	return read_table_post_check(r,c2);
}
static inline int read_table_int64(read_table* r, int64_t *i) {
	return read_table_int64_limits(r,i,INT64_MIN,INT64_MAX);
}

/* try to convert the next value to 32-bit unsigned integer
 * return 0 on success, 1 on error */
static int read_table_uint32_limits(read_table* r, uint32_t* i, uint32_t min, uint32_t max) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	/* stricly require that the next character is alphanumeric
	 * -- strtoul() will silently accept and negate negative values */
	if( ! (isalnum(r->buf[r->pos]) || r->buf[r->pos] == '+') ) {
		if(r->buf[r->pos] == '-') r->last_error = T_OVERFLOW;
		else r->last_error = T_FORMAT;
		return 1;
	}
	unsigned long res = strtoul(r->buf + r->pos, &c2, r->base);
	/* check that result fits in 32-bit integer -- long might be 64-bit */
	if(res > (unsigned long)max || res < (unsigned int)min) {
		r->last_error = T_OVERFLOW;
		return 1;
	}
	*i = res; /* store potential result */
	/* advance position after the number, check if there is proper field separator */
	return read_table_post_check(r,c2);
}
static inline int read_table_uint32(read_table* r, uint32_t* i) {
	return read_table_uint32_limits(r,i,0,UINT32_MAX);
}

/* try to convert the next value to 64-bit unsigned integer
 * return 0 on success, 1 on error */
static int read_table_uint64_limits(read_table* r, uint64_t* i, uint64_t min, uint64_t max) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	/* stricly require that the next character is alphanumeric
	 * -- strtoul() will silently accept and negate negative values */
	if( ! (isalnum(r->buf[r->pos]) || r->buf[r->pos] == '+') ) {
		if(r->buf[r->pos] == '-') r->last_error = T_OVERFLOW;
		else r->last_error = T_FORMAT;
		return 1;
	}
	/* note: try to determine if to use long or long long */
	unsigned long res;
	unsigned long long res2;
	if(ULONG_MAX >= UINT64_MAX) {
		res = strtoul(r->buf + r->pos, &c2, r->base);
		/* note: this check might be unnecessary */
		if(res > (unsigned long)max || res < (unsigned long)min) {
			r->last_error = T_OVERFLOW;
			return 1;
		}
		*i = res; /* store potential result */
	}
	else {
		res2 = strtoull(r->buf + r->pos, &c2, r->base);
		if(res2 > (unsigned long long)max || res < (unsigned long long)min) {
			r->last_error = T_OVERFLOW;
			return 1;
		}
		*i = res2; /* store potential result */
	}
	/* advance position after the number, check if there is proper field separator */
	return read_table_post_check(r,c2);
}
static inline int read_table_uint64(read_table* r, uint64_t* i) {
	return read_table_uint64_limits(r,i,0,UINT64_MAX);
}

/* try to convert the next value to a 16-bit signed integer
 * return 0 on success, 1 on error */
static int read_table_int16_limits(read_table* r, int16_t* i, int16_t min, int16_t max) {
	/* just use the previous function and check for overflow */
	int32_t i2;
	/* note: the following function already check for overflow as well */
	if(read_table_int32_limits(r,&i2,(int32_t)min,(int32_t)max)) return 1;
	*i = i2;
	return 0;
}
static inline int read_table_int16(read_table* r, int16_t* i) {
	return read_table_int16_limits(r,i,INT16_MIN,INT16_MAX);
}

/* try to convert the next value to a 16-bit unsigned integer
 * return 0 on success, 1 on error */
static int read_table_uint16_limits(read_table* r, uint16_t* i, uint16_t min, uint16_t max) {
	/* just use the previous function and check for overflow */
	uint32_t i2;
	if(read_table_uint32_limits(r,&i2,(uint32_t)min,(uint32_t)max)) return 1;
	*i = i2;
	return 0;
}
static inline int read_table_uint16(read_table* r, uint16_t* i) {
	return read_table_uint16_limits(r,i,0,UINT16_MAX);
}

/* try to convert the next value to a double precision float value
 * return 0 on success, 1 on error */
static int read_table_double(read_table* r, double* d) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	*d = strtod(r->buf + r->pos, &c2);
	/* advance position after the number, check if there is proper field separator */
	if(read_table_post_check(r,c2)) return 1;
	if((r->flags & READ_TABLE_ALLOW_NAN_INF) == 0) {
		if(_isnan(*d) || _isinf(*d)) {
			r->last_error = T_NAN;
			return 1;
		}
	}
	return 0;
}
static int read_table_double_limits(read_table* r, double* d, double min, double max) {
	if(read_table_pre_check(r)) return 1;
	errno = 0;
	char* c2;
	*d = strtod(r->buf + r->pos, &c2);
	if(read_table_post_check(r,c2)) return 1;
	if(_isnan(*d)) {
		r->last_error = T_NAN;
		return 1;
	}
	/* note: this will not be true if min or max is NaN, not sure if
	 * that's a problem */
	if(*d > max || *d < min) {
		r->last_error = T_OVERFLOW;
		return 1;
	}
	return 0;
}


/* write formatted error message to the given stream */
static void read_table_write_error(const read_table* r, FILE* f) {
	if(!r) return;
	fprintf(f,"read_table, ");
	if(r->fn) fprintf(f,"file %s, ",r->fn);
	else fprintf(f,"input ");
	fprintf(f,"line %lu, position %lu / column %lu: %s\n",r->line,r->pos,r->col,
		get_error_desc(r->last_error));
}

static const char* read_table_get_line_str(const read_table* r) {
	if(r) return r->buf;
	return 0;
}



/* C++ object oriented interface, templated functions */
#ifdef __cplusplus

#include <utility>

template<class T>
static int read_table_next(read_table* r, T& val) {
	 /* not implemented generally, set error to indicate that an
	  * unsupported conversion was attempted */
	if(r) r->last_error = T_TYPE;
	return 1;
}
/* template specializations to use the same function name */
template<> int read_table_next(read_table* r, int32_t& val)
	{ return read_table_int32(r,&val); }
template<> int read_table_next(read_table* r, uint32_t& val)
	{ return read_table_uint32(r,&val); }
template<> int read_table_next(read_table* r, int64_t& val)
	{ return read_table_int64(r,&val); }
template<> int read_table_next(read_table* r, uint64_t& val)
	{ return read_table_uint64(r,&val); }
template<> int read_table_next(read_table* r, int16_t& val)
	{ return read_table_int16(r,&val); }
template<> int read_table_next(read_table* r, uint16_t& val)
	{ return read_table_uint16(r,&val); }
template<> int read_table_next(read_table* r, double& val)
	{ return read_table_double(r,&val); }
//~ template<class T> int read_table_next(read_table* r, T& val)
	//~ { return read_table_next<T*>(r,&val); }
template<> int read_table_next(read_table* r, std::pair<double,double>& p) {
	double x,y;
	if(read_table_double(r,&x) || read_table_double(r,&y)) return 1;
	p = std::make_pair(x,y);
	return 0;
}




/* recursive templated function to convert whole line using one function call only
 * note: recursion will be probably eliminated and the whole function expanded to
 * the actual sequence of conversions needed */
static int read_table_multiple(read_table* r) { return 0; }
template<class first, class ...rest>
static int read_table_multiple(read_table* r, first&& val, rest&&... vals) {
	if(read_table_next(r,val)) return 1;
	return read_table_multiple(r,vals...);
}


/* C++ class interface for easier usage */
struct read_table2 : public read_table {
	public:
		/* constructor from a file that is already open
		 * note: this will not close the file upon destruction
		 * note: this could be useful when using FILE objects obtained with
		 * 	e.g. popen() or other means */
		read_table2(FILE* f_) {
			read_table_init(this,f_);
		}
		/* constructor accepting a filename and opening it
		 * this will result in the destructor closing the file as well */
		read_table2(const char* fn_) {
			FILE* f_ = fopen(fn_,"r");
			read_table_init(this,f_);
			/* note: if the file cannot be opened, no exception is thrown,
			 * any read will fail and the error is set to indicate this */
			if(!f_) last_error = T_ERROR_FOPEN;
			else flags |= READ_TABLE_CLOSE_FILE;
			fn = fn_;
		}
		/* copy constructor: it is safe to copy everything, except the buffer
		 * which will be allocated; note that only one of the instances
		 * should be used, so copying invalidates the original */
		read_table2(read_table2& rt_) : read_table(rt_) {
			/* note: the above statement copies all data members first
			 * we then invalidate rt_ to not be able to read from the file
			 * with two different instances of this class */
			rt_.buf = 0;
			rt_.buf_size = 0;
			rt_.pos = 0;
			rt_.line_len = 0;
			rt_.col = 0;
			rt_.last_error = T_COPIED;
		}
		/* destructor frees temporary buffer */
		~read_table2() {
			if(buf) free(buf);
			buf = 0;
			buf_size = 0;
			if(flags & READ_TABLE_CLOSE_FILE) if(f) fclose(f);
			f = 0;
		}
		/* read next line into the internal buffer */
		bool read_line(bool skip = true) {
			if(skip) return (read_table_line_skip(this,1)==0);
			else return (read_table_line_skip(this,0)==0);
		}
		/* try to parse one value from the currently read line
		 * T can be 16, 32 or 64 bit signed or unsigned int or double */
		template<class T> bool read_next(T&& val) {
			return (read_table_next(this,val) == 0);
		}
		/* try to parse whole line (read previously with read_line()),
		 * into the given list of parameters
		 * note: parameters need to be pointers, not references
		 * supported types: 16, 32 and 64 bit signed or unsigned int and double */
		template<class first, class ...rest>
		bool read(first&& val, rest&&... vals) {
			return (read_table_multiple(this,val,vals...) == 0);
		}
		
		
		/* non-templated helper functions for reading specific data types and values */
		/* skip next field, ignoring any content */
		bool read_skip() { return (read_table_skip(this) == 0); }
		/* read one 32-bit signed integer in the given limits */
		bool read_int32_limits(int32_t& i, int32_t min, int32_t max) {
			return (read_table_int32_limits(this,&i,min,max) == 0); }
		bool read_int32(int32_t& i) { return (read_table_int32(this,&i) == 0); }
		/* read one 32-bit unsigned integer in the given limits */
		bool read_uint32_limits(uint32_t& i, uint32_t min, uint32_t max) {
			return (read_table_uint32_limits(this,&i,min,max) == 0); }
		bool read_uint32(uint32_t& i) { return (read_table_uint32(this,&i) == 0); }
		/* read one 64-bit signed integer in the given limits */
		bool read_int64_limits(int64_t& i, int64_t min, int64_t max) {
			return (read_table_int64_limits(this,&i,min,max) == 0); }
		bool read_int64(int64_t& i) { return (read_table_int64(this,&i) == 0); }
		/* read one 64-bit unsigned integer in the given limits */
		bool read_uint64_limits(uint64_t& i, uint64_t min, uint64_t max) {
			return (read_table_uint64_limits(this,&i,min,max) == 0); }
		bool read_uint64(uint64_t& i) { return (read_table_uint64(this,&i) == 0); }
		/* read one 16-bit signed integer in the given limits */
		bool read_int16_limits(int16_t& i, int16_t min, int16_t max) {
			return (read_table_int16_limits(this,&i,min,max) == 0); }
		bool read_int16(int16_t& i) { return (read_table_int16(this,&i) == 0); }
		/* read one 16-bit unsigned integer in the given limits */
		bool read_uint16_limits(uint16_t& i, uint16_t min, uint16_t max) {
			return (read_table_uint16_limits(this,&i,min,max) == 0); }
		bool read_uint16(uint16_t& i) { return (read_table_uint16(this,&i) == 0); }
		/* read one double value in the given limits */
		bool read_double_limits(double& d, double min, double max) {
			return (read_table_double_limits(this,&d,min,max) == 0); }
		bool read_double(double& d) { return (read_table_double(this,&d) == 0); }
		
		
		/* set parameters, diagnostics */
		
		/* set delimiter character (default is spaces and tabs) */
		void set_delim(char delim_) { delim = delim_; }
		/* get delimiter character (default is spaces and tabs) */
		char get_delim() const { return delim; }
		/* set comment character (default is none) */
		void set_comment(char comment_) { comment = comment_; }
		/* get comment character (default is none) */
		char get_comment() const { return comment; }
		
		/* get last error code */
		enum read_table_errors get_last_error() const { return last_error; }
		const char* get_last_error_str() const { return get_error_desc(last_error); }
		
		/* get current position in the file */
		uint64_t get_line() const { return line; }
		size_t get_pos() const { return pos; }
		size_t get_col() const { return col; }
		/* set filename (for better formatting of diagnostic messages) */
		void set_fn(const char* fn_) { fn = fn; }
		/* get current line string */
		const char* get_line_str() const { return read_table_get_line_str(this); }
		
		/* write formatted error message to the given stream */
		void write_error(FILE* f) const { read_table_write_error(this,f); }
};

#endif /* __cplusplus */

#endif /* _READ_TABLE_H */


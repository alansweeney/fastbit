// $Id$
// Author: John Wu <John.Wu at acm.org>
//      Lawrence Berkeley National Laboratory
// Copyright 2007-2009 the Regents of the University of California
/** @file
    Declares ibis::selectClause class.
*/
#ifndef IBIS_SELECTCLAUSE_H
#define IBIS_SELECTCLAUSE_H
#include "qExpr.h"
#include "table.h"

namespace ibis {
    class selectLexer;
    class selectParser;
}

/// A class to parse a string into a list of arithmetic expressions and
/// aggregation functions.
class ibis::selectClause {
public:
    /// Parse a new string as a select clause.
    explicit selectClause(const char *cl=0);
    /// Parse a list of strings.
    selectClause(const ibis::table::stringList&);
    ~selectClause();

    /// Copy constructor.  Deep copy.
    selectClause(const selectClause&);

    /// Parse a new string.
    int parse(const char *cl);

    /// Return a pointer to the string form of the select clause.
    const char* getString(void) const {return clause_.c_str();}
    /// Dereferences to the string form of the select clause.
    const char* operator*(void) const {return clause_.c_str();}

    bool empty() const {return terms_.empty();}
    uint32_t size() const {return terms_.size();}
    /// A vector of arithematic expressions.
    typedef std::vector<ibis::math::term*> mathTerms;
    const mathTerms& getTerms() const {return terms_;}
    /// Fetch the ith term of the select clause, with array bound checking.
    const ibis::math::term* operator[](unsigned i) const {
	if (i < terms_.size())
	    return terms_[i];
	else
	    return 0;
    }
    /// Fetch the ith term of the select clause, without array bound checking.
    const ibis::math::term* at(unsigned i) const {return terms_[i];}

    /// Print the content.
    void print(std::ostream&) const;
    /// Remove the current content.
    void clear();

    int find(const char*) const;
    /// Name inside the aggregation function.
    const char* argName(unsigned i) const {return names_[i].c_str();}
    /// Name given to the whole aggregation function.
    const char* termName(unsigned i) const {return xnames_[i].c_str();}
    void describe(unsigned i, std::string &str) const;
    uint32_t nPlain() const;

    /// Aggregation functions.  @note "Agregado" is Spanish for aggregate.
    enum AGREGADO {NIL, AVG, CNT, MAX, MIN, SUM,
		   VARPOP, VARSAMP, STDPOP, STDSAMP, DISTINCT};
    /// Return the aggregation function used for the ith term.
    AGREGADO getAggregator(uint32_t i) const {return aggr_[i];}

    int verify(const ibis::part&);
    int verifySome(const ibis::part&, const std::vector<uint32_t>&);
    void getNullMask(const ibis::part&, ibis::bitvector&) const;

    /// Assignment operator.
    selectClause& operator=(const selectClause& rhs) {
	selectClause tmp(rhs);
	swap(tmp);
	return *this;
    }
    /// Swap the content of two select clauses.
    void swap(selectClause& rhs) {
	terms_.swap(rhs.terms_);
	aggr_.swap(rhs.aggr_);
	alias_.swap(rhs.alias_);
	names_.swap(rhs.names_);
	xnames_.swap(rhs.xnames_);
	clause_.swap(rhs.clause_);
    }

protected:
    /// Arithmetic expressions.
    mathTerms terms_;
    /// Aggregation functions.
    std::vector<AGREGADO> aggr_;
    typedef std::map<std::string, unsigned> StringToInt;
    /// Aliases.
    StringToInt alias_;
    /// Names of given to the variables inside the aggregation functions.
    std::vector<std::string> names_;
    /// Names of given to the aggregation operations.
    std::vector<std::string> xnames_;

    std::string clause_;	///< String version of the select clause.

    ibis::selectLexer *lexer;	///< A pointer for the parser.

    friend class ibis::selectParser;

    /// Sort out the names for the terms.
    void fillNames();
    /// The actual work-horse to do the verification.
    int _verify(const ibis::part&, const ibis::math::term&) const;
}; // class ibis::selectClause

/// Number of terms without aggregation functions.
inline uint32_t ibis::selectClause::nPlain() const {
    uint32_t ret = 0;
    for (uint32_t j = 0; j < aggr_.size(); ++j)
	ret += (aggr_[j] == NIL);
    return ret;
} // ibis::selectClause::nPlain

namespace std {
    inline ostream& operator<<(ostream&, const ibis::selectClause&);
}

inline std::ostream& std::operator<<(std::ostream& out,
				     const ibis::selectClause &sel) {
    sel.print(out);
    return out;
} // std::operator<<
#endif

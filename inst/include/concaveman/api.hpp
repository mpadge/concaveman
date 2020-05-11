#ifndef R_CONCAVEMAN_H
#define R_CONCAVEMAN_H

#pragma once

#include <RcppCommon.h>
#include <array>

namespace Rcpp {
namespace traits {

//  for using Rcpp::as< >();
template < typename T > class Exporter< std::array< T, 2 > >;
template < typename T > class Exporter< std::vector< std::array< T, 2 > > >;

} // traits
} // Rcpp


#include "Rcpp.h"

namespace Rcpp {
namespace traits {

    template < typename T > class Exporter< std::array< T, 2 > > {
        typedef typename std::array< T, 2 > Point;
        
        const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
        Rcpp::Vector< RTYPE > vec;
        
    public:
        Exporter( SEXP x ) : vec( x ) {
            if( TYPEOF( x ) != RTYPE ) {
                throw std::invalid_argument("decido - invalid R object for creating a Point");
            }
        }
        
        Point get() {
            if( vec.length() != 2 ) {
                Rcpp::stop("decido - each point in the polygon must have exactly two coordinates");
            }
            Point x({ vec[0], vec[1] });
            return x;
        }
        
    };
    
    template < typename T > class Exporter< std::vector< std::array< T, 2 > > > {
        typedef typename std::vector< std::array< T, 2 > > Polygon;
        
        const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
        Rcpp::Matrix< RTYPE > mat;
        
    public:
        Exporter( SEXP x ) : mat( x ) {
            if( TYPEOF( x ) != RTYPE ) {
                throw std::invalid_argument("decido - invalid R object for creating a Polygon");
            }
        }
        
        Polygon get() {
            R_xlen_t n_row = mat.nrow();
            Polygon x( n_row );
            R_xlen_t i;
            for( i = 0; i < n_row; ++i ) {
                Rcpp::Vector< RTYPE > v = mat( i, Rcpp::_);
                x[i] = Rcpp::as< std::array< T, 2 > >( v );
            }
            return x;
        }
    };

} // traits
} // Rcpp

#include "concaveman.h"

typedef double T;
typedef std::array <T, 2> Point;
typedef std::vector< Point > Points;

namespace concaveman {
namespace api {

template < int RTYPE >
Rcpp::DataFrame concaveman(
        Rcpp::Matrix< RTYPE >& xy, 
        Rcpp::IntegerVector& hull_in,
        const double concavity, 
        const double length_threshold
    ) {

    Points points = Rcpp::as< Points >( xy );

    std::vector <int> hull = Rcpp::as <std::vector <int> > (hull_in);

    auto concave_points = concaveman::concaveman <T, 16> (points, hull,
            concavity, length_threshold);

    Rcpp::NumericVector xout (concave_points.size ()),
        yout (concave_points.size ());
    for (int i = 0; i < concave_points.size (); i++)
    {
        xout (i) = concave_points [i] [0];
        yout (i) = concave_points [i] [1];
    }

    Rcpp::DataFrame res = Rcpp::DataFrame::create (
            Rcpp::Named ("x") = xout,
            Rcpp::Named ("y") = yout
        );

    return res;
}

Rcpp::DataFrame concaveman(
        SEXP& xy, Rcpp::IntegerVector hull_in,
        const double concavity, const double length_threshold) {
    
    switch( TYPEOF( xy ) ) {
    case INTSXP: {
        Rcpp::IntegerMatrix im = Rcpp::as< Rcpp::IntegerMatrix >( xy );
        return concaveman( im, hull_in, concavity, length_threshold );
    }
    case REALSXP: {
        Rcpp::NumericMatrix nm = Rcpp::as< Rcpp::NumericMatrix >( xy );
        return concaveman( nm, hull_in, concavity, length_threshold );
    }
    default: {
        Rcpp::stop("concaveman - unsupported type");
    }
    }
    
    return Rcpp::DataFrame::create(); // #nocov never reaches
    
}

} // end namespace api
} // end namespace concaveman

#endif

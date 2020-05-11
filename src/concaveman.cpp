#include "concaveman/api.hpp"

//' rcpp_concaveman
//' @noRd 
// [[Rcpp::export]]
Rcpp::DataFrame rcpp_concaveman (SEXP xy, Rcpp::IntegerVector hull_in,
        const double concavity, const double length_threshold)
{
    return concaveman::api::concaveman (xy, hull_in, concavity, length_threshold);
}


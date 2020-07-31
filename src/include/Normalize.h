#ifndef NORMALIZE_H
#define NORMALIZE_H
#include <tuple>
//#include <armadillo>
#include "RcppArmadillo.h"
#include "utils.h"

std::tuple<arma::sp_mat, double> DeNormalize(arma::sp_mat & B_scaled, 
                                             arma::vec & BetaMultiplier, 
                                             arma::vec & meanX, double meany)
{
    arma::sp_mat B_unscaled = B_scaled % BetaMultiplier;
    double intercept = meany - arma::dot(B_unscaled, meanX);
    return std::make_tuple(B_unscaled, intercept);
}

template <typename T>
std::tuple<arma::vec, arma::vec, double>  Normalize(const T& X, 
                                                    const arma::vec& y, 
                                                    T & X_normalized, 
                                                    arma::vec & y_normalized, 
                                                    bool Normalizey = false, 
                                                    bool intercept = true)
{
    unsigned int n = X.n_rows;
    // unsigned int p = X.n_cols;
    
    auto martrix_center_return = matrix_center(X, X_normalized, intercept);
    X_normalized = std::get<0>(martrix_center_return);
    arma::rowvec meanX = std::get<1>(martrix_center_return);

    auto matrix_normailize_return = matrix_normailize(X, X_normalized);
    X_normalized = std::get<0>(matrix_normailize_return);
    arma::rowvec scaleX = std::get<1>(matrix_normailize_return);
    
    arma::vec BetaMultiplier;
    double meany = 0;
    if (Normalizey)
    {
        if (intercept){meany = arma::mean(y);}
        y_normalized = y - meany;
        
        auto stddev = arma::as_scalar(arma::stddev(y_normalized, 1, 0));
        double scaley = 1;
        // below stddev != 0 ensures we properly handle cases where y is constant
        if (stddev != 0){scaley = std::sqrt(n) * stddev;} // contains the l2norm of y
        y_normalized = y_normalized / scaley;
        BetaMultiplier = scaley / (scaleX.t()); // transpose scale to get a col vec
        // Multiplying the learned Beta by BetaMultiplier gives the optimal Beta on the original scale
    }
    else
    {
        y_normalized = y;
        BetaMultiplier = 1 / (scaleX.t()); // transpose scale to get a col vec
    }
    return std::make_tuple(BetaMultiplier, meanX.t(), meany);
}

std::tuple<arma::sp_mat, double> DeNormalize(arma::sp_mat & B_scaled,
                                             arma::vec & BetaMultiplier,
                                             arma::vec & meanX, double meany);

#endif // NORMALIZE_H

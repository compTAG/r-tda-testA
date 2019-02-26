#' @title knnDE
#'
#' @param X an n by d matrix of coordinates containing points used int the
#' density estimation, where n is the number of points and d is the dimension
#'
#' @param Grid an m by d matrix of coordinates, where m is the number of points
#' in the grid and d is the dimension
#'
#' @param k a number used as the smoothing parameter
#'
#' @return a vector of length m (number of points in Grid) containing the value
#' of the knn Density Estimator for each point in the grid

knnDE <-
function(X, Grid, k){
  
  if (!is.numeric(X) && !is.data.frame(X)) {
    stop("X should be a matrix of coordinates")
  }
  if (!is.numeric(Grid) && !is.data.frame(Grid)) {
    stop("Grid should be a matrix of coordinates")
  }
  if (!is.numeric(k) || length(k) != 1 || k < 1) {
    stop("k should be a positive integer")
  }

  # store d, the dimension of X, and n, the number of points in X
  d <- ncol(X)
  n <- nrow(X)
  # find the Euclidean distances of k nearest neighbors from input matrix X and the query matrix
  # Grid using the fast nearest neighbor searching algorithm "kd_tree"
  r.k <- apply(FNN::knnx.dist(X, Grid, k = k, algorithm = "kd_tree"), 1, max)
  # volume of the Euclidean d dimensional unit ball
  v.d <- pi^(d/2) /gamma(d/2+1)
  # final output incorporates smoothing parameter with volume of unit ball and euclidean k nearest neighbor
  # distances to return output vector containing density estimators for each point in the grid
  out <- k / (n * v.d * r.k ^ d)  
  return(out)
}

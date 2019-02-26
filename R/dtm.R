#' @title dtm
#' @param X an n by d matrix of coordinates of points used to construct the uniform
#'   empirical measure for the distance to measure, where n is the number of points
#'   and d is the dimension.
#'
#' @param Grid an m by d matrix of coordinates of points where the distance to measure
#'   is computed, where m is the number of points in Grid and d is the dimension.
#'
#' @param m0 a numeric variable for the smoothing parameter of the distance to measure.
#'   Roughly, m0 is the the percentage of points of X that are considered when the distance
#'   to measure is computed for each point of Grid. The value of m0 should be in (0,1).
#'
#' @param r a numeric variable for the tuning parameter of the distance to measure.
#'   The value of r should be in [1,∞), and the default value is 2.
#'
#' @param weight either a number, or a vector of length n. If it is a number, then same
#'   weight is applied to each points of X. If it is a vector, weight represents weights of
#'   each points of X. The default value is 1.
#'
#' @return a vector of length m (the number of points stored in Grid)
#'   containing the value of the distance to measure function evaluated at each point of Grid.

dtm <-
function(X, Grid, m0, r = 2, weight = 1) {
	
  if (!is.numeric(X) && !is.data.frame(X)) {
    stop("X should be a matrix of coordinates")
  }
  if (!is.numeric(Grid) && !is.data.frame(Grid)) {
    stop("Grid should be a matrix of coordinates")
  }
  if (NCOL(X) != NCOL(Grid)) {
    stop("dimensions of X and Grid do not match")
  }
	
  if (!is.numeric(m0) || length(m0) != 1 || m0 < 0 || m0 > 1) {
    stop("m0 should be a number between 0 and 1")
  }
	
  if (!is.numeric(r) || length(r) != 1 || r < 1) {
    stop("r should be a number greater than or equal to 1")
  }
	
  # verify that weight is either constant or that it provides a correspondence with every point
  if (!is.numeric(weight) || 
      (length(weight) != 1 && length(weight) != NROW(X))) {
    stop("weight should be either a number or a vector of length equals the number of sample")
  }

  # without weight
  if (length(weight) == 1) {
    X <- as.matrix(X)
    weightBound <- m0 * NROW(X) 
    # use fast nearest neighbor search algorithm to find distances to k nearest neighbors
    knnDistance <- FNN::knnx.dist(
		data = X, query = as.matrix(Grid), k = ceiling(weightBound),
		algorithm = c("kd_tree"))
    # find dtm without considering a weight
    return (Dtm(knnDistance = knnDistance, weightBound = weightBound, r = r))

  # with weight
  } else {
    # establish the weightbound and weight parameters to be used in final DtmWeight function
    X0 <- as.matrix(X[weight != 0, , drop = FALSE]) 
    weight0 <- weight[weight != 0]
    weight0sort <- sort(weight0)
    weightBound <- m0 * sum(weight0)
    weightSumTemp <- 0
    # add sorted weight values to a sum until that sum reaches weight bound
    for (k0 in seq(along = weight0)) {
      weightSumTemp <- weightSumTemp + weight0sort[k0]
      if (weightSumTemp >= weightBound) {
        break
      }
    }
    # create a matrix of nearest neighbor indeces using the fast nearest neighbor kd tree algorithm
    knnDistanceIndex <- FNN::get.knnx(
	    data = X0, query = as.matrix(Grid), k = k0, algorithm = c("kd_tree"))
    # find dtm with weight established
    return (DtmWeight(
	    knnDistance = knnDistanceIndex[["nn.dist"]], weightBound = weightBound,
		r = r, knnIndex = knnDistanceIndex[["nn.index"]], weight = weight0))
  }
}

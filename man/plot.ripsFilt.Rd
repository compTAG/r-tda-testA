\name{plot.ripsFilt}

\alias{plot.ripsFilt}

\title{
  Plot Euclidean Rips filtration
}

\description{
  The function \code{plot.ripsFilt} plots the Euclidean Rips filtration stored in an object of class \code{ripsFilt}.
}

\usage{
\method{plot}{ripsFilt}(
    filt, r = NULL, circles = TRUE, ...)
}

\arguments{
  \item{filt}{
    an object of class \code{ripsFilt} (as returned by the functions \code{\link{ripsFiltration}}). Filtration distances must be Euclidean and two-dimensional.
  }

  \item{r}{
    numeric value specifying the threshold radius defining the subcomplex to be plotted. If \code{NULL} then it is automatically computed using the maximum value in the Rips filtration object.
  }

  \item{circles}{
    logical for whether or not to draw circles defining the complex.
  }

  \item{\dots}{
    additional graphical parameters.
  }
}

\references{
  Edelsbrunner H, Harer J (2010). "Computational topology: an introduction." American Mathematical Society.
}

\author{
  Robin Belton, Emily Flanagan, Jisu Kim, Jordan Schupbach
}

\seealso{
  \code{\link{ripsFiltration}}, \code{\link{animate.ripsFilt}}, \code{\link{ripsDiag}}
}

\examples{
X <- cbind(c(0,1,2,3), c(0,3,-1,2))

## rips diagram
ripsFilt <- ripsFiltration(X, maxdimension = 2, maxscale = 5,
                             dist = "euclidean")

#plot
par(mfrow = c(1, 3))
plot(ripsFilt)
plot(ripsFilt, circles = FALSE)
plot(ripsFilt, r = 1)
}

\keyword{hplot}

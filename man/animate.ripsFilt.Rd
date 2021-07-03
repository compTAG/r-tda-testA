\name{animate.ripsFilt}

\alias{animate.ripsFilt}

\title{
  Animate Eulclidean Rips filration
}

\description{
  The function \code{animate.ripsFilt} animates the Rips filtration stored in an object of class \code{ripsFilt}.
}

\usage{
\method{animate}{ripsFilt}(
   filt, circles = TRUE, fn_out = "RipsFilt.gif", lout = NULL, interval = 1, ...)
}

\arguments{
  \item{filt}{
    an object of class \code{ripsFilt} (as returned by the functions \code{\link{ripsFiltration}}). Filtration distances must be Euclidean and two-dimensional.
  }

  \item{circles}{
    logical for whether or not to draw circles defining the complex.
  }

  \item{fn_out}{
    string representing the filename of the animation. If \code{NULL} then animation is saved as "RipsFilt.gif" in current directory.
  }

  \item{lout}{
    an optional numeric value that specifies the number of frames in the animation. If \code{NULL} then the number of unique values in the filtration is used.
  }

  \item{interval}{
    a positive number to set the time interval of the animation (unit in seconds); default to be 1.
  }

  \item{\dots}{
    additional graphical parameters.
  }
}

\references{
  Edelsbrunner H, Harer J (2010). "Computational topology: an introduction." American Mathematical Society.
}

\author{
  Robin Belton, Emily Flanagan, Jordan Schupbach
}

\seealso{
  \code{\link{ripsFiltration}}, \code{\link{animate.ripsFilt}}, \code{\link{ripsDiag}}
}

\examples{
X <- cbind(c(0,1,2,3), c(0,3,-1,2))

## rips diagram
ripsFilt <- ripsFiltration(X, maxdimension = 2, maxscale = 5,
                             dist = "euclidean")

#animate
animate(ripsFilt, fn_out = "myAnimation.gif", lout = 100, interval = 0.1)

}

\keyword{animate}

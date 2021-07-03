plot.ripsFilt <-
function(filt, r = NULL, circles = TRUE, ...) {

  if (filt$dist != "euclidean") {
    stop("Distance is not Euclidean")
    break
  }

  data <- filt$coordinates
  xrange <- range(data[, 1])
  yrange <- range(data[, 2])
  max_dist <- max(dist(data)) / 2

  xlims <- c(xrange[1] - max_dist, xrange[2] + max_dist)
  ylims <- c(yrange[1] - max_dist, yrange[2] + max_dist)

  if (is.null(r)) {
    r <- max(filt$values)
  }

  plot(NULL, type = "n", xlim = xlims, ylim = ylims, main = "Rips Complex",
       xlab = "", ylab = "", asp = 1, ...)
  if (circles) {
    for(j in 1:length(data[, 1])) {
      plotrix::draw.circle(x = data[j, 1], y = data[j, 2], radius = r / 2,
                  col = rgb(0.355, 0.144, 0.628, 0.05))
    }
  }
  for (idx in seq(along = filt[["cmplx"]][filt$values <= r])) {
    polygon(data[filt[["cmplx"]][[idx]], 1], data[filt[["cmplx"]][[idx]], 2],
            col = rgb(0.976, 0.949, 0.73, 0.6), border = NA, lwd = 1.5)
  }
  for (idx in seq(along = filt[["cmplx"]][filt$values <= r])) {
    polygon(data[filt[["cmplx"]][[idx]], 1], data[filt[["cmplx"]][[idx]], 2],
            col = NULL, lwd = 1.5)
  }
  points(data, pch = 16)

}

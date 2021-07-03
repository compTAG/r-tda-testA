animate <- 
function(...) {
  UseMethod("animate")
}

animate.ripsFilt <- 
function(filt, circles = TRUE, fn_out = "RipsFilt.gif", lout = NULL, interval = 1, ...) {
  data <- filt$coordinates
  
  if (is.null(lout)) {
    s <- unique(filt$values)
  } else {
    s <- seq(0, max(dist(data)), length.out = lout)
  }
  
  saveGIF({
    for (i in s){
      plot(filt, r = i, circles = circles, ...)
    }
  },
  movie.name = fn_out, interval = interval)
}

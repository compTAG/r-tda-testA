testthat::context("ripsDiag")

testthat::test_that("Test Rips diagram function with distance matrices", {
  sphere_points <- matrix(c(-1, 0,	0, 1,	0, 0, 0, -1, 0,
                             0, 1, 0, 0,	0, -1, 0,	0, 1),
                          nrow = 6, ncol = 3)

  # Use distance matrix for sphere
  diag1 <- ripsDiag(dist(sphere_points), maxdimension = 2,
                    maxscale = 5, dist = "arbitrary")

  # Use distance  matrix
  diag2 <- ripsDiag(sphere_points, maxdimension = 2,
                    maxscale = 5, dist = "euclidean")

  testthat::expect_equal(diag1$diagram[(diag2$diag[,'dimension'] == 2),2], sqrt(2))
  testthat::expect_equal(diag2$diagram[(diag2$diag[,'dimension'] == 2),2], sqrt(2))
})


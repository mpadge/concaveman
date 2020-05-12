#' A very fast 2D concave hull algorithm
#'
#' The `concaveman` function ports the [concaveman](https://github.com/mapbox/concaveman) library from mapbox. It computes the concave polygon for one set of points.
#'
#' For details regarding the implementation, please see the original javascript library [github page](https://github.com/mapbox/concaveman). This is just a thin wrapper, via [`V8`](https://cran.r-project.org/package=V8).
#'
#' @param points the points for which the concave hull must be computed. Can be represented as a matrix of coordinates or an `sf` object.
#' @param concavity a relative measure of concavity. 1 results in a relatively detailed shape, Infinity results in a convex hull. You can use values lower than 1, but they can produce pretty crazy shapes.
#' @param length_threshold when a segment length is under this threshold, it stops being considered for further detalization. Higher values result in simpler shapes.
#'
#' @return an object of the same class as `points`: a matrix of coordinates or an `sf` object.
#' @examples
#' data(points)
#' polygons <- concaveman(points)
#' plot(points)
#' plot(polygons, add = TRUE)
#'
#' @export
concaveman <- function(points, concavity, length_threshold) UseMethod("concaveman", points)

#' @export
#' @rdname concaveman
concaveman.matrix <- function(points, concavity = 2, length_threshold = 0) {
  index <- grDevices::chull (points)
  index <- c (index, index [1])
  rcpp_concaveman (points, index - 1, concavity, length_threshold)
}

#' @export
#' @rdname concaveman
concaveman.sf <- function(points, concavity = 2, length_threshold = 0) {

  geom_col <- attr( points, "sf_column" )
  geom <- points[[ geom_col ]]
  crs <- attributes( points[["geometry"]] )[["crs"]]
  
  df <- sfheaders::sf_to_df( points )  ## guaranteed to return x & y columns
  m <- as.matrix( df[, c("x","y") ] )
  res <- concaveman( m, concavity, length_threshold )
  
  sf <- sfheaders::sf_polygon(obj = res, x = "x", y = "y")
  
  attr( sf$geometry, "crs" ) <- crs
  return( sf )
}


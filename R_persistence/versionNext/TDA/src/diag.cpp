// for R
#include <R.h>
#include <R_ext/Print.h>

// for Rcpp
#include <Rcpp.h>

// for Rips
#include <tdautils/ripsL2.h>
#include <tdautils/ripsArbit.h>

// for grid
#include <tdautils/gridUtils.h>

// for kernel density
#include <tdautils/kernelUtils.h>

// for changing formats and typecasting
#include <tdautils/typecastUtils.h>

//for GUDHI
#include <tdautils/gudhiUtils.h>

//for CGAL
#include <tdautils/cgalUtils.h>

// for Dionysus
#include <tdautils/dionysusUtils.h>

// for phat
#include <tdautils/phatUtils.h>



/**
 * grid function by Brittany T. Fasy
 * modified by Jisu Kim for
 * arbitrary dimension & using memory as an input & setting maximum dimension.
 */
// [[Rcpp::export]]
Rcpp::List GridDiag(const Rcpp::NumericVector& FUNvalues,
		const Rcpp::IntegerVector& gridDim, const int maxdimension,
		const std::string& decomposition, const std::string& library,
		const bool location, const bool printProgress) {

#ifdef LOGGING
	//rlog::RLogInit(argc, argv);

	stdoutLog.subscribeTo(RLOG_CHANNEL("topology/persistence"));
	//stdoutLog.subscribeTo(RLOG_CHANNEL("topology/chain"));
	//stdoutLog.subscribeTo(RLOG_CHANNEL("topology/vineyard"));
#endif

	std::vector< std::vector< std::vector< double > > > persDgm;
	std::vector< std::vector< std::vector< unsigned > > > persLoc;
	std::vector< std::vector< std::set< unsigned > > > persCycle;

	Fltr f;

	// Generate simplicial complex from function values and grid
	if (decomposition[0] == '5') {
		simplicesFromGrid(f, FUNvalues, gridDim, maxdimension + 1);
	}
	if (decomposition[0] == 'b') {
		simplicesFromGridBarycenter(f, FUNvalues, gridDim, maxdimension + 1);
	}
	if (printProgress) {
		Rprintf("# Generated complex of size: %d \n", f.size());
	}

	// Sort the simplices with respect to function values
	f.sort(Smplx::DataComparison());

	// Compute the persistence diagram of the complex
	if (library[0] == 'D') {
		computePersistenceDionysus< Persistence >(f, Smplx::DataEvaluator(),
				maxdimension, FUNvalues, location, printProgress,
				persDgm, persLoc, persCycle);
	}
	if (library[0] == 'P') {
		 computePersistencePhat(f, maxdimension, FUNvalues, location,
			 printProgress, persDgm, persLoc);
	}

	// Output persistent diagram
	return Rcpp::List::create(
		concatStlToRcpp< Rcpp::NumericMatrix >(persDgm, true, 3),
		concatStlToRcpp< Rcpp::NumericMatrix >(persLoc, false, 2),
		StlToRcppList< Rcpp::List, Rcpp::NumericVector >(persCycle));
}



template< typename RealMatrix >
inline Rcpp::List
RipsDiagL2Dionysus(const RealMatrix & X 
                 , const int          maxdimension
                 , const double       maxscale
                 , const bool         location
	             , const bool         printProgress
	) {
	std::vector< std::vector< std::vector< double > > > persDgm;
	std::vector< std::vector< std::vector< unsigned > > > persLoc;
	std::vector< std::vector< std::set< unsigned > > > persCycle;
	
	PointContainer points = RcppToStl< PointContainer >(X);
	//read_points(infilename, points);

	PairDistances           distances(points);
	Generator               rips(distances);
	Generator::Evaluator    size(distances);
	FltrR                   f;

	// Generate 2-skeleton of the Rips complex for epsilon = 50
	rips.generate(maxdimension + 1, maxscale, make_push_back_functor(f));

	if (printProgress) {
		Rprintf("# Generated complex of size: %d \n", f.size());
	}

	// Sort the simplices with respect to distance 
	f.sort(Generator::Comparison(distances));

	// Compute the persistence diagram of the complex
	computePersistenceDionysus< PersistenceR >(f, size, maxdimension,
			Rcpp::NumericVector(X.nrow()), location, printProgress,
			persDgm, persLoc, persCycle);

	// Output persistent diagram
	return Rcpp::List::create(
			concatStlToRcpp< Rcpp::NumericMatrix >(persDgm, true, 3),
			concatStlToRcpp< Rcpp::NumericMatrix >(persLoc, false, 2),
			StlToRcppList< Rcpp::List, Rcpp::NumericVector >(persCycle));
}



template< typename RealMatrix >
inline Rcpp::List
RipsDiagArbitDionysus(const RealMatrix & X
                    , const int          maxdimension
                    , const double       maxscale
                    , const bool         location
                    , const bool         printProgress
	) {
	std::vector< std::vector< std::vector< double > > > persDgm;
	std::vector< std::vector< std::vector< unsigned > > > persLoc;
	std::vector< std::vector< std::set< unsigned > > > persCycle;

	PointContainer points = RcppToStl< PointContainer >(X, true);
	//read_points2(infilename, points);

	PairDistancesA           distances(points);
	GeneratorA               rips(distances);
	GeneratorA::Evaluator    size(distances);
	FltrRA                   f;

	// Generate 2-skeleton of the Rips complex for epsilon = 50
	rips.generate(maxdimension + 1, maxscale, make_push_back_functor(f));

	if (printProgress) {
		Rprintf("# Generated complex of size: %d \n", f.size());
	}

	// Sort the simplices with respect to distance 
	f.sort(GeneratorA::Comparison(distances));

	// Compute the persistence diagram of the complex
	computePersistenceDionysus< PersistenceR >(f, size, maxdimension,
			Rcpp::NumericVector(X.nrow()), location, printProgress,
			persDgm, persLoc, persCycle);

	// Output persistent diagram
	return Rcpp::List::create(
			concatStlToRcpp< Rcpp::NumericMatrix >(persDgm, true, 3),
			concatStlToRcpp< Rcpp::NumericMatrix >(persLoc, false, 2),
			StlToRcppList< Rcpp::List, Rcpp::NumericVector >(persCycle));
}



// [[Rcpp::export]]
double Bottleneck(
		const Rcpp::NumericMatrix& Diag1, const Rcpp::NumericMatrix& Diag2) {

	return bottleneck_distance(RcppToDionysus< PersistenceDiagram<> >(Diag1),
			RcppToDionysus< PersistenceDiagram<> >(Diag2));
}



// [[Rcpp::export]]
double Wasserstein(const Rcpp::NumericMatrix& Diag1,
		const Rcpp::NumericMatrix& Diag2, const int p) {

	return wasserstein_distance(RcppToDionysus< PersistenceDiagram<> >(Diag1),
			RcppToDionysus< PersistenceDiagram<> >(Diag2), p);
}



// KDE function on a Grid
// [[Rcpp::export]]
Rcpp::NumericVector Kde(const Rcpp::NumericMatrix& X,
		const Rcpp::NumericMatrix& Grid, const double h,
		const Rcpp::NumericVector& weight, const bool printProgress) {

	const double pi = 3.141592653589793;
	const unsigned dimension = Grid.ncol();
	const unsigned gridNum = Grid.nrow();
	const double den = pow(h, (int)dimension) * pow(2 * pi, dimension / 2.0);
	Rcpp::NumericVector kdeValue;
	int counter = 0, percentageFloor = 0;
	int totalCount = gridNum;

	if (printProgress) {
		printProgressFrame(Rprintf);
	}

	if (dimension <= 1) {
		kdeValue = computeKernel< Rcpp::NumericVector >(
				X, Grid, h, weight, printProgress, Rprintf, counter, totalCount,
				percentageFloor);
	}
	else {
		kdeValue = computeGaussOuter< Rcpp::NumericVector >(
				X, Grid, h, weight, printProgress, Rprintf, counter, totalCount,
				percentageFloor);

	}

	for (unsigned gridIdx = 0; gridIdx < gridNum; ++gridIdx) {
		kdeValue[gridIdx] /= den;
	}

	if (printProgress) {
		Rprintf("\n");
	}

	return kdeValue;
}



// kernel Dist function on a Grid
// [[Rcpp::export]]
Rcpp::NumericVector KdeDist(const Rcpp::NumericMatrix& X,
		const Rcpp::NumericMatrix& Grid, const double h,
		const Rcpp::NumericVector& weight, const bool printProgress) {

	const unsigned sampleNum = X.nrow();
	const unsigned dimension = Grid.ncol();
	const unsigned gridNum = Grid.nrow();
	// first = sum K_h(X_i, X_j), second = K_h(x, x), third = sum K_h(x, X_i)
	std::vector< double > firstValue;
	const double second = 1.0;
	std::vector< double > thirdValue;
	double firstmean;
	Rcpp::NumericVector kdeDistValue(gridNum);
	int counter = 0, percentageFloor = 0;
	int totalCount = sampleNum + gridNum;

	if (printProgress) {
		printProgressFrame(Rprintf);
	}

	firstValue = computeKernel< std::vector< double > >(
			X, X, h, weight, printProgress, Rprintf, counter, totalCount,
			percentageFloor);

	if (dimension <= 1) {
		thirdValue = computeKernel< std::vector< double > >(
				X, Grid, h, weight, printProgress, Rprintf, counter, totalCount,
				percentageFloor);
	}
	else {
		thirdValue = computeGaussOuter< std::vector< double > >(
				X, Grid, h, weight, printProgress, Rprintf, counter, totalCount,
				percentageFloor);
	}

	if (weight.size() == 1) {
		firstmean = std::accumulate(firstValue.begin(), firstValue.end(), 0.0) / sampleNum;
	}
	else {
		firstmean = std::inner_product(
				firstValue.begin(), firstValue.end(), weight.begin(), 0.0) / 
				std::accumulate(weight.begin(), weight.end(), 0.0);
	}

	for (unsigned gridIdx = 0; gridIdx < gridNum; ++gridIdx) {
		kdeDistValue[gridIdx] = std::sqrt(firstmean + second - 2 * thirdValue[gridIdx]);
	}

	if (printProgress) {
		Rprintf("\n");
	}

	return kdeDistValue;
}



// distance to measure function on a Grid
// [[Rcpp::export]]
Rcpp::NumericVector Dtm(const Rcpp::NumericMatrix& knnIndex,
		const Rcpp::NumericMatrix& knnDistance,
		const Rcpp::NumericVector& weight, const double weightBound) {

	const unsigned gridNum = knnIndex.nrow();
	double distanceTemp, weightTemp, weightSumTemp;
	Rcpp::NumericVector dtmValue(gridNum, 0.0);

	for (unsigned gridIdx = 0; gridIdx < gridNum; ++gridIdx) {
		weightSumTemp = 0.0;
		for (unsigned kIdx = 0; weightSumTemp < weightBound; ++kIdx) {
			distanceTemp = knnDistance[gridIdx + kIdx * gridNum];
			weightTemp = std::min(weight[knnIndex[gridIdx + kIdx * gridNum] - 1],
					weightBound - weightSumTemp);
			weightSumTemp += weightTemp;
			dtmValue[gridIdx] += distanceTemp * distanceTemp * weightTemp;
		}
		dtmValue[gridIdx] = std::sqrt(dtmValue[gridIdx] / weightBound);
	}
	return (dtmValue);
}



// RipsDiag in GUDHI
/** \brief Interface for R code, construct the persistence diagram
* of the Rips complex constructed on the input set of points.
*
* @param[out] void            every function called by R must return void
* @param[in]  point           pointer toward the coordinates of all points. Format
*                             must be X11 X12 ... X1d X21 X22 ... X2d X31 ...
* @param[in]  dim             embedding dimension
* @param[in]  num_points      number of points. The input point * must be a
*                             pointer toward num_points*dim double exactly.
* @param[in]  rips_threshold  threshold for the Rips complex
* @param[in]  max_complex_dim maximal dimension of the Rips complex
* @param[in]  diagram         where to output the diagram. The format must be dimension birth death.
* @param[in]  max_num_bars    write the max_num_pairs most persistent pairs of the
*                             diagram. diagram must point to enough memory space for
*                             3*max_num_pairs double. If there is not enough pairs in the diagram,
*                             write nothing after.
*/
inline Rcpp::List
RipsDiagGUDHI(const Rcpp::NumericMatrix & X          //points to some memory space
            , const int                   maxdimension
            , const double                maxscale
            , const bool                  printProgress
	) {
	std::vector< std::vector< std::vector< double > > > persDgm;
	std::vector< std::vector< std::vector< unsigned > > > persLoc;
	std::vector< std::vector< std::set< unsigned > > > persCycle;

	// Turn the input points into a range of points
	typedef std::vector< double > Point_t;
	std::vector< Point_t > point_set =
			RcppToStl< std::vector< Point_t > >(X);


	// Compute the proximity graph of the points
	Graph_t prox_graph = compute_proximity_graph(point_set, maxscale
		, euclidean_distance<Point_t>);

	// Construct the Rips complex in a Simplex Tree
	Gudhi::Simplex_tree<> st;
	st.insert_graph(prox_graph); // insert the proximity graph in the simplex tree
	st.expansion(maxdimension + 1); // expand the graph until dimension dim_max

	if (printProgress) {
		Rprintf("# Generated complex of size: %d \n", st.num_simplices());
		// std::cout << st.num_simplices() << " simplices \n";
	}

	// Sort the simplices in the order of the filtration
	st.initialize_filtration();

	// Compute the persistence diagram of the complex
	int p = 2; //characteristic of the coefficient field for homology
	double min_persistence = 0; //minimal length for persistent intervals
	computePersistenceGUDHI(st, p, min_persistence, maxdimension, persDgm);

	// Output persistent diagram
	return Rcpp::List::create(
			concatStlToRcpp< Rcpp::NumericMatrix >(persDgm, true, 3),
			concatStlToRcpp< Rcpp::NumericMatrix >(persLoc, false, 2),
			StlToRcppList< Rcpp::List, Rcpp::NumericVector >(persCycle));
}



// RipsDiag
/** \brief Interface for R code, construct the persistence diagram
* of the Rips complex constructed on the input set of points.
*
* @param[out] void            every function called by R must return void
* @param[in]  point           pointer toward the coordinates of all points. Format
*                             must be X11 X12 ... X1d X21 X22 ... X2d X31 ...
* @param[in]  dim             embedding dimension
* @param[in]  num_points      number of points. The input point * must be a
*                             pointer toward num_points*dim double exactly.
* @param[in]  rips_threshold  threshold for the Rips complex
* @param[in]  max_complex_dim maximal dimension of the Rips complex
* @param[in]  diagram         where to output the diagram. The format must be dimension birth death.
* @param[in]  max_num_bars    write the max_num_pairs most persistent pairs of the
*                             diagram. diagram must point to enough memory space for
*                             3*max_num_pairs double. If there is not enough pairs in the diagram,
*                             write nothing after.
*/
// [[Rcpp::export]]
Rcpp::List
RipsDiag(const Rcpp::NumericMatrix & X
       , const int                   maxdimension
       , const double                maxscale
       , const std::string         & dist
       , const std::string         & library
       , const bool                  location
       , const bool                  printProgress
	) {
	if (library[0] == 'G') {
		return RipsDiagGUDHI(X, maxdimension, maxscale, printProgress);
	}
	if (dist[0] == 'e') {
		return RipsDiagL2Dionysus(X, maxdimension, maxscale,
				location, printProgress);
	}
	else {
		return RipsDiagArbitDionysus(X, maxdimension, maxscale,
				location, printProgress);
	}
}



//---------------------------------------------------------------------------------------------------------------------
// gudhi type definition
typedef Gudhi::Simplex_tree<>::Vertex_handle Simplex_tree_vertex;
typedef std::map<Alpha_shape_3::Vertex_handle, Simplex_tree_vertex > Alpha_shape_simplex_tree_map;
typedef std::pair<Alpha_shape_3::Vertex_handle, Simplex_tree_vertex> Alpha_shape_simplex_tree_pair;
typedef std::vector< Simplex_tree_vertex > Simplex_tree_vector_vertex;

Vertex_list fromCell(const Cell_handle& ch)
{
	Vertex_list the_list;
	for (auto i = 0; i < 4; i++)
	{
		the_list.push_back(ch->vertex(i));
	}
	return the_list;
}
Vertex_list fromFacet(const Facet& fct)
{
	Vertex_list the_list;
	for (auto i = 0; i < 4; i++)
	{
		if (fct.second != i)
		{
			the_list.push_back(fct.first->vertex(i));
		}
	}
	return the_list;
}
Vertex_list fromEdge(const Edge_3& edg)
{
	Vertex_list the_list;
	for (auto i = 0; i < 4; i++)
	{
		if ((edg.second == i) || (edg.third == i))
		{
			the_list.push_back(edg.first->vertex(i));
		}
	}
	return the_list;
}
Vertex_list fromVertex(const Alpha_shape_3::Vertex_handle& vh)
{
	Vertex_list the_list;
	the_list.push_back(vh);
	return the_list;
}



	// GUDHI RIPS
	/** \brief Interface for R code, construct the persistence diagram
	  * of the Rips complex constructed on the input set of points.
	  *
	  * @param[out] void            every function called by R must return void
	  * @param[in]  point           pointer toward the coordinates of all points. Format
	  *                             must be X11 X12 ... X1d X21 X22 ... X2d X31 ...
	  * @param[in]  dim             embedding dimension
	  * @param[in]  num_points      number of points. The input point * must be a
	  *                             pointer toward num_points*dim double exactly.
	  * @param[in]  max_complex_dim maximal dimension of the Rips complex
	  * @param[in]  diagram         where to output the diagram. The format must be dimension birth death.
	  * @param[in]  max_num_bars    write the max_num_pairs most persistent pairs of the
	  *                             diagram. diagram must point to enough memory space for
	  *                             3*max_num_pairs double. If there is not enough pairs in the diagram,
	  *                             write nothing after.
	  */
// [[Rcpp::export]]
Rcpp::List
AlphaDiagGUDHI(const Rcpp::NumericMatrix & X          //points to some memory space
             , const bool                  printProgress
	) {
	std::vector< std::vector< std::vector< double > > > persDgm;
	std::vector< std::vector< std::vector< unsigned > > > persLoc;
	std::vector< std::vector< std::set< unsigned > > > persCycle;

	  int coeff_field_characteristic = 2;

	  float min_persistence = 0.0;



	  // Turn the input points into a range of points
	  std::list<Point_3> lp = RcppToStlPoint3< std::list<Point_3> >(X);


	  // alpha shape construction from points. CGAL has a strange behavior in REGULARIZED mode.
	  Alpha_shape_3 as(lp.begin(),lp.end(),0,Alpha_shape_3::GENERAL);
	  //std::cout << "Alpha shape computed in GENERAL mode" << std::endl;

	  // filtration with alpha values from alpha shape
	  std::vector<Object> the_objects;
	  std::vector<Alpha_value_type> the_alpha_values;

	  Dispatch disp = CGAL::dispatch_output<Object, Alpha_value_type>( std::back_inserter(the_objects), std::back_inserter(the_alpha_values));

	  as.filtration_with_alpha_values(disp);
	  //std::cout << "filtration_with_alpha_values returns : " << the_objects.size() << " objects" << std::endl;

	  Alpha_shape_3::size_type count_vertices = 0;
	  Alpha_shape_3::size_type count_edges    = 0;
	  Alpha_shape_3::size_type count_facets   = 0;
	  Alpha_shape_3::size_type count_cells    = 0;

	  // Loop on objects vector
	  Vertex_list vertex_list;
	  Gudhi::Simplex_tree<> simplex_tree;
	  Alpha_shape_simplex_tree_map map_cgal_simplex_tree;
	  std::vector<Alpha_value_type>::iterator the_alpha_value_iterator = the_alpha_values.begin();
	  int dim_max=0;
	  Filtration_value filtration_max=0.0;
	  for(auto object_iterator: the_objects)
	  {
	    // Retrieve Alpha shape vertex list from object
	    if (const Cell_handle* cell = CGAL::object_cast<Cell_handle>(&object_iterator))
	    {
	      vertex_list = fromCell(*cell);
	      count_cells++;
	      if (dim_max < 3) {
	        dim_max=3; // Cell is of dim 3
	      }
	    }
	    else if (const Facet* facet = CGAL::object_cast<Facet>(&object_iterator))
	    {
	      vertex_list = fromFacet(*facet);
	      count_facets++;
	      if (dim_max < 2) {
	        dim_max=2; // Facet is of dim 2
	      }
	    }
	    else if (const Edge_3* edge = CGAL::object_cast<Edge_3>(&object_iterator))
	    {
	      vertex_list = fromEdge(*edge);
	      count_edges++;
	      if (dim_max < 1) {
	        dim_max=1; // Edge_3 is of dim 1
	      }
	    }
	    else if (const Alpha_shape_3::Vertex_handle* vertex = CGAL::object_cast<Alpha_shape_3::Vertex_handle>(&object_iterator))
	    {
	      count_vertices++;
	      vertex_list = fromVertex(*vertex);
	    }
	    // Construction of the vector of simplex_tree vertex from list of alpha_shapes vertex
	    Simplex_tree_vector_vertex the_simplex_tree;
	    for (auto the_alpha_shape_vertex:vertex_list)
	    {
	      Alpha_shape_simplex_tree_map::iterator the_map_iterator = map_cgal_simplex_tree.find(the_alpha_shape_vertex);
	      if (the_map_iterator == map_cgal_simplex_tree.end())
	      {
	        // alpha shape not found
	        Simplex_tree_vertex vertex = map_cgal_simplex_tree.size();
	        //std::cout << "vertex [" << the_alpha_shape_vertex->point() << "] not found - insert " << vertex << std::endl;
	        the_simplex_tree.push_back(vertex);
	        map_cgal_simplex_tree.insert(Alpha_shape_simplex_tree_pair(the_alpha_shape_vertex,vertex));
	      } else
	      {
	        // alpha shape found
	        Simplex_tree_vertex vertex = the_map_iterator->second;
	        //std::cout << "vertex [" << the_alpha_shape_vertex->point() << "] found in " << vertex << std::endl;
	        the_simplex_tree.push_back(vertex);
	      }
	    }
	    // Construction of the simplex_tree
	    Filtration_value filtr = std::sqrt(*the_alpha_value_iterator);
	    //std::cout << "filtration = " << filtr << std::endl;
	    if (filtr > filtration_max) {
	      filtration_max = filtr;
	    }
	    simplex_tree.insert(the_simplex_tree, filtr);
		if (the_alpha_value_iterator != the_alpha_values.end()) {
		  ++the_alpha_value_iterator;
		}
		else {
		  //std::cout << "This shall not happen" << std::endl;
		}
	  }
	  simplex_tree.set_filtration(filtration_max);
	  simplex_tree.set_num_simplices(count_vertices + count_edges + count_facets + count_cells);
	  simplex_tree.set_dimension(dim_max);

	  /*std::cout << "vertices \t\t" << count_vertices << std::endl;
	  std::cout << "edges \t\t"    << count_edges << std::endl;
	  std::cout << "facets \t\t"   << count_facets << std::endl;
	  std::cout << "cells \t\t"    << count_cells << std::endl;


	  std::cout << "Information of the Simplex Tree: " << std::endl;
	  std::cout << "  Number of vertices = " << simplex_tree.num_vertices() << " ";
	  std::cout << "  Number of simplices = " << simplex_tree.num_simplices() << std::endl << std::endl;
	  std::cout << "  Dimension = " << simplex_tree.dimension() << " ";
	  std::cout << "  filtration = " << simplex_tree.filtration() << std::endl << std::endl;
	  std::cout << "Iterator on vertices: " << std::endl;
	  for( auto vertex : simplex_tree.complex_vertex_range() )
	  { std::cout << vertex << " "; }*/

	  // Sort the simplices in the order of the filtration
	  simplex_tree.initialize_filtration();

	  //std::cout << "Simplex_tree dim: " << simplex_tree.dimension() << std::endl;

	// Compute the persistence diagram of the complex
	computePersistenceGUDHI(simplex_tree, coeff_field_characteristic,
			min_persistence, 2, persDgm);

	// Output persistent diagram
	return Rcpp::List::create(
			concatStlToRcpp< Rcpp::NumericMatrix >(persDgm, true, 3),
			concatStlToRcpp< Rcpp::NumericMatrix >(persLoc, false, 2),
			StlToRcppList< Rcpp::List, Rcpp::NumericVector >(persCycle));
}

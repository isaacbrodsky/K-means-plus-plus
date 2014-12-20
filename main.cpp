//***********************************************************************
// main.cpp for k-means++
//
//   this is an implementation of the K-means classification algorithm.
//   it accepts vectors of multiple attributes (with classifications) and clusters them
//	 into K clusters
//
// INVOKE APPLICATION USING: k-means++ <control file name>
//
// INPUTS: (from disk file)
//        <control.txt> - control file
//			   the control file can have any name - specified at application command
//
//			   Format: <control label> <value> these can be in any order
//
//			   control labels: #k-count, #input-filename, #output-filename, #use-labels,
//				 #tolerance, #plus-plus, #plus-plus-random-seed, #num-threads, #EOF
//
//             values: k value = integer, input datafile name = string,
//				 output datafile name = string, use data labels = boolean (1, 0),
//				 stopping tolerance value = float, number of threads = integer, eof = no value
//
//        <datafile.dat> - classification set - filename specified in the control file
//             attribute count - don't include the classification in
//                                 the count
//             data - space delimited, classification (can be empty)
//
// OUTPUTS: (to disk file)
//        <output_filename> - space delimited - filename specified in the control file
//             for each cluster:
//					the cluster mean value (by attribute) and the count of clustered instances
//					listing of each clustered instance with its classification
//
//***********************************************************************
//  WARNING: none
//
//***********************************************************************
// IMPLEMENTATION NOTE: all files are in the executable working directory
//
//***********************************************************************
// created by: j. aleshunas
// created on: 9 nov 04
// modified on: 16 nov 14
//
// © 2004 John Aleshunas
// Copyright 2014 Isaac Brodsky
//
//***********************************************************************

#include "k-means-multi.h"

int main(int argc, char *argv[]) {

	// local variables
	Cluster_set clCluster_set_instance;

	// check if there are any command line arguments
	if (argc < 2) { // missing input argument
		cout << "No control file specified" << endl << endl;
		cout << "Required input format is: k-means++ <control file name>" << endl << endl;
		return 1;
	}
	else { // input argument present
		// read the parameter data from the input file
		clCluster_set_instance.Read_control_data(argv[1]);

		// start the clustering program
		clCluster_set_instance.Execute_clustering();
	} // if

	return 0;  // that's all folks
} // main()


//***********************************************************************
// main.cpp for k-means-multi
//
//   this is an implementation of the K-means classification algorithm. 
//   it accepts vectors of multiple attributes (with classifications) and clusters them 
//	 into K clusters
//
// INPUTS: (from disk file)
//        control.dat - control file - space delimited (optional)
//             k value, input datafile name, output datafile name, data labels boolean flag, 
//				stopping tolerance value
//
//        datafile.dat - classification set
//             attribute count - don't include the classification in
//                                 the count
//             data - space delimited, classification (can be empty)
//
// OUTPUTS: (to disk file)
//        datafile_cl.out - space delimited
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
// modified on: 22 jan 07
//
// © 2004 John Aleshunas
//
//***********************************************************************

#include "k-means-multi.h"

int main(int argc, char *argv[]) {

	// local variables
	Cluster_set clCluster_set_instance;
	int iArg_val;
	char cPause;

	// check if there are any command line arguments
	iArg_val = argc;  
	if (iArg_val < 2) { // missing input argument
		cout << "No control file specified" << endl << endl;
		cout << "Required input format is: k-means-multi <control file name>" << endl << endl;
	}
	else { // input argument present
		// read the parameter data from the input file 
		clCluster_set_instance.Read_control_data(argv[1]);
		// next line is for testing
		//clCluster_set_instance.Read_control_data("control_iris.dat");

		// start the clustering program
		clCluster_set_instance.Execute_clustering();
	} // if 

	return(1);  // that's all folks
} // main()


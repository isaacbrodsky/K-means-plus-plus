//***********************************************************************
// k-means-multi.h
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
//				 #tolerance, #plus-plus, #plus-plus-random-seed #EOF
//
//             values: k value = integer, input datafile name = string,
//				 output datafile name = string, use data labels = boolean (1, 0),
//				 stopping tolerance value = float, eof = no value
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

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>

using namespace std;

//***********************************************************************
// class Cluster_instance declaration
// An instance of data in the clustering system.
//***********************************************************************
class Cluster_instance {

	// private class variables

	// private methods

public:
	// public class variables
	vector<float> vfAttribute;
	string sClassification;

	// public methods
	Cluster_instance(void); // constructor

}; // class Cluster_instance

//***********************************************************************
// class Cluster declaration
//***********************************************************************
class Cluster {

	// private class variables

	// private methods

public:
	// public class variables
	vector<Cluster_instance> vclThe_cluster;

	// public methods
	Cluster(void); // constructor

}; // class Cluster

//***********************************************************************
// class Cluster_set declaration
//***********************************************************************
class Cluster_set {

	// private class variables
	vector<Cluster> vclThe_cluster_set;
	vector< vector<float> > vvfMeans;
	vector< vector<float> > vvfOld_means;
	int iK_count;
	string sIn_file;
	string sOut_file;
	float fTolerance;
	Cluster vclInput_data;
	int iIteration;
	int iAttribute_ct;
	bool bUseLabels;
	bool bUsePlusPlus;
	mt19937 mtRandom;

	// private methods
	void Read_input_data(void);
	void Write_output_data(void);
	void Setup_cluster_set(void);
	void Initialize_plus_plus(void);
	void Identify_mean_values(void);
	void Cluster_data(void);
	void Calculate_cluster_means(void);
	bool Compare_mean_values(void);

public:
	// public class variables

	// public methods
	Cluster_set(void); // constructor
	void Read_control_data(string sControlFilename);
	void Execute_clustering(void);

}; // class Cluster_set


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
//				 #tolerance, #plus-plus, #plus-plus-threads, #plus-plus-random-seed,
//				 #num-threads, #EOF
//
//             values: k value = integer, input datafile name = string,
//				 output datafile name = string, use data labels = boolean (1, 0),
//				 stopping tolerance value = float, use k-means++ = boolean (1, 0),
//				 number of k-means++ threads = integer, random seed for k-means++ = integer,
//				 number of threads = integer, eof = no value
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
// modified on: 20 dec 14
//
// © 2004 John Aleshunas
// Copyright 2014 Isaac Brodsky
//
//***********************************************************************

#include <iostream>
#include <string>
#include <vector>
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
	int iCluster;

	// public methods
	Cluster_instance(void); // constructor

}; // class Cluster_instance

//***********************************************************************
// class Cluster_set declaration
//***********************************************************************
class Cluster_set {

	// private class variables
	vector< vector<float> > vvfMeans;
	vector< vector<float> > vvfOld_means;
	int iK_count;
	string sIn_file;
	string sOut_file;
	float fTolerance;
	vector<Cluster_instance> vclInput_data;
	int iIteration;
	int iAttribute_ct;
	bool bUseLabels;
	bool bUsePlusPlus;
	mt19937 mtRandom;
	int iNumPlusPlusThreads;
	int iNumThreads;

	// private methods
	bool Read_input_data(void);
	void Write_output_data(void);
	float Initialize_plus_plus_process(unsigned uIndex, unsigned uLength, int iSelectedPoints, vector<float> *vfDistance, const vector<bool>& vbSkipPoints);
	void Initialize_plus_plus(void);
	void Identify_mean_values(void);
	void Cluster_data(void);
	void Cluster_data_process(unsigned uIndex, unsigned uLength);
	void Calculate_cluster_means(void);
	bool Compare_mean_values(void);

public:
	// public class variables

	// public methods
	Cluster_set(void); // constructor
	void Read_control_data(string sControlFilename);
	void Execute_clustering(void);

}; // class Cluster_set


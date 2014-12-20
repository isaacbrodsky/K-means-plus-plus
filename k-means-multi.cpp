//***********************************************************************
// k-means-multi.cpp
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
#include <fstream>
#include <thread>

//***********************************************************************
// class Cluster_instance method declarations
//***********************************************************************
// class Cluster_instance constructor
Cluster_instance::Cluster_instance(void){

	// make room for the attributes
	//vvfAttributes.resize(iAttribute_ct);

	return;
} //Cluster_instance::Cluster_instance

//***********************************************************************
// class Cluster_set public method declarations
//***********************************************************************
// class Cluster_set constructor
Cluster_set::Cluster_set(void){

	// local variables
	random_device rd;

	// initialize the class variables
	iIteration = 0;
	iK_count = 1;
	sIn_file = "default_in.dat";
	sOut_file = "default_out.txt";
	bUseLabels = false;
	fTolerance = 0.1f;
	// Use K-means++ by default.
	bUsePlusPlus = true;
	// Seed with real random value if available
	mtRandom = mt19937(rd());
	iNumThreads = 1;

	return;
} //Cluster_set::Cluster_set

//***********************************************************************
void Cluster_set::Read_control_data(string sControlFilename) {

	// local variables
	string sTitle;
	bool bNot_done;
	unsigned int uRandomSeed;

	// initialize loop flag
	bNot_done = true;

	// declare an input stream to read the data
	ifstream strInput_stream;

	// open the input stream to read the key
	strInput_stream.open(sControlFilename.c_str());

	// check if the file was OK
	if (strInput_stream.is_open()){
		while(bNot_done){ // read to the end of the control file

			strInput_stream >> sTitle; // read the control label
			if (strInput_stream.fail() // Check for failed read
				|| sTitle == "#EOF") { // read the end of file label
				bNot_done =  false;
			} // if
			else if (sTitle == "#k-count"){ // read the count of neighbors
				strInput_stream >> iK_count;
			} // if
			else if (sTitle == "#input-filename"){ // read the input filename
				strInput_stream >> sIn_file;
			} // if
			else if (sTitle == "#output-filename"){ // read the output filename
				strInput_stream >> sOut_file;
			} // if
			else if (sTitle == "#use-labels"){ // read the use-labels flag
				strInput_stream >> bUseLabels;
			} // if
			else if (sTitle == "#tolerance"){ // read the stopping criteria
				strInput_stream >> fTolerance;
			} // if
			else if (sTitle == "#plus-plus"){ // control k-means++ initialization
				strInput_stream >> bUsePlusPlus;
			} // if
			else if (sTitle == "#plus-plus-random-seed"){ // Specify k-means++ random seed
				strInput_stream >> uRandomSeed;
				mtRandom.seed(uRandomSeed);
			} // if
			else if (sTitle == "#num-threads"){ // Number of parallel threads
				strInput_stream >> iNumThreads;
			} // if
			else{
				cout << "Unrecognized directive in control file." << endl;
			}
		} // while read control file
	} // if control file open
	else { // print error message
		cout << "Error reading the file " << sControlFilename << endl << endl;
	}

	strInput_stream.close();  // close filestream

	return;
} // Cluster_set::Read_control_file

//***********************************************************************
void Cluster_set::Execute_clustering(void){

	// local variables
	bool bNot_done = true;

	// read the input data
	if (Read_input_data()) {

		// loop until we are done clustering - the mean values don't change
		while (bNot_done){

			// identify the k means values
			Identify_mean_values();

			// cluster the input data using the k means values
			Cluster_data();

			// calculate the means of the clusters
			Calculate_cluster_means();

			// compare the old mean values to the new mean values
			// if the difference is less than the tolerance value then stop clustering
			bNot_done = Compare_mean_values();

			// increment the iteration
			iIteration++;

			// end the main loop
		} // while

		// write the output data
		Write_output_data();
	} // If input data read

	return;
} // Cluster_set::Execute_clustering

//***********************************************************************
// class Cluster_set private method declarations
//***********************************************************************

//***********************************************************************
// Returns true on success
bool Cluster_set::Read_input_data(void){

	// local variables
	int iAttribute_index, iCluster_index;
	float fInput_attribute;
	bool bResult;

	// declare an input stream to read the key
	ifstream strInput_stream;

	// declare data storage for the input data
	Cluster_instance clInput_instance;

	// open the input stream to read the key
	strInput_stream.open(sIn_file.c_str());

	// check if the file was OK
	if (strInput_stream.is_open()){

		strInput_stream >> iAttribute_ct;

		clInput_instance.vfAttribute.resize(iAttribute_ct);

		while (!strInput_stream.eof()){

			for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
				// get the attribute
				strInput_stream >> fInput_attribute;
				clInput_instance.vfAttribute[iAttribute_index] = fInput_attribute;
			} // for

			if (bUseLabels) {
				// read the classification name
				strInput_stream >> clInput_instance.sClassification;
			}
			else {
				clInput_instance.sClassification = "BLANK";
			} // if
			
			if (strInput_stream.fail()) {
				// Failed to read the input.
				// (E.g. hit the end of the file.)
				break;
			}

			// save the data in the vector set
			vclInput_data.push_back(clInput_instance);

		}// while

		// allocate memory for the mean storage
		vvfMeans.resize(iK_count);
		vvfOld_means.resize(iK_count);

		for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){
			// get the attribute
			vvfMeans[iCluster_index].resize(iAttribute_ct);
			vvfOld_means[iCluster_index].resize(iAttribute_ct);
		} // for

		bResult = true;
	} //if
	else {
		cout << "Error reading " << sIn_file << endl << endl;
		bResult = false;
	}

	strInput_stream.close();

	return bResult;
} //Cluster_set::Read_input_data

//***********************************************************************
void Cluster_set::Write_output_data(void){

	// local variables
	unsigned uInstance_index;
	int iCluster_index, iAttribute_index;
	vector< vector<Cluster_instance> > vclThe_cluster_set(iK_count);

	// declare an output stream
	ofstream strResults_out_stream;

	// open the stream to write the output plaintext
	strResults_out_stream.open(sOut_file.c_str());

	//Build the cluster results
	for (uInstance_index = 0; uInstance_index < vclInput_data.size(); uInstance_index++)
	{
		vclThe_cluster_set[vclInput_data[uInstance_index].iCluster].push_back(vclInput_data[uInstance_index]);
	}

	if(iK_count < 1) { // we have an empty cluster_set
		cout << endl << "No clusters to send to output file!" << endl << endl;
	} else { // output the cluster results
		// loop thru each cluster
		for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){

			// output a cluster header - cluster #, cluster mean
			strResults_out_stream << "Cluster #" << iCluster_index + 1 << " with mean ";
			for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
				strResults_out_stream << vvfMeans[iCluster_index][iAttribute_index] << " ";
			} // for
			strResults_out_stream << " and member count "
				<< vclThe_cluster_set[iCluster_index].size() << endl;

			// loop thru the cluster members
			for (uInstance_index = 0;
				uInstance_index < vclThe_cluster_set[iCluster_index].size(); uInstance_index++){

				// output the cluster member data
				for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
					strResults_out_stream
						<< vclThe_cluster_set[iCluster_index][uInstance_index].vfAttribute[iAttribute_index];
					strResults_out_stream << " ";
				} // for
				strResults_out_stream << " ";
				strResults_out_stream << vclThe_cluster_set[iCluster_index][uInstance_index].sClassification;

				// output a CR/LF
				strResults_out_stream << endl;
			} // for

			// output CR/LF
			strResults_out_stream << endl;
		} // for
	} // if

	strResults_out_stream.close();

	return;
} // Cluster_set::Write_output_data

//***********************************************************************
void Cluster_set::Initialize_plus_plus(void) {
	// Initializes using K-means++.

	// Number of instances in the input data
	size_t szData = vclInput_data.size();
	size_t uIndex;
	// Points that have already been selected as starting points.
	// true indicates already selected, false not selected
	// as a starting point.
	vector<bool> vbSkipPoints;
	vector<float> vfDistance;
	int iSelectedPoints;
	int iAttribute_index;
	int iK_index;
	float fTotalDistance;
	float fDifference;
	float fSum_of_squares;
	float fRandomDistance;

	vbSkipPoints.resize(szData);
	vfDistance.resize(szData);

	// Select the first data instance as the initial mean
	// (by copying it into the list of means and marking
	// it as selected in our list of points to skip.)
	for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){ // read attributes
		vvfMeans[0][iAttribute_index]
			= vclInput_data[0].vfAttribute[iAttribute_index];
	} // for
	vbSkipPoints[0] = true;
	iSelectedPoints = 1;

	// While we don't have enough starting clusters
	for (; iSelectedPoints < iK_count; iSelectedPoints++) {
		fTotalDistance = 0;
		
		// Compute distance to nearest cluster for all data instances.
		for (uIndex = 0; uIndex < szData; uIndex++) {
			// Skip if already selected as a starting point
			if (!vbSkipPoints[uIndex]) {
				// Compute distance to nearest mean
				for (iK_index = 0; iK_index < iSelectedPoints; iK_index++) {
					fSum_of_squares = 0;

					for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++) {
						fDifference = (vclInput_data[uIndex].vfAttribute[iAttribute_index] - vvfMeans[iK_index][iAttribute_index]);
						fSum_of_squares += (fDifference * fDifference);
					} // for

					// If this is the first evaluated distance, use it.
					// Otherwise only use the distance if it is better
					// than the previous best distance.
					if (iK_index == 0 || fSum_of_squares < vfDistance[uIndex]) {
						vfDistance[uIndex] = fSum_of_squares;
					}
				}

				// Sum the distance of all points to the closest
				// starting points as each one is calculated.
				fTotalDistance += vfDistance[uIndex];
			}
		}

		// Determine which instance to take as a new starting cluster
		// First generate a random number uniformly between [0, fTotalDistance)
		// using mtRandom as the PRNG.
		fRandomDistance = uniform_real_distribution<float>(0, fTotalDistance)(mtRandom);
		for (uIndex = 0; uIndex < szData; uIndex++) {
			// Don't cnsider points already selected as starting points.
			if (!vbSkipPoints[uIndex]) {
				// Data instances with a larger distance will have a higher effect
				// here, biasing the k-means++ algorithm toward selecting them.
				fRandomDistance -= vfDistance[uIndex];
				if (fRandomDistance <= 0) {
					// Select this point as a starting point
					// (by copying it into the means vector
					// and marking it as used in our own
					// vector of points to skip.)
					for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
						vvfMeans[iSelectedPoints][iAttribute_index]
							= vclInput_data[uIndex].vfAttribute[iAttribute_index];
					} // for
					vbSkipPoints[uIndex] = true;
					
					// Stop selecting the next starting point
					break;
				}
			}
		}
	}
} // Cluster_set::Initialize_plus_plus

//***********************************************************************
void Cluster_set::Identify_mean_values(void){

	// local variables
	int iCluster_index, iAttribute_index;
	vector<float> vfValues;

	// allocate memory for the local vector
	vfValues.resize(iAttribute_ct);

	if (iIteration < 1) { // if this is the first iteration - initialize the cluster mean values
		if (bUsePlusPlus) {
			Initialize_plus_plus();
		}
		else { // Use the first k instances.
			for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){ // read K-instances
				for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){ // read attributes
					vvfMeans[iCluster_index][iAttribute_index]
						= vclInput_data[iCluster_index].vfAttribute[iAttribute_index];
				} // for
			} //for
		}
	} // if

	// save the existing mean values
	for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){
		for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){

			// copy the new_means values into the old_means values storage
			vvfOld_means[iCluster_index][iAttribute_index] = vvfMeans[iCluster_index][iAttribute_index];
		} // for
	} //for

	return;
} //Cluster_set::Identify_mean_values

//***********************************************************************
void Cluster_set::Cluster_data_process(unsigned uIndex, unsigned uLength)
{
	// local variables
	float fDifference;
	float fSquared_difference, fSum_of_squares;
	float fBest_squared_difference;
	int iBest_index;
	int iK_index, iAttribute_index;
	Cluster_instance clData_instance;
	unsigned uLast = uIndex + uLength;

	// loop for all the input data values
	for (; uIndex < uLast; uIndex++) {

		// initialize the tracking variables
		fBest_squared_difference = FLT_MAX; // make this erroneously big
		iBest_index = -1; // Forces taking the first value

		// get the next data vector
		clData_instance = vclInput_data[uIndex];

		// loop thru all of the mean values
		for (iK_index = 0; iK_index < iK_count; iK_index++) {

			// initialize the variables
			fDifference = 0;
			fSum_of_squares = 0;

			// compare the data vector to the mean vector
			for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
				// calculate the difference between the data value and the mean value
				fDifference = (clData_instance.vfAttribute[iAttribute_index] - vvfMeans[iK_index][iAttribute_index]);
				// square the difference
				fSquared_difference = fDifference * fDifference;
				// add to the sum of squares
				fSum_of_squares = fSum_of_squares + fSquared_difference;
			} // for

			// if this value is less than the best squared difference (BSD)
			if (iBest_index == -1 || fSum_of_squares < fBest_squared_difference){
				fBest_squared_difference = fSum_of_squares; // save it as BSD

				// and save the this index as best index
				iBest_index = iK_index;

			} // if
		} // for

		clData_instance.iCluster = iBest_index;
	} // for
} //Cluster_set::Cluster_data_process

//***********************************************************************
void Cluster_set::Cluster_data(void){
	if (iNumThreads == 1)
	{
		//Don't bother creating more threads.
		Cluster_data_process(0, vclInput_data.size());
	}
	else
	{
		// local variables
		unsigned uPerThread;
		unsigned uData;
		unsigned uDataStart;
		int iThread_index;
		vector<thread> vtThreads;

		uDataStart = 0;
		uData = vclInput_data.size();
		uPerThread = uData / iNumThreads;

		//Split the dataset into parts and launch as individual
		//threads.
		for (iThread_index = 0; iThread_index < iNumThreads; iThread_index++)
		{
			//Force the last thread to take remaining data
			if (iThread_index == iNumThreads - 1)
			{
				uPerThread = uData - uDataStart;
			}

			vtThreads.push_back(thread([=](unsigned uStart, unsigned uLength) { Cluster_data_process(uStart, uLength); }, uDataStart, uPerThread));

			uDataStart += uPerThread;
		} // Launch all threads

		//Wait for all threads to complete.
		for (iThread_index = 0; iThread_index < iNumThreads; iThread_index++)
		{
			vtThreads[iThread_index].join();
		} // Wait for all threads
	}
	return;
} // Cluster_set::Cluster_data

//***********************************************************************
void Cluster_set::Calculate_cluster_means(void){

	// local variables
	int iK_index, iAttribute_index;
	unsigned uInstance_index, uInstance_sz;
	vector<int> viCounts(iK_count);

	uInstance_sz = vclInput_data.size();

	for (uInstance_index = 0; uInstance_index < uInstance_sz; uInstance_index++)
	{
		iK_index = vclInput_data[uInstance_index].iCluster;
		//loop through each vector attribute
		for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
			vvfMeans[iK_index][iAttribute_index] += vclInput_data[uInstance_index].vfAttribute[iAttribute_index];
		} // for

		viCounts[iK_index]++;
	}
	
	// loop thru each cluster
	for (iK_index = 0; iK_index < iK_count; iK_index++){
		//loop through each vector attribute
		for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++) {
			//If no elements, default to origin as per old code.
			if (viCounts[iK_index] == 0)
				vvfMeans[iK_index][iAttribute_index] = 0;
			else
				vvfMeans[iK_index][iAttribute_index] /= (float)viCounts[iK_index];
		}
	} // for

	return;
} // Cluster_set::Calculate_cluster_means

//***********************************************************************
bool Cluster_set::Compare_mean_values(void){

	// local variables
	bool bNot_done;
	float sStep_difference;
	float fResult_difference = 0;
	int iCluster_index, iAttribute_index;

	// calculate the difference between the old means and the new means
	for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){ // by each cluster
		// compare the attribute mean values
		for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){

			sStep_difference = vvfMeans[iCluster_index][iAttribute_index]
				- vvfOld_means[iCluster_index][iAttribute_index];
			// guarantee a positive value without using squares
			if (sStep_difference < 0) sStep_difference = sStep_difference * (-1);

			// add the step difference to the total difference of means
			fResult_difference = fResult_difference + sStep_difference;

		} // for

	} // for

	// stop clustering if there is little change in  the mean values
	if (fResult_difference < fTolerance) bNot_done = false;
	else bNot_done = true;

	return bNot_done;

} // Cluster_set::Compare_mean_values

//***********************************************************************

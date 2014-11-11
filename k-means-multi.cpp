//***********************************************************************
// k-means-multi.cpp
//
//   this is an implementation of the K-means classification algorithm.
//   it accepts vectors of multiple attributes (with classifications) and clusters them
//	 into K clusters
//
// INVOKE APPLICATION USING: k-means-multi <control file name>
//
// INPUTS: (from disk file)
//        <control.txt> - control file
//			   the control file can have any name - specified at application command
//
//			   Format: <control label> <value> these can be in any order
//
//			   control labels: #k-count, #input-filename, #output-filename, #use-labels,
//				 #tolerance, #EOF
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
// modified on: 20 oct 14
//
// © 2004 John Aleshunas
//
//***********************************************************************

#include "k-means-multi.h"

//***********************************************************************
// class Cluster_instance method declarations
//***********************************************************************
// class Classification_instance constructor
Cluster_instance::Cluster_instance(void){

	// make room for the attributes
	//vvfAttributes.resize(iAttribute_ct);

	return;
} //Cluster_instance::Cluster_instance

//***********************************************************************
// class Cluster method declarations
//***********************************************************************
// class Classification_instance constructor
Cluster::Cluster(void){

	// no code yet

	return;
} //Cluster::Cluster

//***********************************************************************
// class Cluster_set public method declarations
//***********************************************************************
// class Cluster_set constructor
Cluster_set::Cluster_set(void){

	// local variables

	// initialize the class variables
	iIteration = 0;
	iK_count = 1;
	sIn_file = "default_in.dat";
	sOut_file = "default_out.txt";
	bUseLabels = false;
	fTolerance = 0.1;

	return;
} //Cluster_set::Cluster_set

//***********************************************************************
void Cluster_set::Read_control_data(string sControlFilename) {

	// local variables
	string sTitle;
	bool bNot_done;

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

			if(sTitle == "#k-count"){ // read the count of neighbors
			strInput_stream >> iK_count;
			} // if

			if(sTitle == "#input-filename"){ // read the input filename
			strInput_stream >> sIn_file;
			} // if

			if(sTitle == "#output-filename"){ // read the output filename
			strInput_stream >> sOut_file;
			} // if

			if(sTitle == "#use-labels"){ // read the use-labels flag
			strInput_stream >> bUseLabels;
			} // if

			if(sTitle == "#tolerance"){ // // read the stopping criteria
			strInput_stream >> fTolerance;
			} // if

			if(sTitle == "#EOF"){ // // read the end of file label
			bNot_done = false;
			} // if
		} // while
	} //if

	else cout << "Error reading the file " << sControlFilename << endl << endl; // print error message

	strInput_stream.close();  // close filestream

	return;
} // Cluster_set::Read_control_file

//***********************************************************************
void Cluster_set::Execute_clustering(void){

	// local variables
	bool bNot_done = true;

	// read the input data
	Read_input_data();

	// loop until we are done clustering - the mean values don't change
	while (bNot_done){

		// set up the cluster set
		Setup_cluster_set(); //

		// identify the k means values
		Identify_mean_values(); //

		// cluster the input data using the k means values
		Cluster_data(); //

		// calculate the means of the clusters
		Calculate_cluster_means(); //

		// compare the old mean values to the new mean values
		// if the difference is less than the tolerance value then stop clustering
		bNot_done = Compare_mean_values(); //

		// increment the iteration
		iIteration++;

		// end the main loop
	} // while

	// write the output data
	Write_output_data(); //  <--- THIS IS THE NEXT LOCATION TO WORK ON

	return;
} // Cluster_set::Execute_clustering

//***********************************************************************
// class Cluster_set private method declarations
//***********************************************************************

//***********************************************************************
void Cluster_set::Read_input_data(void){

	// local variables
	int iAttribute_index, iCluster_index;
	float fInput_attribute;

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

			// save the data in the vector set
			vclInput_data.vclThe_cluster.push_back(clInput_instance);

		}// while

	} //if

	else cout << "Error reading " << sIn_file << endl << endl; // print error message

	strInput_stream.close();  // close filestream

	// allocate memory for the mean storage
	vvfMeans.resize(iK_count);
	vvfOld_means.resize(iK_count);

	for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){
		// get the attribute
		vvfMeans[iCluster_index].resize(iAttribute_ct);
		vvfOld_means[iCluster_index].resize(iAttribute_ct);
	} // for

	return;
} //Cluster_set::Read_input_data

//***********************************************************************
void Cluster_set::Write_output_data(void){

	// local variables
	unsigned uInstance_index;
	int iCluster_index, iAttribute_index;

	// declare an output stream
	ofstream strResults_out_stream;

	// open the stream to write the output plaintext
	strResults_out_stream.open(sOut_file.c_str());

	if(vclThe_cluster_set.size() < 1) { // we have an empty cluster_set
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
				<< vclThe_cluster_set[iCluster_index].vclThe_cluster.size() << endl;

			// loop thru the cluster members
			for (uInstance_index = 0;
				uInstance_index < vclThe_cluster_set[iCluster_index].vclThe_cluster.size(); uInstance_index++){

				// output the cluster member data
				for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){
					strResults_out_stream
						<< vclThe_cluster_set[iCluster_index].vclThe_cluster[uInstance_index].vfAttribute[iAttribute_index];
					strResults_out_stream << " ";
				} // for
				strResults_out_stream << " ";
				strResults_out_stream << vclThe_cluster_set[iCluster_index].vclThe_cluster[uInstance_index].sClassification;

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
void Cluster_set::Setup_cluster_set(void) {

	// local variables

	// clear the clusterset memory
	if (iIteration > 0) vclThe_cluster_set.clear();

	// allocate the clusterset memory
	vclThe_cluster_set.resize(iK_count);

	return;
} // Cluster_set::Setup_cluster_set

//***********************************************************************
void Cluster_set::Identify_mean_values(void){

	// local variables
	int iCluster_index, iAttribute_index;
	vector<float> vfValues;
	float fCheck_value = 99999;

	// allocate memory for the local vector
	vfValues.resize(iAttribute_ct);

	if (iIteration < 1) { // if this is the first iteration - initialize the cluster mean values
		for (iCluster_index = 0; iCluster_index < iK_count; iCluster_index++){ // read K-instances
			for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){ // read attributes
				vvfMeans[iCluster_index][iAttribute_index]
					= vclInput_data.vclThe_cluster[iCluster_index].vfAttribute[iAttribute_index];
				fCheck_value = vclInput_data.vclThe_cluster[iCluster_index].vfAttribute[iAttribute_index];
			} // for
		} //for
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
void Cluster_set::Cluster_data(void){

	// local variables
	float fDifference;
	float fSquared_difference, fSum_of_squares;
	float fBest_squared_difference;
	int iBest_index;
	unsigned uIndex;
	int iK_index, iAttribute_index;
	Cluster_instance clData_instance;

	// loop for all the input data values
	for (uIndex = 0; uIndex < vclInput_data.vclThe_cluster.size(); uIndex++) {

		// initialize the tracking variables
		fBest_squared_difference = 999999.0; // make this erroneously big
		iBest_index = 999999; // make this erroneously big

		// get the next data vector
		clData_instance = vclInput_data.vclThe_cluster[uIndex];

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
			if (fSum_of_squares < fBest_squared_difference){
				fBest_squared_difference = fSum_of_squares; // save it as BSD

				// and save the this index as best index
				iBest_index = iK_index;

			} // if
		} // for

		// insert the data_instance into the iBest_index cluster of the cluster_set
		vclThe_cluster_set[iBest_index].vclThe_cluster.push_back(clData_instance);

	} // for

	return;
} // Cluster_set::Cluster_data

//***********************************************************************
void Cluster_set::Calculate_cluster_means(void){

	// local variables
	float fSum_of_values;
	int iK_index, iAttribute_index;
	unsigned uInstance_index;
	float fCluster_means;

	// loop thru each cluster
	for (iK_index = 0; iK_index < iK_count; iK_index++){

		//loop through each vector attribute
		for (iAttribute_index = 0; iAttribute_index < iAttribute_ct; iAttribute_index++){

			// initialize the sum_of_values
			fSum_of_values = 0;

			// loop thru each value in the cluster
			for (uInstance_index = 0;
				uInstance_index < vclThe_cluster_set[iK_index].vclThe_cluster.size(); uInstance_index++){

				// add to the sum
				fSum_of_values = fSum_of_values +
					vclThe_cluster_set[iK_index].vclThe_cluster[uInstance_index].vfAttribute[iAttribute_index];

			} // for

			// calulate the mean of the sum
			fCluster_means = fSum_of_values / (float)uInstance_index;

			// save the mean value
			vvfMeans[iK_index][iAttribute_index] = fCluster_means;
		} // for

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


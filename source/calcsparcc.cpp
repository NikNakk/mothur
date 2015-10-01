//
//  runSparcc.cpp
//  PDSSparCC
//
//  Created by Patrick Schloss on 10/31/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include "calcsparcc.h"
#include "linearalgebra.h"

/**************************************************************************************************/

CalcSparcc::CalcSparcc(vector<vector<float> > sharedVector, int maxIterations, int numSamplings, string method)
{
	try {
		numOTUs = (int)sharedVector[0].size();
		numGroups = (int)sharedVector.size();
		normalizationMethod = method;
		int numOTUs = (int)sharedVector[0].size();

		addPseudoCount(sharedVector);

		vector<vector<vector<float> > > allCorrelations(numSamplings);

		//    float cycClockStart = clock();
		//    unsigned long long cycTimeStart = time(NULL);

		for (int i = 0;i < numSamplings;i++) {
			if (ctrlc_pressed) { break; }
			vector<float> logFractions = getLogFractions(sharedVector, method);
			getT_Matrix(logFractions);     //this step is slow...
			getT_Vector();
			getD_Matrix();
			vector<float> basisVariances = getBasisVariances();     //this step is slow...
			vector<vector<float> > correlation = getBasisCorrelations(basisVariances);

			excluded.resize(numOTUs);
			for (int j = 0;j < numOTUs;j++) { excluded[j].assign(numOTUs, 0); }

			float maxRho = 1;
			int excludeRow = -1;
			int excludeColumn = -1;

			int iter = 0;
			while (maxRho > 0.10 && iter < maxIterations) {
				maxRho = getExcludedPairs(correlation, excludeRow, excludeColumn);
				excludeValues(excludeRow, excludeColumn);
				vector<float> excludedBasisVariances = getBasisVariances();
				correlation = getBasisCorrelations(excludedBasisVariances);
				iter++;
			}
			allCorrelations[i] = correlation;
		}

		if (!ctrlc_pressed) {
			if (numSamplings > 1) {
				getMedian(allCorrelations);
			}
			else {
				median = allCorrelations[0];
			}
		}
		//    cout << median[0][3] << '\t' << median[0][6] << endl;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CalcSparcc, CalcSparcc";
		exit(1);
	}
}

/**************************************************************************************************/

void CalcSparcc::addPseudoCount(vector<vector<float> >& sharedVector) {
	for (int i = 0;i < numGroups;i++) {   //iterate across the groups
		if (ctrlc_pressed) { return; }
		for (int j = 0;j < numOTUs;j++) {
			sharedVector[i][j] += 1;
		}
	}

}

/**************************************************************************************************/

vector<float> CalcSparcc::getLogFractions(vector<vector<float> > sharedVector, string method) {   //dirichlet by default
	try {
		vector<float> logSharedFractions(numGroups * numOTUs, 0);

		if (method == "dirichlet") {
			vector<float> alphas(numGroups);
			for (int i = 0;i < numGroups;i++) {   //iterate across the groups
				if (ctrlc_pressed) { return logSharedFractions; }
				alphas = RNG.randomDirichlet(sharedVector[i]);

				for (int j = 0;j < numOTUs;j++) {
					logSharedFractions[i * numOTUs + j] = alphas[j];
				}
			}
		}
		else if (method == "relabund") {
			for (int i = 0;i < numGroups;i++) {
				if (ctrlc_pressed) { return logSharedFractions; }
				float total = 0.0;
				for (int j = 0;j < numOTUs;j++) {
					total += sharedVector[i][j];
				}
				for (int j = 0;j < numOTUs;j++) {
					logSharedFractions[i * numOTUs + j] = sharedVector[i][j] / total;
				}
			}
		}

		for (int i = 0;i < logSharedFractions.size();i++) {
			logSharedFractions[i] = log(logSharedFractions[i]);
		}

		return logSharedFractions;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CalcSparcc, addPseudoCount";
		exit(1);
	}

}

/**************************************************************************************************/

void CalcSparcc::getT_Matrix(vector<float> sharedFractions) {
	tMatrix.resize(numOTUs * numOTUs, 0);

	vector<float> diff(numGroups);

	for (int j1 = 0;j1 < numOTUs;j1++) {
		for (int j2 = 0;j2 < j1;j2++) {
			if (ctrlc_pressed) { return; }
			float mean = 0.0;
			for (int i = 0;i < numGroups;i++) {
				diff[i] = sharedFractions[i * numOTUs + j1] - sharedFractions[i * numOTUs + j2];
				mean += diff[i];
			}
			mean /= float(numGroups);

			float variance = 0.0;
			for (int i = 0;i < numGroups;i++) {
				variance += (diff[i] - mean) * (diff[i] - mean);
			}
			variance /= (float)(numGroups - 1);

			tMatrix[j1 * numOTUs + j2] = variance;
			tMatrix[j2 * numOTUs + j1] = tMatrix[j1 * numOTUs + j2];
		}
	}

}

/**************************************************************************************************/

void CalcSparcc::getT_Vector() {
	tVector.assign(numOTUs, 0);

	for (int j1 = 0;j1 < numOTUs;j1++) {
		if (ctrlc_pressed) { return; }
		for (int j2 = 0;j2 < numOTUs;j2++) {
			tVector[j1] += tMatrix[j1 * numOTUs + j2];
		}
	}
}

/**************************************************************************************************/

void CalcSparcc::getD_Matrix() {
	float d = numOTUs - 1.0f;

	dMatrix.resize(numOTUs);
	for (int i = 0;i < numOTUs;i++) {
		if (ctrlc_pressed) { return; }
		dMatrix[i].resize(numOTUs, 1);
		dMatrix[i][i] = d;
	}
}

/**************************************************************************************************/

vector<float> CalcSparcc::getBasisVariances() {
	LinearAlgebra LA;

	vector<float> variances = LA.solveEquations(dMatrix, tVector);

	for (int i = 0;i < variances.size();i++) {
		if (ctrlc_pressed) { return variances; }
		if (variances[i] < 0) { variances[i] = 1e-4f; }
	}

	return variances;
}

/**************************************************************************************************/

vector<vector<float> > CalcSparcc::getBasisCorrelations(vector<float> basisVariance) {
	vector<vector<float> > rho(numOTUs);
	for (int i = 0;i < numOTUs;i++) { rho[i].resize(numOTUs, 0); }

	for (int i = 0;i < numOTUs;i++) {
		float var_i = basisVariance[i];
		float sqrt_var_i = sqrt(var_i);

		rho[i][i] = 1.00;

		for (int j = 0;j<i;j++) {
			if (ctrlc_pressed) { return rho; }
			float var_j = basisVariance[j];

			rho[i][j] = (var_i + var_j - tMatrix[i * numOTUs + j]) / (2.0f * sqrt_var_i * sqrt(var_j));
			if (rho[i][j] > 1.0) { rho[i][j] = 1.0f; }
			else if (rho[i][j] < -1.0) { rho[i][j] = -1.0f; }

			rho[j][i] = rho[i][j];

		}
	}

	return rho;
}

/**************************************************************************************************/

float CalcSparcc::getExcludedPairs(vector<vector<float> > rho, int& maxRow, int& maxColumn) {
	float maxRho = 0;
	maxRow = -1;
	maxColumn = -1;

	for (int i = 0;i < numOTUs;i++) {

		for (int j = 0;j < i;j++) {
			if (ctrlc_pressed) { return maxRho; }
			float tester = abs(rho[i][j]);

			if (tester > maxRho && excluded[i][j] != 1) {
				maxRho = tester;
				maxRow = i;
				maxColumn = j;
			}
		}

	}

	return maxRho;
}

/**************************************************************************************************/

void CalcSparcc::excludeValues(int excludeRow, int excludeColumn) {
	try {
		tVector[excludeRow] -= tMatrix[excludeRow * numOTUs + excludeColumn];
		tVector[excludeColumn] -= tMatrix[excludeRow * numOTUs + excludeColumn];

		dMatrix[excludeRow][excludeColumn] = 0;
		dMatrix[excludeColumn][excludeRow] = 0;
		dMatrix[excludeRow][excludeRow]--;
		dMatrix[excludeColumn][excludeColumn]--;

		excluded[excludeRow][excludeColumn] = 1;
		excluded[excludeColumn][excludeRow] = 1;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CalcSparcc, excludeValues";
		exit(1);
	}
}

/**************************************************************************************************/

void CalcSparcc::getMedian(vector<vector<vector<float> > > allCorrelations) {
	int numSamples = (int)allCorrelations.size();
	median.resize(numOTUs);
	for (int i = 0;i < numOTUs;i++) { median[i].assign(numOTUs, 1); }

	vector<float> hold(numSamples);

	for (int i = 0;i < numOTUs;i++) {
		for (int j = 0;j < i;j++) {
			if (ctrlc_pressed) { return; }

			for (int k = 0;k < numSamples;k++) {
				hold[k] = allCorrelations[k][i][j];
			}

			sort(hold.begin(), hold.end());
			median[i][j] = hold[int(numSamples * 0.5)];
			median[j][i] = median[i][j];
		}
	}
}

/**************************************************************************************************/

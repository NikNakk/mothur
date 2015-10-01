//
//  svm.cpp
//  support vector machine
//
//  Created by Joshua Lynch on 6/19/2013.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <stack>
#include <utility>
#include <iterator>

#include "svm.hpp"

// OutputFilter constants
const int OutputFilter::QUIET = 0;
const int OutputFilter::INFO = 1;
const int OutputFilter::mDEBUG = 2;
const int OutputFilter::TRACE = 3;


#define RANGE(X) X, X + sizeof(X)/sizeof(double)

// parameters will be tested in the order they are specified

const string LinearKernelFunction::MapKey = "linear";//"LinearKernel";
const string LinearKernelFunction::MapKey_Constant = "constant";//"LinearKernel_Constant";
const double defaultLinearConstantRangeArray[] = { 0.0, -1.0, 1.0, -10.0, 10.0 };
const ParameterRange LinearKernelFunction::defaultConstantRange = ParameterRange(RANGE(defaultLinearConstantRangeArray));

const string RbfKernelFunction::MapKey = "rbf";//"RbfKernel";
const string RbfKernelFunction::MapKey_Gamma = "gamma";//"RbfKernel_Gamma";
const double defaultRbfGammaRangeArray[] = { 0.0001, 0.001, 0.01, 0.1, 1.0, 10.0, 100.0 };
const ParameterRange RbfKernelFunction::defaultGammaRange = ParameterRange(RANGE(defaultRbfGammaRangeArray));

const string PolynomialKernelFunction::MapKey = "polynomial";//"PolynomialKernel";
const string PolynomialKernelFunction::MapKey_Constant = "constant";//"PolynomialKernel_Constant";
const string PolynomialKernelFunction::MapKey_Coefficient = "coefficient";//"PolynomialKernel_Coefficient";
const string PolynomialKernelFunction::MapKey_Degree = "degree";//"PolynomialKernel_Degree";

const double defaultPolynomialConstantRangeArray[] = { 0.0, -1.0, 1.0, -2.0, 2.0, -3.0, 3.0 };
const ParameterRange PolynomialKernelFunction::defaultConstantRange = ParameterRange(RANGE(defaultPolynomialConstantRangeArray));
const double defaultPolynomialCoefficientRangeArray[] = { 0.01, 0.1, 1.0, 10.0, 100.0 };
const ParameterRange PolynomialKernelFunction::defaultCoefficientRange = ParameterRange(RANGE(defaultPolynomialCoefficientRangeArray));
const double defaultPolynomialDegreeRangeArray[] = { 2.0, 3.0, 4.0 };
const ParameterRange PolynomialKernelFunction::defaultDegreeRange = ParameterRange(RANGE(defaultPolynomialDegreeRangeArray));

const string SigmoidKernelFunction::MapKey = "sigmoid";
const string SigmoidKernelFunction::MapKey_Alpha = "alpha";
const string SigmoidKernelFunction::MapKey_Constant = "constant";

const double defaultSigmoidAlphaRangeArray[] = { 1.0, 2.0 };
const ParameterRange SigmoidKernelFunction::defaultAlphaRange = ParameterRange(RANGE(defaultSigmoidAlphaRangeArray));
const double defaultSigmoidConstantRangeArray[] = { 1.0, 2.0 };
const ParameterRange SigmoidKernelFunction::defaultConstantRange = ParameterRange(RANGE(defaultSigmoidConstantRangeArray));

const string SmoTrainer::MapKey_C = "smoc";//"SmoTrainer_C";
const double defaultSmoTrainerCRangeArray[] = { 0.0001, 0.001, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0 };
const ParameterRange SmoTrainer::defaultCRange = ParameterRange(RANGE(defaultSmoTrainerCRangeArray));

MothurOut* m = MothurOut::getInstance();

LabelPair buildLabelPair(const Label& one, const Label& two) {
	LabelVector labelPair(2);
	labelPair[0] = one;
	labelPair[1] = two;
	return labelPair;
}

// Dividing a dataset into training and testing sets while maintaining equal
// representation of all classes is done using a LabelToLabeledObservationVector.
// This container is used to divide datasets into groups of LabeledObservations
// having the same label.  For example, given a LabeledObservationVector like
//     ["blue",  [1.0, 2.0, 3.0]]
//     ["green", [3.0, 4.0, 5.0]]
//     ["blue",  [2,0, 3.0. 4.0]]
//     ["green", [4.0, 5.0, 6.0]]
// the corresponding LabelToLabeledObservationVector looks like
//     "blue"  : [["blue",  [1.0, 2.0, 3.0]], ["blue",  [2,0, 3.0. 4.0]]]
//     "green" : [["green", [3.0, 4.0, 5.0]], ["green", [4.0, 5.0, 6.0]]]
void buildLabelToLabeledObservationVector(LabelToLabeledObservationVector& labelToLabeledObservationVector, const LabeledObservationVector& labeledObservationVector) {
	for (LabeledObservationVector::const_iterator j = labeledObservationVector.begin(); j != labeledObservationVector.end(); j++) {
		labelToLabeledObservationVector[j->first].push_back(*j);
	}
}


class MeanAndStd {
private:
	double n;
	double M2;
	double mean;

public:
	MeanAndStd() {}
	~MeanAndStd() {}

	void initialize() {
		n = 0.0;
		mean = 0.0;
		M2 = 0.0;
	}

	void processNextValue(double x) {
		n += 1.0;
		double delta = x - mean;
		mean += delta / n;
		M2 += delta * (x - mean);
	}

	double getMean() {
		return mean;
	}

	double getStd() {
		double variance = M2 / (n - 1.0);
		return sqrt(variance);
	}
};


// The LabelMatchesEither functor is used only in a call to remove_copy_if in the
// OneVsOneMultiClassSvmTrainer::train method.  It returns true if the labeled
// observation argument has the same label as either of the two label arguments.
class FeatureLabelMatches {
public:
	FeatureLabelMatches(const string& _featureLabel) : featureLabel(_featureLabel) {}

	bool operator() (const Feature& f) {
		return f.getFeatureLabel() == featureLabel;
	}

private:
	const string& featureLabel;

};

Feature removeFeature(Feature featureToRemove, LabeledObservationVector& observations, FeatureVector& featureVector) {
	FeatureLabelMatches matchFeatureLabel(featureToRemove.getFeatureLabel());
	featureVector.erase(
		remove_if(featureVector.begin(), featureVector.end(), matchFeatureLabel),
		featureVector.end()
		);
	for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
		observations[observation].removeFeatureAtIndex(featureToRemove.getFeatureIndex());
	}
	// update the feature indices
	for (int i = 0; i < featureVector.size(); i++) {
		featureVector.at(i).setFeatureIndex(i);
	}
	featureToRemove.setFeatureIndex(-1);
	return featureToRemove;
}

FeatureVector applyStdThreshold(double stdThreshold, LabeledObservationVector& observations, FeatureVector& featureVector) {
	// calculate standard deviation of each feature
	// remove features with standard deviation less than or equal to stdThreshold
	MeanAndStd ms;
	// loop over features in reverse order so we can get the index of each
	// for example,
	//     if there are 5 features a,b,c,d,e
	//     and features a, c, e fall below the stdThreshold
	//     loop iteration 0: remove feature e (index 4) -- features are now a,b,c,d
	//     loop iteration 1: leave feature d (index 3)
	//     loop iteration 2: remove feature c (index 2) -- features are now a,b,d
	//     loop iteration 3: leave feature b (index 1)
	//     loop iteration 4: remove feature a (index 0) -- features are now b,d
	FeatureVector removedFeatureVector;
	for (int feature = observations[0].second->size() - 1; feature >= 0; feature--) {
		ms.initialize();
		LOG(INFO) << "feature index " + toString(feature) << '\n';
		for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
			ms.processNextValue(observations[observation].second->at(feature));
		}
		LOG(INFO) << "feature " + toString(feature) + " has std " + toString(ms.getStd()) << '\n';
		if (ms.getStd() <= stdThreshold) {
			LOG(INFO) << "removing feature with index " + toString(feature) << '\n';
			// remove this feature

			Feature featureToRemove = featureVector.at(feature);
			removedFeatureVector.push_back(
				removeFeature(featureToRemove, observations, featureVector)
				);
		}
	}
	reverse(removedFeatureVector.begin(), removedFeatureVector.end());
	return removedFeatureVector;
}


// this function standardizes data to mean 0 and variance 1
// but this may not be a good standardization for OTU data
void transformZeroMeanUnitVariance(LabeledObservationVector& observations) {
	bool vebose = false;
	// online method for mean and variance
	MeanAndStd ms;
	for (Observation::size_type feature = 0; feature < observations[0].second->size(); feature++) {
		ms.initialize();
		//double n = 0.0;
		//double mean = 0.0;
		//double M2 = 0.0;
		for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
			ms.processNextValue(observations[observation].second->at(feature));
			//n += 1.0;
			//double x = observations[observation].second->at(feature);
			//double delta = x - mean;
			//mean += delta / n;
			//M2 += delta * (x - mean);
		}
		//double variance = M2 / (n - 1.0);
		//double standardDeviation = sqrt(variance);
		if (vebose) {
			LOG(INFO) << "mean of feature " + toString(feature) + " is " + toString(ms.getMean()) << '\n';
			LOG(INFO) << "std of feature " + toString(feature) + " is " + toString(ms.getStd()) << '\n';
		}
		// normalize the feature
		double mean = ms.getMean();
		double std = ms.getStd();
		for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
			observations[observation].second->at(feature) = (observations[observation].second->at(feature) - mean) / std;
		}
	}
}


double getMinimumFeatureValueForObservation(Observation::size_type featureIndex, LabeledObservationVector& observations) {
	double featureMinimum = numeric_limits<double>::max();
	for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
		if (observations[observation].second->at(featureIndex) < featureMinimum) {
			featureMinimum = observations[observation].second->at(featureIndex);
		}
	}
	return featureMinimum;
}


double getMaximumFeatureValueForObservation(Observation::size_type featureIndex, LabeledObservationVector& observations) {
	double featureMaximum = numeric_limits<double>::min();
	for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
		if (observations[observation].second->at(featureIndex) > featureMaximum) {
			featureMaximum = observations[observation].second->at(featureIndex);
		}
	}
	return featureMaximum;
}


// this function standardizes data to minimum value 0.0 and maximum value 1.0
void transformZeroOne(LabeledObservationVector& observations) {
	for (Observation::size_type feature = 0; feature < observations[0].second->size(); feature++) {
		double featureMinimum = getMinimumFeatureValueForObservation(feature, observations);
		double featureMaximum = getMaximumFeatureValueForObservation(feature, observations);
		// standardize the feature
		for (ObservationVector::size_type observation = 0; observation < observations.size(); observation++) {
			double x = observations[observation].second->at(feature);
			double xstd = (x - featureMinimum) / (featureMaximum - featureMinimum);
			observations[observation].second->at(feature) = xstd / (1.0 - 0.0) + 0.0;
		}
	}
}


//
// SVM member functions
//
// the discriminant member function returns +1 or -1
int SVM::discriminant(const Observation& observation) const {
	// d is the discriminant function
	double d = b;
	for (int i = 0; i < y.size(); i++) {
		d += y[i] * a[i] * inner_product(observation.begin(), observation.end(), x[i].second->begin(), 0.0);
	}
	return d > 0.0 ? 1 : -1;
}

LabelVector SVM::classify(const LabeledObservationVector& twoClassLabeledObservationVector) const {
	LabelVector predictionVector;
	for (LabeledObservationVector::const_iterator i = twoClassLabeledObservationVector.begin(); i != twoClassLabeledObservationVector.end(); i++) {
		Label prediction = classify(*(i->getObservation()));
		Label actual = i->getLabel();
		//cout << "classification of actual " << actual << " is " << prediction << endl;
		predictionVector.push_back(prediction);
	}
	return predictionVector;
}

// the score member function classifies each labeled observation from the
// argument and returns the fraction of correct classifications
// don't need this any more????
double SVM::score(const LabeledObservationVector& twoClassLabeledObservationVector) const {
	//cout << "score:" << endl;
	double s = 0.0;
	for (LabeledObservationVector::const_iterator i = twoClassLabeledObservationVector.begin(); i != twoClassLabeledObservationVector.end(); i++) {
		Label predicted_label = classify(*(i->second));
		//cout << "in score actual label: '" << i->first << "' predicted label: '" << predicted_label << "'" << endl;
		if (predicted_label == i->first) {
			s = s + 1.0;
		}
		else {

		}
	}
	return s / double(twoClassLabeledObservationVector.size());
}

void SvmPerformanceSummary::init(const SVM& svm, const LabeledObservationVector& actualLabels, const LabelVector& predictedLabels) {
	// accumulate four counts:
	//     tp (true positive)  -- correct classifications (classified +1 as +1)
	//     fp (false positive) -- incorrect classifications (classified -1 as +1)
	//     fn (false negative) -- incorrect classifications (classified +1 as -1)
	//     tn (true negative)  -- correct classification (classified -1 as -1)
	// the label corresponding to discriminant +1 will be the 'positive' class
	NumericClassToLabel discriminantToLabel = svm.getDiscriminantToLabel();
	positiveClassLabel = discriminantToLabel[1];
	negativeClassLabel = discriminantToLabel[-1];
	//cout << "positive class label: " << positiveClassLabel << endl;
	//cout << "negative class label: " << negativeClassLabel << endl;
	//cout << "actual labels vector has length " << actualLabels.size() << endl;
	//cout << "predicted labels vector has length " << predictedLabels.size() << endl;
	double tp = 0;
	double fp = 0;
	double fn = 0;
	double tn = 0;
	double unknown = 0;
	for (int i = 0; i < actualLabels.size(); i++) {
		Label predictedLabel = predictedLabels.at(i);
		Label actualLabel = actualLabels.at(i).getLabel();
		//cout << "predicted: " << predictedLabel << " actual: " << actualLabel << endl;
		if (actualLabel.compare(positiveClassLabel) == 0) {
			if (predictedLabel.compare(positiveClassLabel) == 0) {
				tp++;
			}
			else if (predictedLabel.compare(negativeClassLabel) == 0) {
				fn++;
			}
			else {
				LOG(INFO) << "actual label is positive but something is wrong" << '\n';
			}
		}
		else if (actualLabel.compare(negativeClassLabel) == 0) {
			if (predictedLabel.compare(positiveClassLabel) == 0) {
				fp++;
			}
			else if (predictedLabel.compare(negativeClassLabel) == 0) {
				tn++;
			}
			else {
				LOG(INFO) << "actual label is negative but something is wrong" << '\n';
			}
		}
		else {
			// in the event we have been given an observation that is labeled
			// neither positive nor negative then we will get a false classification
			//cout << "unrecognized actual label " << actualLabel << endl;
			if (predictedLabel.compare(positiveClassLabel)) {
				fp++;
			}
			else {
				fn++;
			}
		}
	}
	if (tp == 0 && fp == 0) {
		precision = 0;
	}
	else {
		precision = tp / (tp + fp);
	}
	recall = tp / (tp + fn);
	if (precision == 0 && recall == 0) {
		f = 0;
	}
	else {
		f = 2.0 * (precision * recall) / (precision + recall);
	}
	accuracy = (tp + tn) / (tp + tn + fp + fn);
	//cout << "svm performance summary for labels " << positiveClassLabel << " " << negativeClassLabel << endl;
	//cout << "tp: " << tp << " fp: " << fp << " tn: " << tn << " fn: " << fn << endl;
	//cout << "precision: " << precision << " recall: " << recall << " f: " << f << " accuracy: " << accuracy << endl;
}


MultiClassSVM::MultiClassSVM(const vector<SVM*> s, const LabelSet& l, const SvmToSvmPerformanceSummary& p, OutputFilter of) : twoClassSvmList(s.begin(), s.end()), labelSet(l), svmToSvmPerformanceSummary(p), outputFilter(of), accuracy(0) {}


MultiClassSVM::~MultiClassSVM() {
	for (int i = 0; i < twoClassSvmList.size(); i++) {
		delete twoClassSvmList[i];
	}
}

// The fewerVotes function is used to find the maximum vote
// tally in MultiClassSVM::classify.  This function returns true
// if the first element (number of votes for the first label) is
// less than the second element (number of votes for the second label).
bool fewerVotes(const pair<Label, int>& p, const pair<Label, int>& q) {
	return p.second < q.second;
}


Label MultiClassSVM::classify(const Observation& observation) {
	map<Label, int> labelToVoteCount;
	for (int i = 0; i < twoClassSvmList.size(); i++) {
		Label predictedLabel = twoClassSvmList[i]->classify(observation);
		labelToVoteCount[predictedLabel]++;
	}
	pair<Label, int> winner = *max_element(labelToVoteCount.begin(), labelToVoteCount.end(), fewerVotes);
	LabelVector winningLabels;
	winningLabels.push_back(winner.first);
	for (map<Label, int>::const_iterator i = labelToVoteCount.begin(); i != labelToVoteCount.end(); i++) {
		if (i->second == winner.second && i->first != winner.first) {
			winningLabels.push_back(i->first);
		}
	}
	if (winningLabels.size() == 1) {
		// we have a winner
	}
	else {
		// we have a tie
		throw MultiClassSvmClassificationTie(winningLabels, winner.second);
	}

	return winner.first;
}

double MultiClassSVM::score(const LabeledObservationVector& multiClassLabeledObservationVector) {
	double s = 0.0;
	for (LabeledObservationVector::const_iterator i = multiClassLabeledObservationVector.begin(); i != multiClassLabeledObservationVector.end(); i++) {
		//cout << "classifying observation with label " << i->first << endl;
		try {
			Label predicted_label = classify(*(i->second));
			if (predicted_label == i->first) {
				s = s + 1.0;
			}
			else {
				// predicted label does not match actual label
			}
		}
		catch (MultiClassSvmClassificationTie& e) {
			if (outputFilter.debug()) {
				LOG(INFO) << "classification tie for observation " + toString(i->datasetIndex) + " with label " + toString(i->first) << '\n';
			}
		}
	}
	return s / double(multiClassLabeledObservationVector.size());
}

class MaxIterationsExceeded : public exception {
	virtual const char* what() const throw() {
		return "maximum iterations exceeded during SMO";
	}
} maxIterationsExceeded;


//SvmTrainingInterruptedException smoTrainingInterruptedException("SMO training interrupted by user");

//  The train method implements Sequential Minimal Optimization as described in
//  "Support Vector Machine Solvers" by Bottou and Lin.
//
//  SmoTrainer::train releases a pointer to an SVM into the wild so we must be
//  careful about handling the LabeledObservationVector....  Must create a copy
//  of those labeled vectors???
SVM* SmoTrainer::train(KernelFunctionCache& K, const LabeledObservationVector& twoClassLabeledObservationVector) {
	const int observationCount = twoClassLabeledObservationVector.size();
	const int featureCount = twoClassLabeledObservationVector[0].second->size();

	if (outputFilter.debug()) LOG(INFO) << "observation count : " + toString(observationCount) << '\n';
	if (outputFilter.debug()) LOG(INFO) << "feature count     : " + toString(featureCount) << '\n';
	// dual coefficients
	vector<double> a(observationCount, 0.0);
	// gradient
	vector<double> g(observationCount, 1.0);
	// convert the labels to -1.0,+1.0
	vector<double> y(observationCount);
	if (outputFilter.trace()) LOG(INFO) << "assign numeric labels" << '\n';
	NumericClassToLabel discriminantToLabel;
	assignNumericLabels(y, twoClassLabeledObservationVector, discriminantToLabel);
	if (outputFilter.trace()) LOG(INFO) << "assign A and B" << '\n';
	vector<double> A(observationCount);
	vector<double> B(observationCount);
	for (int n = 0; n < observationCount; n++) {
		if (y[n] == +1.0) {
			A[n] = 0.0;
			B[n] = C;
		}
		else {
			A[n] = -C;
			B[n] = 0;
		}
		if (outputFilter.trace()) LOG(INFO) << toString(n) + " " + toString(A[n]) + " " + toString(B[n]) << '\n';
	}
	if (outputFilter.trace()) LOG(INFO) << "assign K" << '\n';
	int m_count = 0;
	vector<double> u(3);
	vector<double> ya(observationCount);
	vector<double> yg(observationCount);
	double lambda = numeric_limits<double>::max();
	while (true) {

		if (ctrlc_pressed) { return 0; }
		//if ( externalSvmTrainingInterruption.interruptTraining() ) {
			// this should be a specialized exception
			//cout << "***************************** interrupting training **********************************" << endl;
			//throw smoTrainingInterruptedException;
		//}

		m_count++;
		int i = 0; // 0
		int j = 0; // 0
		double yg_max = numeric_limits<double>::min();
		double yg_min = numeric_limits<double>::max();
		if (outputFilter.trace()) LOG(INFO) << "m = " + toString(m_count) << '\n';
		for (int k = 0; k < observationCount; k++) {
			ya[k] = y[k] * a[k];
			yg[k] = y[k] * g[k];
		}
		if (outputFilter.trace()) {
			LOG(INFO) << "yg =";
			for (int k = 0; k < observationCount; k++) {
				//cout << A[k] << " " << B[k] << " " << y[k] << " " << a[k] << " " << g[k] << " " << ya[k] << " " << yg[k] << endl;
				LOG(INFO) << " " + toString(yg[k]);
			}
			LOG(INFO) << "";
		}

		for (int k = 0; k < observationCount; k++) {
			if (ya[k] < B[k] && yg[k] > yg_max) {
				yg_max = yg[k];
				i = k;
			}
			if (A[k] < ya[k] && yg[k] < yg_min) {
				yg_min = yg[k];
				j = k;
			}
			//cout << "n = " << n << endl;
			//cout << ya[k] << " " << yg[k] << endl;
			//cout << "j = " << j << " yg[j] = " << yg[j] << endl;
		}
		// maximum violating pair is i,j
		if (outputFilter.trace()) {
			LOG(INFO) << "maximal violating pair: " + toString(i) + " " + toString(j) << '\n';
			LOG(INFO) << "  i = " + toString(i) + " features: ";
			for (int feature = 0; feature < featureCount; feature++) {
				LOG(INFO) << toString(twoClassLabeledObservationVector[i].second->at(feature)) + " ";
			};
			LOG(INFO) << '\n' << "  j = " + toString(j) + " features: ";
			for (int feature = 0; feature < featureCount; feature++) {
				LOG(INFO) << toString(twoClassLabeledObservationVector[j].second->at(feature)) + " ";
			};
			LOG(INFO) << "";
		}

		// parameterize this
		if (m_count > 1000) { //1000
			// what happens if we just go with what we've got instead of throwing an exception?
			// things work pretty well for the most part
			// might be better to look at lambda???
			if (outputFilter.debug()) LOG(INFO) << "iteration limit reached with lambda = " + toString(lambda) << '\n';
			break;
		}

		// using lambda to break is a good performance enhancement
		if (yg[i] <= yg[j] || lambda < 0.0001) {
			break;
		}
		u[0] = B[i] - ya[i];
		u[1] = ya[j] - A[j];

		double K_ii = K.similarity(twoClassLabeledObservationVector[i], twoClassLabeledObservationVector[i]);
		double K_jj = K.similarity(twoClassLabeledObservationVector[j], twoClassLabeledObservationVector[j]);
		double K_ij = K.similarity(twoClassLabeledObservationVector[i], twoClassLabeledObservationVector[j]);
		u[2] = (yg[i] - yg[j]) / (K_ii + K_jj - 2.0*K_ij);
		if (outputFilter.trace()) LOG(INFO) << "directions: (" + toString(u[0]) + "," + toString(u[1]) + "," + toString(u[2]) + ")" << '\n';
		lambda = *min_element(u.begin(), u.end());
		if (outputFilter.trace()) LOG(INFO) << "lambda: " + toString(lambda) << '\n';
		for (int k = 0; k < observationCount; k++) {
			double K_ik = K.similarity(twoClassLabeledObservationVector[i], twoClassLabeledObservationVector[k]);
			double K_jk = K.similarity(twoClassLabeledObservationVector[j], twoClassLabeledObservationVector[k]);
			g[k] += (-lambda * y[k] * K_ik + lambda * y[k] * K_jk);
		}
		if (outputFilter.trace()) {
			LOG(INFO) << "g =";
			for (int k = 0; k < observationCount; k++) {
				LOG(INFO) << " " + toString(g[k]);
			}
			LOG(INFO) << "";
		}
		a[i] += y[i] * lambda;
		a[j] -= y[j] * lambda;
	}


	// at this point the optimal a's have been found
	// now use them to find w and b
	if (outputFilter.trace()) LOG(INFO) << "find w" << '\n';
	vector<double> w(twoClassLabeledObservationVector[0].second->size(), 0.0);
	double b = 0.0;
	for (int i = 0; i < y.size(); i++) {
		if (outputFilter.trace()) LOG(INFO) << "alpha[" + toString(i) + "] = " + toString(a[i]) << '\n';
		for (int j = 0; j < w.size(); j++) {
			w[j] += a[i] * y[i] * twoClassLabeledObservationVector[i].second->at(j);
		}
		if (A[i] < a[i] && a[i] < B[i]) {
			b = yg[i];
			if (outputFilter.trace()) LOG(INFO) << "b = " + toString(b) << '\n';
		}
	}

	if (outputFilter.trace()) {
		for (int i = 0; i < w.size(); i++) {
			LOG(INFO) << "w[" + toString(i) + "] = " + toString(w[i]) << '\n';
		}
	}

	// be careful about passing twoClassLabeledObservationVector - what if this vector
	// is deleted???
	//
	// we can eliminate elements of y, a and observation vectors corresponding to a = 0
	vector<double> support_y;
	vector<double> nonzero_a;
	LabeledObservationVector supportVectors;
	for (int i = 0; i < a.size(); i++) {
		if (a.at(i) == 0.0) {
			// this dual coefficient does not correspond to a support vector
		}
		else {
			support_y.push_back(y.at(i));
			nonzero_a.push_back(a.at(i));
			supportVectors.push_back(twoClassLabeledObservationVector.at(i));
		}
	}
	//return new SVM(y, a, twoClassLabeledObservationVector, b, discriminantToLabel);
	if (outputFilter.info()) LOG(INFO) << "found " + toString(supportVectors.size()) + " support vectors" << '\n';
	return new SVM(support_y, nonzero_a, supportVectors, b, discriminantToLabel);
}

typedef map<Label, double> LabelToNumericClassLabel;

// For SVM training we need to assign numeric class labels of -1.0 and +1.0.
// This method populates the y vector argument with -1.0 and +1.0
// corresponding to the two classes in the labelVector argument.
// For example, if labeledObservationVector looks like this:
//     [ (0, "blue",  [...some observations...]),
//       (1, "green", [...some observations...]),
//       (2, "blue",  [...some observations...]) ]
// Then after the function executes the y vector will look like this:
//     [-1.0,   blue
//      +1.0,   green
//      -1.0]   blue
// and discriminantToLabel will look like this:
//     { -1.0 : "blue",
//       +1.0 : "green" }
// The label "blue" is mapped to -1.0 because it is (lexicographically) less than "green".
// When given labels "blue" and "green" this function will always assign "blue" to -1.0 and
// "green" to +1.0.  This is not fundamentally important but it makes testing easier and is
// not a hassle to implement.
void SmoTrainer::assignNumericLabels(vector<double>& y, const LabeledObservationVector& labeledObservationVector, NumericClassToLabel& discriminantToLabel) {
	// it would be nice if we assign -1.0 and +1.0 consistently for each pair of labels
	// I think the label set will always be traversed in sorted order so we should get this for free

	// we are going to overwrite arguments y and discriminantToLabel
	y.clear();
	discriminantToLabel.clear();

	LabelSet labelSet;
	buildLabelSet(labelSet, labeledObservationVector);
	LabelVector uniqueLabels(labelSet.begin(), labelSet.end());
	if (labelSet.size() != 2) {
		// throw an exception
		cerr << "unexpected label set size " << labelSet.size() << endl;
		for (LabelSet::const_iterator i = labelSet.begin(); i != labelSet.end(); i++) {
			cerr << "    label " << *i << endl;
		}
		throw SmoTrainerException("SmoTrainer::assignNumericLabels was passed more than 2 labels");
	}
	else {
		LabelToNumericClassLabel labelToNumericClassLabel;
		labelToNumericClassLabel[uniqueLabels[0]] = -1.0;
		labelToNumericClassLabel[uniqueLabels[1]] = +1.0;
		for (LabeledObservationVector::const_iterator i = labeledObservationVector.begin(); i != labeledObservationVector.end(); i++) {
			y.push_back(labelToNumericClassLabel[i->first]);
		}
		discriminantToLabel[-1.0] = uniqueLabels[0];
		discriminantToLabel[+1.0] = uniqueLabels[1];
	}
}

// the is a convenience function for getting parameter ranges for all kernels
void getDefaultKernelParameterRangeMap(KernelParameterRangeMap& kernelParameterRangeMap) {
	ParameterRangeMap linearParameterRangeMap;
	linearParameterRangeMap[SmoTrainer::MapKey_C] = SmoTrainer::defaultCRange;
	linearParameterRangeMap[LinearKernelFunction::MapKey_Constant] = LinearKernelFunction::defaultConstantRange;

	ParameterRangeMap rbfParameterRangeMap;
	rbfParameterRangeMap[SmoTrainer::MapKey_C] = SmoTrainer::defaultCRange;
	rbfParameterRangeMap[RbfKernelFunction::MapKey_Gamma] = RbfKernelFunction::defaultGammaRange;

	ParameterRangeMap polynomialParameterRangeMap;
	polynomialParameterRangeMap[SmoTrainer::MapKey_C] = SmoTrainer::defaultCRange;
	polynomialParameterRangeMap[PolynomialKernelFunction::MapKey_Constant] = PolynomialKernelFunction::defaultConstantRange;
	polynomialParameterRangeMap[PolynomialKernelFunction::MapKey_Coefficient] = PolynomialKernelFunction::defaultCoefficientRange;
	polynomialParameterRangeMap[PolynomialKernelFunction::MapKey_Degree] = PolynomialKernelFunction::defaultDegreeRange;

	ParameterRangeMap sigmoidParameterRangeMap;
	sigmoidParameterRangeMap[SmoTrainer::MapKey_C] = SmoTrainer::defaultCRange;
	sigmoidParameterRangeMap[SigmoidKernelFunction::MapKey_Alpha] = SigmoidKernelFunction::defaultAlphaRange;
	sigmoidParameterRangeMap[SigmoidKernelFunction::MapKey_Constant] = SigmoidKernelFunction::defaultConstantRange;

	kernelParameterRangeMap[LinearKernelFunction::MapKey] = linearParameterRangeMap;
	kernelParameterRangeMap[RbfKernelFunction::MapKey] = rbfParameterRangeMap;
	kernelParameterRangeMap[PolynomialKernelFunction::MapKey] = polynomialParameterRangeMap;
	kernelParameterRangeMap[SigmoidKernelFunction::MapKey] = sigmoidParameterRangeMap;
}


//
// OneVsOneMultiClassSvmTrainer
//
// An instance of OneVsOneMultiClassSvmTrainer is intended to work with a single set of data
// to produce a single instance of MultiClassSVM.  That's why observations and labels go in to
// the constructor.
OneVsOneMultiClassSvmTrainer::OneVsOneMultiClassSvmTrainer(SvmDataset& d, int e, int t, OutputFilter& of) :
	svmDataset(d),
	evaluationFoldCount(e),
	trainFoldCount(t),
	outputFilter(of) {
	buildLabelSet(labelSet, svmDataset.getLabeledObservationVector());
	buildLabelToLabeledObservationVector(labelToLabeledObservationVector, svmDataset.getLabeledObservationVector());
	buildLabelPairSet(labelPairSet, svmDataset.getLabeledObservationVector());
}

void buildLabelSet(LabelSet& labelSet, const LabeledObservationVector& labeledObservationVector) {
	for (LabeledObservationVector::const_iterator i = labeledObservationVector.begin(); i != labeledObservationVector.end(); i++) {
		labelSet.insert(i->first);
	}
}


//  This function uses the LabeledObservationVector argument to populate the LabelPairSet
//  argument with pairs of labels.  For example, if labeledObservationVector looks like this:
//    [ ("blue", x), ("green", y), ("red", z) ]
//  then the labelPairSet will be populated with the following label pairs:
//    ("blue", "green"), ("blue", "red"), ("green", "red")
//  The order of labels in the pairs is determined by the ordering of labels in the temporary
//  LabelSet.  By default this order will be ascending.  However, labels are taken off the
//  temporary labelStack in reverse order, so the labelStack is initialized with reverse iterators.
//  In the end our label pairs will be in sorted order.
void OneVsOneMultiClassSvmTrainer::buildLabelPairSet(LabelPairSet& labelPairSet, const LabeledObservationVector& labeledObservationVector) {
	//cout << "buildLabelPairSet" << endl;
	LabelSet labelSet;
	buildLabelSet(labelSet, labeledObservationVector);
	LabelVector labelStack(labelSet.rbegin(), labelSet.rend());
	while (labelStack.size() > 1) {
		Label label = labelStack.back();
		labelStack.pop_back();
		LabelPair labelPair(2);
		labelPair[0] = label;
		for (LabelVector::const_iterator i = labelStack.begin(); i != labelStack.end(); i++) {
			labelPair[1] = *i;
			labelPairSet.insert(
				//make_pair(label, *i)
				labelPair
				);
		}
	}
}


// The LabelMatchesEither functor is used only in a call to remove_copy_if in the
// OneVsOneMultiClassSvmTrainer::train method.  It returns true if the labeled
// observation argument has the same label as either of the two label arguments.
class LabelMatchesEither {
public:
	LabelMatchesEither(const Label& _label0, const Label& _label1) : label0(_label0), label1(_label1) {}

	bool operator() (const LabeledObservation& o) {
		return !((o.first == label0) || (o.first == label1));
	}

private:
	const Label& label0;
	const Label& label1;
};

MultiClassSVM* OneVsOneMultiClassSvmTrainer::train(const KernelParameterRangeMap& kernelParameterRangeMap) {
	double bestMultiClassSvmScore = 0.0;
	MultiClassSVM* bestMc;

	KernelFunctionFactory kernelFunctionFactory(svmDataset.getLabeledObservationVector());

	// first divide the data into a 'development' set for tuning hyperparameters
	// and an 'evaluation' set for measuring performance
	int evaluationFoldNumber = 0;
	KFoldLabeledObservationsDivider kFoldDevEvalDivider(evaluationFoldCount, svmDataset.getLabeledObservationVector());
	for (kFoldDevEvalDivider.start(); !kFoldDevEvalDivider.end(); kFoldDevEvalDivider.next()) {
		const LabeledObservationVector& developmentObservations = kFoldDevEvalDivider.getTrainingData();
		const LabeledObservationVector& evaluationObservations = kFoldDevEvalDivider.getTestingData();

		evaluationFoldNumber++;
		if (outputFilter.debug()) {
			LOG(INFO) << "evaluation fold " + toString(evaluationFoldNumber) + " of " + toString(evaluationFoldCount) << '\n';
		}

		vector<SVM*> twoClassSvmList;
		SvmToSvmPerformanceSummary svmToSvmPerformanceSummary;
		SmoTrainer smoTrainer(outputFilter);
		LabelPairSet::iterator labelPair;
		for (labelPair = labelPairSet.begin(); labelPair != labelPairSet.end(); labelPair++) {
			// generate training and testing data for this label pair
			Label label0 = (*labelPair)[0];
			Label label1 = (*labelPair)[1];
			if (outputFilter.debug()) {
				LOG(INFO) << "training SVM on labels " + toString(label0) + " and " + toString(label1) << '\n';
			}

			double bestMeanScoreOnKFolds = 0.0;
			ParameterMap bestParameterMap;
			string bestKernelFunctionKey;
			LabeledObservationVector twoClassDevelopmentObservations;
			LabelMatchesEither labelMatchesEither(label0, label1);
			remove_copy_if(
				developmentObservations.begin(),
				developmentObservations.end(),
				back_inserter(twoClassDevelopmentObservations),
				labelMatchesEither
				//[&](const LabeledObservation& o){
				//    return !((o.first == label0) || (o.first == label1));
				//}
				);
			KFoldLabeledObservationsDivider kFoldLabeledObservationsDivider(trainFoldCount, twoClassDevelopmentObservations);
			// loop on kernel functions and kernel function parameters
			for (KernelParameterRangeMap::const_iterator kmap = kernelParameterRangeMap.begin(); kmap != kernelParameterRangeMap.end(); kmap++) {
				string kernelFunctionKey = kmap->first;
				KernelFunction& kernelFunction = kernelFunctionFactory.getKernelFunctionForKey(kmap->first);
				ParameterSetBuilder p(kmap->second);
				for (ParameterMapVector::const_iterator hp = p.getParameterSetList().begin(); hp != p.getParameterSetList().end(); hp++) {
					kernelFunction.setParameters(*hp);
					KernelFunctionCache kernelFunctionCache(kernelFunction, svmDataset.getLabeledObservationVector());
					smoTrainer.setParameters(*hp);
					if (outputFilter.debug()) {
						LOG(INFO) << "parameters for " + toString(kernelFunctionKey) + " kernel" << '\n';
						for (ParameterMap::const_iterator i = hp->begin(); i != hp->end(); i++) {
							LOG(INFO) << "    " + toString(i->first) + ":" + toString(i->second) << '\n';
						}
					}
					double meanScoreOnKFolds = trainOnKFolds(smoTrainer, kernelFunctionCache, kFoldLabeledObservationsDivider);
					if (meanScoreOnKFolds > bestMeanScoreOnKFolds) {
						bestMeanScoreOnKFolds = meanScoreOnKFolds;
						bestParameterMap = *hp;
						bestKernelFunctionKey = kernelFunctionKey;
					}
				}
			}

			if (bestMeanScoreOnKFolds == 0.0) {
				LOG(INFO) << "failed to train SVM on labels " + toString(label0) + " and " + toString(label1) << '\n';
				throw exception();
			}
			else {
				if (outputFilter.debug()) {
					LOG(INFO) << "trained SVM on labels " + label0 + " and " + label1 << '\n';
					LOG(INFO) << "    best mean score over " + toString(trainFoldCount) + " folds is " + toString(bestMeanScoreOnKFolds) << '\n';
					LOG(INFO) << "    best parameters for " + bestKernelFunctionKey + " kernel" << '\n';
					for (ParameterMap::const_iterator p = bestParameterMap.begin(); p != bestParameterMap.end(); p++) {
						LOG(INFO) << "        " + toString(p->first) + " : " + toString(p->second) << '\n';
					}
				}

				LabelMatchesEither labelMatchesEither(label0, label1);
				LabeledObservationVector twoClassDevelopmentObservations;
				remove_copy_if(
					developmentObservations.begin(),
					developmentObservations.end(),
					back_inserter(twoClassDevelopmentObservations),
					labelMatchesEither
					//[&](const LabeledObservation& o){
					//    return !((o.first == label0) || (o.first == label1));
					//}
					);
				if (outputFilter.info()) {
					LOG(INFO) << "training final SVM with " + toString(twoClassDevelopmentObservations.size()) + " labeled observations" << '\n';
					for (ParameterMap::const_iterator i = bestParameterMap.begin(); i != bestParameterMap.end(); i++) {
						LOG(INFO) << "    " + toString(i->first) + ":" + toString(i->second) << '\n';
					}
				}

				KernelFunction& kernelFunction = kernelFunctionFactory.getKernelFunctionForKey(bestKernelFunctionKey);
				kernelFunction.setParameters(bestParameterMap);
				smoTrainer.setParameters(bestParameterMap);
				KernelFunctionCache kernelFunctionCache(kernelFunction, svmDataset.getLabeledObservationVector());
				SVM* svm = smoTrainer.train(kernelFunctionCache, twoClassDevelopmentObservations);
				//cout << "done training final SVM" << endl;
				twoClassSvmList.push_back(svm);
				// return a performance summary using the evaluation dataset
				LabeledObservationVector twoClassEvaluationObservations;
				remove_copy_if(
					evaluationObservations.begin(),
					evaluationObservations.end(),
					back_inserter(twoClassEvaluationObservations),
					labelMatchesEither
					//[&](const LabeledObservation& o){
					//    return !((o.first == label0) || (o.first == label1));
					//}
					);
				SvmPerformanceSummary p(*svm, twoClassEvaluationObservations);
				svmToSvmPerformanceSummary[svm->getLabelPair()] = p;
			}
		}

		MultiClassSVM* mc = new MultiClassSVM(twoClassSvmList, labelSet, svmToSvmPerformanceSummary, outputFilter);
		//double score = mc->score(evaluationObservations);
		mc->setAccuracy(evaluationObservations);
		if (outputFilter.debug()) {
			LOG(INFO) << "fold " + toString(evaluationFoldNumber) + " multiclass SVM score: " + toString(mc->getAccuracy()) << '\n';
		}
		if (mc->getAccuracy() > bestMultiClassSvmScore) {
			bestMc = mc;
			bestMultiClassSvmScore = mc->getAccuracy();
		}
		else {
			delete mc;
		}
	}

	if (outputFilter.info()) {
		LOG(INFO) << "best multiclass SVM has score " + toString(bestMc->getAccuracy()) << '\n';
	}
	//for ( SvmVector::iterator i = bestMc->getSvmList().begin(); i != bestMc->getSvmList().end(); i++ ) {
	//    SvmPerformanceSummary bestMc->getSvmPerformanceSummary(*i);
	//}
	return bestMc;
}

//SvmTrainingInterruptedException multiClassSvmTrainingInterruptedException("one-vs-one multiclass SVM training interrupted by user");

double OneVsOneMultiClassSvmTrainer::trainOnKFolds(SmoTrainer& smoTrainer, KernelFunctionCache& kernelFunction, KFoldLabeledObservationsDivider& kFoldLabeledObservationsDivider) {
	double meanScoreOverKFolds = 0.0;
	double online_mean_n = 0.0;
	double online_mean_score = 0.0;
	meanScoreOverKFolds = -1.0;  // means we failed to train a SVM

	for (kFoldLabeledObservationsDivider.start(); !kFoldLabeledObservationsDivider.end(); kFoldLabeledObservationsDivider.next()) {
		const LabeledObservationVector& kthTwoClassTrainingFold = kFoldLabeledObservationsDivider.getTrainingData();
		const LabeledObservationVector& kthTwoClassTestingFold = kFoldLabeledObservationsDivider.getTestingData();
		if (outputFilter.info()) {
			LOG(INFO) << "fold " + toString(kFoldLabeledObservationsDivider.getFoldNumber()) + " training data has " + toString(kthTwoClassTrainingFold.size()) + " labeled observations" << '\n';
			LOG(INFO) << "fold " + toString(kFoldLabeledObservationsDivider.getFoldNumber()) + " testing data has " + toString(kthTwoClassTestingFold.size()) + " labeled observations" << '\n';
		}
		if (ctrlc_pressed) { return 0; }
		//        if ( externalSvmTrainingInterruption.interruptTraining() ) {
		//            LOG(INFO) <<  "training interrupted by user"  << '\n';
		//            throw multiClassSvmTrainingInterruptedException;
		//        }
		else {
			try {
				if (outputFilter.debug()) LOG(INFO) << "begin training" << '\n';

				SVM* evaluationSvm = smoTrainer.train(kernelFunction, kthTwoClassTrainingFold);
				SvmPerformanceSummary svmPerformanceSummary(*evaluationSvm, kthTwoClassTestingFold);
				double score = evaluationSvm->score(kthTwoClassTestingFold);
				//double score = svmPerformanceSummary.getAccuracy();
				if (outputFilter.debug()) {
					LOG(INFO) << "score on fold " + toString(kFoldLabeledObservationsDivider.getFoldNumber()) + " of test data is " + toString(score) << '\n';
					LOG(INFO) << "positive label: " + toString(svmPerformanceSummary.getPositiveClassLabel()) << '\n';
					LOG(INFO) << "negative label: " + toString(svmPerformanceSummary.getNegativeClassLabel()) << '\n';
					m->mothurOut("  precision: " + toString(svmPerformanceSummary.getPrecision())
						+ "     recall: " + toString(svmPerformanceSummary.getRecall())
						+ "          f: " + toString(svmPerformanceSummary.getF())
						+ "   accuracy: " + toString(svmPerformanceSummary.getAccuracy())
						); m->mothurOutEndLine();
				}
				online_mean_n += 1.0;
				double online_mean_delta = score - online_mean_score;
				online_mean_score += online_mean_delta / online_mean_n;
				meanScoreOverKFolds = online_mean_score;

				delete evaluationSvm;
			}
			catch (exception& e) {
				LOG(INFO) << "exception: " + toString(e.what()) << '\n';
				LOG(INFO) << "    on fold " + toString(kFoldLabeledObservationsDivider.getFoldNumber()) + " failed to train SVM with C = " + toString(smoTrainer.getC()) << '\n';
			}
		}
	}
	if (outputFilter.debug()) {
		LOG(INFO) << "done with cross validation on C = " + toString(smoTrainer.getC()) << '\n';
		LOG(INFO) << "    mean score over " + toString(kFoldLabeledObservationsDivider.getFoldNumber()) + " folds is " + toString(meanScoreOverKFolds) << '\n';
	}
	if (meanScoreOverKFolds == 0.0) {
		LOG(INFO) << "failed to train SVM with C = " + toString(smoTrainer.getC()) << '\n';
	}
	return meanScoreOverKFolds;
}


class UnrankedFeature {
public:
	UnrankedFeature(const Feature& f) : feature(f), rankingCriterion(0.0) {}
	~UnrankedFeature() {}

	Feature getFeature() const { return feature; }

	double getRankingCriterion() const { return rankingCriterion; }
	void setRankingCriterion(double rc) { rankingCriterion = rc; }

private:
	Feature feature;
	double rankingCriterion;
};

bool lessThanRankingCriterion(const UnrankedFeature& a, const UnrankedFeature& b) {
	return a.getRankingCriterion() < b.getRankingCriterion();
}

bool lessThanFeatureIndex(const UnrankedFeature& a, const UnrankedFeature& b) {
	return a.getFeature().getFeatureIndex() < b.getFeature().getFeatureIndex();
}

typedef list<UnrankedFeature> UnrankedFeatureList;


// Only the linear svm can be used here.
// Consider allowing only parameter ranges as arguments.
// Right now any kernel can be sent in.
// It would be useful to remove more than one feature at a time
// Might make sense to turn last two arguments into one
RankedFeatureList SvmRfe::getOrderedFeatureList(SvmDataset& svmDataset, OneVsOneMultiClassSvmTrainer& t, const ParameterRange& linearKernelConstantRange, const ParameterRange& smoTrainerParameterRange) {

	KernelParameterRangeMap rfeKernelParameterRangeMap;
	ParameterRangeMap linearParameterRangeMap;
	linearParameterRangeMap[SmoTrainer::MapKey_C] = smoTrainerParameterRange;
	linearParameterRangeMap[LinearKernelFunction::MapKey_Constant] = linearKernelConstantRange;

	rfeKernelParameterRangeMap[LinearKernelFunction::MapKey] = linearParameterRangeMap;

	/*
	UnrankedFeatureList unrankedFeatureList;
	//for ( FeatureVector::iterator f = svmDataset.getFeatureVector().begin(); f != svmDataset.getFeatureVector().end(); f++ ) {
	for (int featureIndex = 0; featureIndex < svmDataset.getFeatureVector().size(); featureIndex++) {
		Feature f = svmDataset.getFeatureVector().at(featureIndex);
		unrankedFeatureList.push_back(UnrankedFeature(f));
	}
	*/

	// the rankedFeatureList is empty at first
	RankedFeatureList rankedFeatureList;
	// loop until all but one feature have been eliminated
	// no need to eliminate the last feature, after all
	int svmRfeRound = 0;
	//while ( rankedFeatureList.size() < (svmDataset.getFeatureVector().size()-1) ) {
	while (svmDataset.getFeatureVector().size() > 1) {
		svmRfeRound++;
		LOG(INFO) << "SVM-RFE round " + toString(svmRfeRound) + ":" << '\n';
		UnrankedFeatureList unrankedFeatureList;
		for (int featureIndex = 0; featureIndex < svmDataset.getFeatureVector().size(); featureIndex++) {
			Feature f = svmDataset.getFeatureVector().at(featureIndex);
			unrankedFeatureList.push_back(UnrankedFeature(f));
		}
		LOG(INFO) << toString(unrankedFeatureList.size()) + " unranked features" << '\n';

		MultiClassSVM* s = t.train(rfeKernelParameterRangeMap);
		LOG(INFO) << "multiclass SVM accuracy: " + toString(s->getAccuracy()) << '\n';

		LOG(INFO) << "two-class SVM performance" << '\n';
		int labelFieldWidth = 2 + max_element(s->getLabels().begin(), s->getLabels().end())->size();
		int performanceFieldWidth = 10;
		int performancePrecision = 3;
		LOG(INFO) << "class 1\tclass 2\tprecision\trecall\f\accuracy" << '\n';
		for (SvmVector::const_iterator svm = s->getSvmList().begin(); svm != s->getSvmList().end(); svm++) {
			SvmPerformanceSummary sps = s->getSvmPerformanceSummary(**svm);
			m->mothurOut(toString(sps.getPositiveClassLabel())
				+ toString(sps.getNegativeClassLabel())
				+ toString(sps.getPrecision())
				+ toString(sps.getRecall())
				+ toString(sps.getF())
				+ toString(sps.getAccuracy())); m->mothurOutEndLine();


			//            m->mothurOut( setw(labelFieldWidth) + setprecision(performancePrecision) + sps.getPositiveClassLabel()
			//                      + setw(labelFieldWidth) + setprecision(performancePrecision) + sps.getNegativeClassLabel()
			//                      + setw(performanceFieldWidth) + setprecision(performancePrecision) + sps.getPrecision()
			//                      + setw(performanceFieldWidth) + setprecision(performancePrecision) + sps.getRecall()
			//                      + setw(performanceFieldWidth) + setprecision(performancePrecision) + sps.getF()
			//                      + setw(performanceFieldWidth) + setprecision(performancePrecision) + sps.getAccuracy() ); m->mothurOutEndLine();
		}
		// calculate the 'ranking criterion' for each (remaining) feature using each binary svm
		for (UnrankedFeatureList::iterator f = unrankedFeatureList.begin(); f != unrankedFeatureList.end(); f++) {
			const int i = f->getFeature().getFeatureIndex();
			// rankingCriterion combines feature weights for feature i in all svms
			double rankingCriterion = 0.0;
			for (SvmVector::const_iterator svm = s->getSvmList().begin(); svm != s->getSvmList().end(); svm++) {
				// output SVM performance summary
				// calculate the weight w of feature i for this svm
				double wi = 0.0;
				for (int j = 0; j < (*svm)->x.size(); j++) {
					// all support vectors contribute to wi
					wi += (*svm)->a.at(j) * (*svm)->y.at(j) * (*svm)->x.at(j).second->at(i);
				}
				// accumulate weights for feature i from all svms
				rankingCriterion += pow(wi, 2);
			}
			// update the (unranked) feature ranking criterion
			f->setRankingCriterion(rankingCriterion);
		}
		delete s;

		// sort the unranked features by ranking criterion
		unrankedFeatureList.sort(lessThanRankingCriterion);

		// eliminate the bottom 1/(n+1) features - this is very slow but gives good results
		////int eliminateFeatureCount = ceil(unrankedFeatureList.size() / (iterationCount+1.0));
		// eliminate the bottom 1/3 features - fast but results slightly different from above
		// how about 1/4?
		int eliminateFeatureCount = ceil(unrankedFeatureList.size() / 4.0);
		LOG(INFO) << "eliminating " + toString(eliminateFeatureCount) + " feature(s) of " + toString(unrankedFeatureList.size()) + " total features" << '\n';
		LOG(INFO) << "";
		UnrankedFeatureList featuresToEliminate;
		for (int i = 0; i < eliminateFeatureCount; i++) {
			// remove the lowest ranked feature(s) from the list of unranked features
			UnrankedFeature unrankedFeature = unrankedFeatureList.front();
			unrankedFeatureList.pop_front();

			featuresToEliminate.push_back(unrankedFeature);
			// put the lowest ranked feature at the front of the list of ranked features
			// the first feature to be eliminated will be at the back of this list
			// the last feature to be eliminated will be at the front of this list
			rankedFeatureList.push_front(RankedFeature(unrankedFeature.getFeature(), svmRfeRound));

			/*
			// TODO speed things up by removing the feature completely???
			const int unrankedFeatureIndex = unrankedFeature.getFeature().getFeatureIndex();
			for (LabeledObservationVector::iterator v = svmDataset.getLabeledObservationVector().begin(); v != svmDataset.getLabeledObservationVector().end(); v++) {
				v->second->at(unrankedFeatureIndex) = 0.0;
			}
			*/
		}

		featuresToEliminate.sort(lessThanFeatureIndex);
		reverse(featuresToEliminate.begin(), featuresToEliminate.end());
		for (UnrankedFeatureList::iterator g = featuresToEliminate.begin(); g != featuresToEliminate.end(); g++) {
			Feature unrankedFeature = g->getFeature();
			removeFeature(unrankedFeature, svmDataset.getLabeledObservationVector(), svmDataset.getFeatureVector());
		}
		//cout << "remaining unranked features:" << endl;
		//for (UnrankedFeatureList::iterator g = unrankedFeatureList.begin(); g != unrankedFeatureList.end(); g++) {
		//    cout << "  feature " << g->getFeature().getFeatureLabel() << " with index " << g->getFeature().getFeatureIndex() << endl;
		//}
		// end of experiment

	}

	// there may be one feature left
	svmRfeRound++;
	//cout << unrankedFeatureList.size() << " feature(s) remain" << endl;
	for (FeatureVector::iterator f = svmDataset.getFeatureVector().begin(); f != svmDataset.getFeatureVector().end(); f++) {
		rankedFeatureList.push_front(RankedFeature(*f, svmRfeRound));
	}

	return rankedFeatureList;
}


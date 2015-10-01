/*
 *  validparameter.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/5/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "validparameter.h"

 /***********************************************************************/

ValidParameters::ValidParameters() {
	initParameterRanges();
	commandName = "";
}
/***********************************************************************/

ValidParameters::ValidParameters(string c) {
	initParameterRanges();
	commandName = c;
}

/***********************************************************************/

ValidParameters::~ValidParameters() {}

/***********************************************************************/
bool ValidParameters::isValidParameter(string parameter, vector<string> cParams, string value) {
	try {
		bool valid = false;
		//vector<string> cParams = commandParameters[command];
		int numParams = cParams.size();
		for (int i = 0; i < numParams; i++) {
			if (cParams.at(i).compare(parameter) == 0) {
				valid = true;
				i = numParams;
			}
		}
		if (!valid) {
			LOG(INFO) << parameter + " is not a valid parameter." << '\n';
			LOG(INFO) << "The valid parameters are: ";
			for (int i = 0; i < numParams - 1; i++)
				LOG(INFO) << cParams.at(i) + ", ";
			LOG(INFO) << "and " + cParams.at(numParams - 1) + ".\n";
			return false;
		}

		if (parameterRanges.count(parameter) != 1)
			return true;

		int pVal;
		double piSentinel = 3.14159;
		vector<string> range = parameterRanges[parameter];

		vector<string> values;
		Utility::split(value, '-', values);

		for (int i = 0; i < values.size(); i++) {
			value = values.at(i);
			valid = convertTest(value, pVal);

			if (!valid)
				return false;



			/********************************************************************************************************
				   Special Cases
			*********************************************************************************************************/

			if (parameter.compare("precision") == 0) {
				double logNum = log10((double)pVal);
				double diff = (double)((int)logNum - logNum);
				if (diff != 0) {
					LOG(INFO) << "The precision parameter can only take powers of 10 as a value (e.g. 10,1000,1000, etc.)\n";
					return false;
				}
			}

			/************************************************************************************************************/



			double a, b, c, d, e;

			if (range.at(1).compare("NA") == 0)
				a = piSentinel;
			else
				a = atoi(range.at(1).c_str());

			if (range.at(3).compare("NA") == 0)
				b = piSentinel;
			else
				b = atoi(range.at(3).c_str());

			if (range.at(4).compare("between") == 0)
				c = 0;
			else if (range.at(4).compare("only") == 0)
				c = 1;
			else {
				LOG(INFO) << "The range can only be 'between' or 'only' the bounding numbers.\n";
				return false;
			}

			if (range.at(0).compare(">") == 0)
				d = 0;
			else if (range.at(0).compare(">=") == 0 || range[3].compare("=>") == 0)
				d = 1;
			else {
				LOG(INFO) << "The parameter value can only be '>', '>=', or '=>' the lower bounding number.\n";
				return false;
			}

			if (range.at(2).compare("<") == 0)
				e = 0;
			else if (range.at(2).compare("<=") == 0 || range[4].compare("=<") == 0)
				e = 1;
			else {
				LOG(INFO) << "The parameter value can only be '<', '<=', or '=<' the upper bounding number.\n";
				return false;
			}

			bool a0 = pVal > a;
			bool a1 = pVal >= a;
			bool b0 = pVal < b;
			bool b1 = pVal <= b;

			if (c != 1) {
				if (a != piSentinel && b == piSentinel) {
					if (d == 0)
						valid = a0;
					else
						valid = a1;
				}
				else if (a == piSentinel && b != piSentinel) {
					if (e == 0)
						valid = b0;
					else
						valid = b1;
				}
				else {
					if (d == 0 && e == 0)
						valid = (a0 && b0);
					else if (d == 0 && e == 1)
						valid = (a0 && b1);
					else if (d == 1 && e == 0)
						valid = (a1 && b0);
					else
						valid = (a1 && b1);
				}
			}
			else {
				if (a != piSentinel && b == piSentinel)
					valid = (pVal == a);
				else if (a == piSentinel && b != piSentinel)
					valid = (pVal == b);
				else
					valid = (pVal == a || pVal == b);
			}


			if (!valid) {
				LOG(INFO) << "The '" + parameter + "' parameter needs to be ";
				if (c == 1)
					LOG(INFO) << "either '" + toString(a) + "' or '" + toString(b) + "'.\n";
				else {
					if (a != piSentinel) {
						LOG(INFO) << ">";
						if (d != 0)
							LOG(INFO) << "=";
						LOG(INFO) << " '" + toString(a) + "'";
					}
					if (b == piSentinel)
						LOG(INFO) << "'.\n";
					else if (a != piSentinel)
						LOG(INFO) << " and ";
					if (b != piSentinel) {
						LOG(INFO) << "<";
						if (e != 0)
							LOG(INFO) << "=";
						LOG(INFO) << " '" + toString(b) + "'.\n";
					}
				}
				return false;
			}
		}
		return true;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidParameters, isValidParameters";
		exit(1);
	}
}
/*******************************************************/

/******************************************************/

string ValidParameters::validFile(map<string, string>& container, string parameter, bool isFile) {
	int ableToOpen;

	map<string, string>::iterator it;

	it = container.find(parameter);
	if (it != container.end()) { //no parameter given

		if (isFile == true) {

			if ((it->second == "NONE") || (it->second == "none")) { it->second = "NONE"; }//ignore
			else {

				int pos = (it->second).find(".tx.");
				if (pos != string::npos) { m->sharedHeaderMode = "tax"; }
				else { m->sharedHeaderMode = "otu"; }

				ifstream in;
				ableToOpen = File::openInputFile(it->second, in, "noerror");
				in.close();

				//if you can't open it, try default location
				if (ableToOpen == 1) {
					if (settings.getDefaultPath() != "") { //default path is set
						string tryPath = settings.getDefaultPath() + File::getSimpleName(it->second);
						LOG(INFO) << "Unable to open " + it->second + ". Trying default " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						in2.close();
						container[parameter] = tryPath;
					}
				}

				//if you can't open it, try default location
				if (ableToOpen == 1) {
					if (settings.getOutputDir() != "") { //default path is set
						string tryPath = settings.getOutputDir() + File::getSimpleName(it->second);
						LOG(INFO) << "Unable to open " + it->second + ". Trying output directory " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						container[parameter] = tryPath;
						in2.close();
					}
				}

				if (ableToOpen == 1) {
					LOG(INFO) << "Unable to open " + container[parameter] << '\n';
					return "not open";
				}

				//check phylip file to make sure its really phylip and not column
				if ((it->first == "phylip") && (ableToOpen != 1)) {
					ifstream inPhylip;
					File::openInputFile(it->second, inPhylip);

					string numTest, name;
					inPhylip >> numTest >> name;
					inPhylip.close();

					if (!m->isContainingOnlyDigits(numTest)) { LOG(LOGERROR) << "expected a number and got " + numTest + ". I suspect you entered a column formatted file as a phylip file, aborting." << '\n'; return "not found"; }
				}

				//check for blank file
				if (ableToOpen != 1) {
					if (File::isBlank(container[parameter])) {
						LOG(LOGERROR) << "" + container[parameter] + " is blank, aborting." << '\n'; return "not found";
					}
				}
			}

		}
	}
	else { return "not found"; }

	return it->second;

}

/***********************************************************************/

/***********************************************************************/
void ValidParameters::initParameterRanges() {
	try {
		int rangeSize = 5;

		/**************************************************************************************************************
			{">=" or "=>" or ">" if the value should be greater than or equal to or just greater than the lower bound,
			A number representing the lower bound ("NA" if there is no lower bound),
		   "<=" or "=<" or "<" if the value shoud be less than or equal to or just less than the upper bound,
			A number representing the upper bound ("NA" if there is no lower bound),
		   "between" if between lower and upper bounds or "only" if exactly one of the bounds};

		   # = parameter
		   # (>, >=) lower bound, # (<, <=) upperbound, # should be (between, only) lower and upper bounds.
		   ***********************************************************************************************************/

		string precisionArray[] = { ">=","10", "<","NA", "between" };
		parameterRanges["precision"] = addParameters(precisionArray, rangeSize);

		string itersArray[] = { ">=","1", "<","NA", "between" };
		parameterRanges["iters"] = addParameters(itersArray, rangeSize);

		string abundArray[] = { ">=","5", "<","NA", "between" };
		parameterRanges["abund"] = addParameters(abundArray, rangeSize);

		string softArray[] = { ">=","0", "<=","100", "between" };
		parameterRanges["soft"] = addParameters(softArray, rangeSize);

		string sizeArray[] = { ">=","1", "<","NA", "between" };
		parameterRanges["size"] = addParameters(sizeArray, rangeSize);
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidParameters, initParameterRanges";
		exit(1);
	}
}

/***********************************************************************/

/***********************************************************************/
vector<string> ValidParameters::addParameters(string parameters[], int size) {
	try {
		vector<string> pVector(parameters, parameters + size);
		return pVector;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidParameters, addParameters";
		exit(1);
	}
}
/***********************************************************************/


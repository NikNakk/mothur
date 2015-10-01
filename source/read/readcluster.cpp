/*
 *  readcluster.cpp
 *  Mothur
 *
 *  Created by westcott on 10/28/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "readcluster.h"

 /***********************************************************************/

ReadCluster::ReadCluster(string distfile, float c, string o, bool s) {
	distFile = distfile;
	cutoff = c;
	outputDir = o;
	sortWanted = s;
	list = NULL;
}

/***********************************************************************/

int ReadCluster::read(NameAssignment*& nameMap) {

	if (format == "phylip") { convertPhylip2Column(nameMap); }
	else { list = new ListVector(nameMap->getListVector()); }

	if (ctrlc_pressed) { return 0; }

	if (sortWanted) { OutPutFile = m->sortFile(distFile, outputDir); }
	else { OutPutFile = distFile; } //for use by clusters splitMatrix to convert a phylip matrix to column

	return 0;

}
/***********************************************************************/
int ReadCluster::read(CountTable*& ct) {

	if (format == "phylip") { convertPhylip2Column(ct); }
	else { list = new ListVector(ct->getListVector()); }

	if (ctrlc_pressed) { return 0; }

	if (sortWanted) { OutPutFile = m->sortFile(distFile, outputDir); }
	else { OutPutFile = distFile; } //for use by clusters splitMatrix to convert a phylip matrix to column

	return 0;

}
/***********************************************************************/

int ReadCluster::convertPhylip2Column(NameAssignment*& nameMap) {
	try {
		//convert phylip file to column file
		map<int, string> rowToName;
		map<int, string>::iterator it;

		ifstream in;
		ofstream out;
		string tempFile = distFile + ".column.temp";

		File::openInputFile(distFile, in);  File::gobble(in);
		File::openOutputFile(tempFile, out);

		float distance;
		int square, nseqs;
		string name;
		vector<string> matrixNames;

		string numTest;
		in >> numTest >> name;

		if (!m->isContainingOnlyDigits(numTest)) { LOG(LOGERROR) << "expected a number and got " + numTest + ", quitting." << '\n'; exit(1); }
		else { convert(numTest, nseqs); }

		rowToName[0] = name;
		matrixNames.push_back(name);

		if (nameMap == NULL) {
			list = new ListVector(nseqs);
			list->set(0, name);
		}
		else {
			list = new ListVector(nameMap->getListVector());
			if (nameMap->count(name) == 0) { LOG(INFO) << "Error: Sequence '" + name + "' was not found in the names file, please correct" << '\n'; }
		}

		char d;
		while ((d = in.get()) != EOF) {

			if (isalnum(d)) {
				square = 1;
				in.putback(d);
				for (int i = 0;i < nseqs;i++) {
					in >> distance;
				}
				break;
			}
			if (d == '\n') {
				square = 0;
				break;
			}
		}

		if (square == 0) {

			for (int i = 1;i < nseqs;i++) {
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);

				//there's A LOT of repeated code throughout this method...
				if (nameMap == NULL) {
					list->set(i, name);

					for (int j = 0;j < i;j++) {

						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}

				}
				else {
					if (nameMap->count(name) == 0) { LOG(INFO) << "Error: Sequence '" + name + "' was not found in the names file, please correct" << '\n'; }

					for (int j = 0;j < i;j++) {

						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
			}
		}
		else {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);

				if (nameMap == NULL) {
					list->set(i, name);
					for (int j = 0;j < nseqs;j++) {
						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff && j < i) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
				else {
					if (nameMap->count(name) == 0) { LOG(INFO) << "Error: Sequence '" + name + "' was not found in the names file, please correct" << '\n'; }

					for (int j = 0;j < nseqs;j++) {
						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff && j < i) {
							out << i << '\t' << j << '\t' << distance << endl;
						}

					}
				}
			}
		}

		list->setLabel("0");
		in.close();
		out.close();

		if (nameMap == NULL) {
			nameMap = new NameAssignment();
			for (int i = 0;i < matrixNames.size();i++) {
				nameMap->push_back(matrixNames[i]);
			}
		}


		ifstream in2;
		ofstream out2;

		string outputFile = File::getRootName(distFile) + "column.dist";
		File::openInputFile(tempFile, in2);
		File::openOutputFile(outputFile, out2);

		int first, second;
		float dist;

		while (in2) {
			if (ctrlc_pressed) { in2.close(); out2.close(); File::remove(tempFile); File::remove(outputFile); return 0; }

			in2 >> first >> second >> dist;
			out2 << rowToName[first] << '\t' << rowToName[second] << '\t' << dist << endl;
			File::gobble(in2);
		}
		in2.close();
		out2.close();

		File::remove(tempFile);
		distFile = outputFile;

		if (ctrlc_pressed) { File::remove(outputFile); }

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReadCluster, convertPhylip2Column";
		exit(1);
	}
}
/***********************************************************************/

int ReadCluster::convertPhylip2Column(CountTable*& ct) {
	try {
		//convert phylip file to column file
		map<int, string> rowToName;
		map<int, string>::iterator it;

		ifstream in;
		ofstream out;
		string tempFile = distFile + ".column.temp";

		File::openInputFile(distFile, in);  File::gobble(in);
		File::openOutputFile(tempFile, out);

		float distance;
		int square, nseqs;
		string name;
		vector<string> matrixNames;

		string numTest;
		in >> numTest >> name;

		if (!m->isContainingOnlyDigits(numTest)) { LOG(LOGERROR) << "expected a number and got " + numTest + ", quitting." << '\n'; exit(1); }
		else { convert(numTest, nseqs); }

		rowToName[0] = name;
		matrixNames.push_back(name);

		if (ct == NULL) {
			list = new ListVector(nseqs);
			list->set(0, name);
		}
		else { list = new ListVector(ct->getListVector()); }

		char d;
		while ((d = in.get()) != EOF) {

			if (isalnum(d)) {
				square = 1;
				in.putback(d);
				for (int i = 0;i < nseqs;i++) {
					in >> distance;
				}
				break;
			}
			if (d == '\n') {
				square = 0;
				break;
			}
		}

		if (square == 0) {

			for (int i = 1;i < nseqs;i++) {
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);

				//there's A LOT of repeated code throughout this method...
				if (ct == NULL) {
					list->set(i, name);

					for (int j = 0;j < i;j++) {

						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}

				}
				else {

					for (int j = 0;j < i;j++) {

						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
			}
		}
		else {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);

				if (ct == NULL) {
					list->set(i, name);
					for (int j = 0;j < nseqs;j++) {
						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff && j < i) {
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
				else {
					for (int j = 0;j < nseqs;j++) {
						if (ctrlc_pressed) { in.close(); out.close(); File::remove(tempFile); return 0; }

						in >> distance;

						if (distance == -1) { distance = 1000000; }

						if (distance < cutoff && j < i) {
							out << i << '\t' << j << '\t' << distance << endl;
						}

					}
				}
			}
		}

		list->setLabel("0");
		in.close();
		out.close();

		if (ct == NULL) {
			ct = new CountTable();
			for (int i = 0;i < matrixNames.size();i++) {
				ct->push_back(matrixNames[i]);
			}
		}


		ifstream in2;
		ofstream out2;

		string outputFile = File::getRootName(distFile) + "column.dist";
		File::openInputFile(tempFile, in2);
		File::openOutputFile(outputFile, out2);

		int first, second;
		float dist;

		while (in2) {
			if (ctrlc_pressed) { in2.close(); out2.close(); File::remove(tempFile); File::remove(outputFile); return 0; }

			in2 >> first >> second >> dist;
			out2 << rowToName[first] << '\t' << rowToName[second] << '\t' << dist << endl;
			File::gobble(in2);
		}
		in2.close();
		out2.close();

		File::remove(tempFile);
		distFile = outputFile;

		if (ctrlc_pressed) { File::remove(outputFile); }

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReadCluster, convertPhylip2Column";
		exit(1);
	}
}
/***********************************************************************/

ReadCluster::~ReadCluster() {}
/***********************************************************************/


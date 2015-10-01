/*
 *  validcalculator.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/5/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "validcalculator.h"
#include "ace.h"
#include "sobs.h"
#include "nseqs.h"
#include "chao1.h"
#include "bootstrap.h"
#include "simpson.h"
#include "simpsoneven.h"
#include "invsimpson.h"
#include "npshannon.h"
#include "shannon.h"
#include "smithwilson.h"
#include "heip.h"
#include "shannoneven.h"
#include "jackknife.h"
#include "geom.h"
#include "qstat.h"
#include "logsd.h"
#include "bergerparker.h"
#include "bstick.h"
#include "goodscoverage.h"
#include "efron.h"
#include "boneh.h"
#include "solow.h"
#include "shen.h"
#include "coverage.h"
#include "sharedsobscollectsummary.h"
#include "sharedchao1.h"
#include "sharedace.h"
#include "sharedjabund.h"
#include "sharedsorabund.h"
#include "sharedjclass.h"
#include "sharedsorclass.h"
#include "sharedjest.h"
#include "sharedsorest.h"
#include "sharedthetayc.h"
#include "sharedthetan.h"
#include "sharedkstest.h"
#include "whittaker.h"
#include "sharednseqs.h"
#include "sharedochiai.h"
#include "sharedanderbergs.h"
#include "sharedkulczynski.h"
#include "sharedkulczynskicody.h"
#include "sharedlennon.h"
#include "sharedmorisitahorn.h"
#include "sharedbraycurtis.h"
#include "sharedjackknife.h"
#include "whittaker.h"
#include "odum.h"
#include "canberra.h"
#include "structeuclidean.h"
#include "structchord.h"
#include "hellinger.h"
#include "manhattan.h"
#include "structpearson.h"
#include "soergel.h"
#include "spearman.h"
#include "structkulczynski.h"
#include "structchi2.h"
#include "speciesprofile.h"
#include "hamming.h"
#include "gower.h"
#include "memchi2.h"
#include "memchord.h"
#include "memeuclidean.h"
#include "mempearson.h"
#include "sharedsobs.h"
#include "sharednseqs.h"
#include "sharedjsd.h"
#include "sharedrjsd.h"
#include "shannonrange.h"


 /********************************************************************/
ValidCalculators::ValidCalculators() {

	initialSingle();
	initialShared();
	initialRarefaction();
	initialSharedRarefact();
	initialSummary();
	initialSharedSummary();
	initialVennSingle();
	initialVennShared();
	initialTreeGroups();
	initialBoot();
	initialDistance();
	initialMatrix();
	initialHeat();

	for (it = single.begin(); it != single.end(); it++) { allCalcs.insert(it->first); }
	for (it = shared.begin(); it != shared.end(); it++) { allCalcs.insert(it->first); }
	for (it = rarefaction.begin(); it != rarefaction.end(); it++) { allCalcs.insert(it->first); }
	for (it = summary.begin(); it != summary.end(); it++) { allCalcs.insert(it->first); }
	for (it = sharedrarefaction.begin(); it != sharedrarefaction.end(); it++) { allCalcs.insert(it->first); }
	for (it = sharedsummary.begin(); it != sharedsummary.end(); it++) { allCalcs.insert(it->first); }
	for (it = vennsingle.begin(); it != vennsingle.end(); it++) { allCalcs.insert(it->first); }
	for (it = vennshared.begin(); it != vennshared.end(); it++) { allCalcs.insert(it->first); }
	for (it = treegroup.begin(); it != treegroup.end(); it++) { allCalcs.insert(it->first); }
	for (it = matrix.begin(); it != matrix.end(); it++) { allCalcs.insert(it->first); }
	for (it = heat.begin(); it != heat.end(); it++) { allCalcs.insert(it->first); }
	for (it = boot.begin(); it != boot.end(); it++) { allCalcs.insert(it->first); }
	for (it = distance.begin(); it != distance.end(); it++) { allCalcs.insert(it->first); }

}
/********************************************************************/
ValidCalculators::~ValidCalculators() {}
/********************************************************************/
void ValidCalculators::printCitations(vector<string> Estimators) {

	for (int i = 0; i < Estimators.size(); i++) {
		//is this citation, do nothing
		if ((Estimators[i] == "citation") || (Estimators[i] == "default") || (Estimators[i] == "eachgap") || (Estimators[i] == "nogaps") || (Estimators[i] == "onegap")) {}
		//is this a valid calculator
		else if (allCalcs.count(Estimators[i]) != 0) {
			if (Estimators[i] == "sobs") {
				Calculator* temp = new Sobs(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "chao") {
				Calculator* temp = new Chao1(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "nseqs") {
				Calculator* temp = new NSeqs(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "coverage") {
				Calculator* temp = new Coverage(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "ace") {
				Calculator* temp = new Ace(10); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "jack") {
				Calculator* temp = new Jackknife(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "shannon") {
				Calculator* temp = new Shannon(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "shannoneven") {
				Calculator* temp = new ShannonEven(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "shannonrange") {
				Calculator* temp = new RangeShannon(0); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "npshannon") {
				Calculator* temp = new NPShannon(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "heip") {
				Calculator* temp = new Heip(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "smithwilson") {
				Calculator* temp = new SmithWilson(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "simpson") {
				Calculator* temp = new Simpson(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "simpsoneven") {
				Calculator* temp = new SimpsonEven(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "invsimpson") {
				Calculator* temp = new InvSimpson(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "bootstrap") {
				Calculator* temp = new Bootstrap(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "geometric") {
				Calculator* temp = new Geom(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "qstat") {
				Calculator* temp = new QStat(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "logseries") {
				Calculator* temp = new LogSD(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "bergerparker") {
				Calculator* temp = new BergerParker(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "bstick") {
				Calculator* temp = new BStick(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "goodscoverage") {
				Calculator* temp = new GoodsCoverage(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "efron") {
				Calculator* temp = new Efron(10); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "boneh") {
				Calculator* temp = new Boneh(10); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "solow") {
				Calculator* temp = new Solow(10); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "shen") {
				Calculator* temp = new Shen(10, 10); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "sharedchao") {
				Calculator* temp = new SharedChao1(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sharedsobs") {
				Calculator* temp = new SharedSobsCS(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sharedace") {
				Calculator* temp = new SharedAce(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "jabund") {
				Calculator* temp = new JAbund(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sorabund") {
				Calculator* temp = new SorAbund(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "jclass") {
				Calculator* temp = new Jclass(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sorclass") {
				Calculator* temp = new SorClass(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "jest") {
				Calculator* temp = new Jest(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sorest") {
				Calculator* temp = new SorEst(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "thetayc") {
				Calculator* temp = new ThetaYC(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "thetan") {
				Calculator* temp = new ThetaN(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "kstest") {
				Calculator* temp = new KSTest(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "whittaker") {
				Calculator* temp = new Whittaker(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sharednseqs") {
				Calculator* temp = new SharedNSeqs(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "ochiai") {
				Calculator* temp = new Ochiai(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "anderberg") {
				Calculator* temp = new Anderberg(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "kulczynski") {
				Calculator* temp = new Kulczynski(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "kulczynskicody") {
				Calculator* temp = new KulczynskiCody(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "lennon") {
				Calculator* temp = new Lennon(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "morisitahorn") {
				Calculator* temp = new MorHorn(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "braycurtis") {
				Calculator* temp = new BrayCurtis(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "odum") {
				Calculator* temp = new Odum(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "canberra") {
				Calculator* temp = new Canberra(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "structeuclidean") {
				Calculator* temp = new StructEuclidean(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "structchord") {
				Calculator* temp = new StructChord(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "hellinger") {
				Calculator* temp = new Hellinger(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "manhattan") {
				Calculator* temp = new Manhattan(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "structpearson") {
				Calculator* temp = new StructPearson(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "soergel") {
				Calculator* temp = new Soergel(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "spearman") {
				Calculator* temp = new Spearman(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "structkulczynski") {
				Calculator* temp = new StructKulczynski(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "speciesprofile") {
				Calculator* temp = new SpeciesProfile(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "hamming") {
				Calculator* temp = new Hamming(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "structchi2") {
				Calculator* temp = new StructChi2(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "gower") {
				Calculator* temp = new Gower(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;

			}
			else if (Estimators[i] == "memchi2") {
				Calculator* temp = new MemChi2(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "memchord") {
				Calculator* temp = new MemChord(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "memeuclidean") {
				Calculator* temp = new MemEuclidean(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "mempearson") {
				Calculator* temp = new MemPearson(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "sharedobserved") {
				Calculator* temp = new SharedSobs(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "kulczynski") {
				Calculator* temp = new Kulczynski(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "jsd") {
				Calculator* temp = new JSD(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else if (Estimators[i] == "rjsd") {
				Calculator* temp = new RJSD(); LOG(INFO) << temp->getName() + ": "; temp->citation(); delete temp;
			}
			else { LOG(LOGERROR) << "Missing else if for " + Estimators[i] + " in printCitations." << '\n'; }
		}
		else { LOG(INFO) << Estimators[i] + " is not a valid calculator, no citation will be given." << '\n'; }
	}

}
/********************************************************************/

bool ValidCalculators::isValidCalculator(string parameter, string calculator) {
	try {
		//are you looking for a calculator for a single parameter
		if (parameter == "single") {
			//is it valid
			if ((single.find(calculator)) != (single.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the collect.single command and will be disregarded. Valid estimators are ";
				for (it = single.begin(); it != single.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
			//are you looking for a calculator for a shared parameter
		}
		else if (parameter == "shared") {
			//is it valid
			if ((shared.find(calculator)) != (shared.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the collect.shared command and will be disregarded.  Valid estimators are ";
				for (it = shared.begin(); it != shared.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
			//are you looking for a calculator for a rarefaction parameter
		}
		else if (parameter == "rarefaction") {
			//is it valid
			if ((rarefaction.find(calculator)) != (rarefaction.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the rarefaction.single command and will be disregarded. Valid estimators are ";
				for (it = rarefaction.begin(); it != rarefaction.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
			//are you looking for a calculator for a summary parameter
		}
		else if (parameter == "summary") {
			//is it valid
			if ((summary.find(calculator)) != (summary.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the summary.shared command and will be disregarded. Valid estimators are ";
				for (it = summary.begin(); it != summary.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
			//are you looking for a calculator for a sharedsummary parameter
		}
		else if (parameter == "sharedsummary") {
			//is it valid
			if ((sharedsummary.find(calculator)) != (sharedsummary.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the summary.shared command and will be disregarded. Valid estimators are ";
				for (it = sharedsummary.begin(); it != sharedsummary.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "sharedrarefaction") {
			//is it valid
			if ((sharedrarefaction.find(calculator)) != (sharedrarefaction.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the rarefaction.shared command and will be disregarded. Valid estimator is ";
				for (it = sharedrarefaction.begin(); it != sharedrarefaction.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "vennsingle") {
			//is it valid
			if ((vennsingle.find(calculator)) != (vennsingle.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the venn command in single mode and will be disregarded. Valid estimators are ";
				for (it = vennsingle.begin(); it != vennsingle.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "vennshared") {
			//is it valid
			if ((vennshared.find(calculator)) != (vennshared.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the venn command in shared mode and will be disregarded. Valid estimators are ";
				for (it = vennshared.begin(); it != vennshared.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "treegroup") {
			//is it valid
			if ((treegroup.find(calculator)) != (treegroup.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the tree.shared command and will be disregarded. Valid estimators are ";
				for (it = treegroup.begin(); it != treegroup.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "matrix") {
			//is it valid
			if ((matrix.find(calculator)) != (matrix.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the matrix.output command and will be disregarded. Valid estimators are ";
				for (it = matrix.begin(); it != matrix.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "heat") {
			//is it valid
			if ((heat.find(calculator)) != (heat.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the heatmap.sim command and will be disregarded. Valid estimators are ";
				for (it = heat.begin(); it != heat.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "boot") {
			//is it valid
			if ((boot.find(calculator)) != (boot.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the bootstrap.shared command and will be disregarded. Valid estimators are ";
				for (it = boot.begin(); it != boot.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
		}
		else if (parameter == "distance") {
			//is it valid
			if ((distance.find(calculator)) != (distance.end())) {
				return true;
			}
			else {
				LOG(INFO) << calculator + " is not a valid estimator for the distance command and will be disregarded. Valid calculators are ";
				for (it = distance.begin(); it != distance.end(); it++) {
					LOG(INFO) << it->first + ", ";
				}
				LOG(INFO) << "";
				return false;
			}
			//not a valid parameter
		}
		else { return false; }

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, isValidCalculator";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialSingle() {
	try {
		single["sobs"] = "sobs";
		single["chao"] = "chao";
		single["ace"] = "ace";
		single["jack"] = "jack";
		single["shannon"] = "shannon";
		single["npshannon"] = "npshannon";
		single["shannoneven"] = "shannoneven";
		single["shannonrange"] = "shannonrange";
		single["smithwilson"] = "smithwilson";
		single["heip"] = "heip";
		single["simpson"] = "simpson";
		single["simpsoneven"] = "simpsoneven";
		single["invsimpson"] = "invsimpson";
		single["bergerparker"] = "bergerparker";
		single["bootstrap"] = "bootstrap";
		single["geometric"] = "geometric";
		single["logseries"] = "logseries";
		single["qstat"] = "qstat";
		single["bstick"] = "bstick";
		single["goodscoverage"] = "goodscoverage";
		single["nseqs"] = "nseqs";
		single["coverage"] = "coverage";
		single["efron"] = "efron";
		single["boneh"] = "boneh";
		single["solow"] = "solow";
		single["shen"] = "shen";
		single["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialSingle";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialShared() {
	try {
		shared["sharedsobs"] = "sharedsobs";
		shared["sharedchao"] = "sharedchao";
		shared["sharedace"] = "sharedace";
		shared["jabund"] = "jabund";
		shared["sorabund"] = "sorabund";
		shared["jclass"] = "jclass";
		shared["sorclass"] = "sorclass";
		shared["jest"] = "jest";
		shared["sorest"] = "sorest";
		shared["thetayc"] = "thetayc";
		shared["thetan"] = "thetan";
		shared["kstest"] = "kstest";
		shared["whittaker"] = "whittaker";
		shared["sharednseqs"] = "sharednseqs";
		shared["ochiai"] = "ochiai";
		shared["anderberg"] = "anderberg";
		shared["kulczynski"] = "kulczynski";
		shared["kulczynskicody"] = "kulczynskicody";
		shared["lennon"] = "lennon";
		shared["morisitahorn"] = "morisitahorn";
		shared["braycurtis"] = "braycurtis";
		shared["odum"] = "odum";
		shared["canberra"] = "canberra";
		shared["structeuclidean"] = "structeuclidean";
		shared["structchord"] = "structchord";
		shared["hellinger"] = "hellinger";
		shared["manhattan"] = "manhattan";
		shared["structpearson"] = "structpearson";
		shared["soergel"] = "soergel";
		shared["spearman"] = "spearman";
		shared["structkulczynski"] = "structkulczynski";
		shared["structchi2"] = "structchi2";
		shared["speciesprofile"] = "speciesprofile";
		shared["hamming"] = "hamming";
		shared["gower"] = "gower";
		shared["memchi2"] = "memchi2";
		shared["memchord"] = "memchord";
		shared["memeuclidean"] = "memeuclidean";
		shared["mempearson"] = "mempearson";
		shared["jsd"] = "jsd";
		shared["rjsd"] = "rjsd";
		shared["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialShared";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialRarefaction() {
	try {
		rarefaction["sobs"] = "sobs";
		rarefaction["chao"] = "chao";
		rarefaction["ace"] = "ace";
		rarefaction["jack"] = "jack";
		rarefaction["shannon"] = "shannon";
		rarefaction["smithwilson"] = "smithwilson";
		rarefaction["heip"] = "heip";
		rarefaction["npshannon"] = "npshannon";
		rarefaction["shannoneven"] = "shannoneven";
		rarefaction["shannonrange"] = "shannonrange";
		rarefaction["simpson"] = "simpson";
		rarefaction["invsimpson"] = "invsimpson";
		rarefaction["simpsoneven"] = "simpsoneven";
		rarefaction["bootstrap"] = "bootstrap";
		rarefaction["nseqs"] = "nseqs";
		rarefaction["coverage"] = "coverage";
		rarefaction["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialRarefaction";
		exit(1);
	}
}

/********************************************************************/

void ValidCalculators::initialSummary() {
	try {
		summary["sobs"] = "sobs";
		summary["chao"] = "chao";
		summary["ace"] = "ace";
		summary["jack"] = "jack";
		summary["shannon"] = "shannon";
		summary["heip"] = "heip";
		summary["shannoneven"] = "shannoneven";
		summary["smithwilson"] = "smithwilson";
		summary["invsimpson"] = "invsimpson";
		summary["npshannon"] = "npshannon";
		summary["shannonrange"] = "shannonrange";
		summary["simpson"] = "simpson";
		summary["simpsoneven"] = "simpsoneven";
		summary["bergerparker"] = "bergerparker";
		summary["geometric"] = "geometric";
		summary["bootstrap"] = "bootstrap";
		summary["logseries"] = "logseries";
		summary["qstat"] = "qstat";
		summary["bstick"] = "bstick";
		summary["nseqs"] = "nseqs";
		summary["goodscoverage"] = "goodscoverage";
		summary["coverage"] = "coverage";
		summary["efron"] = "efron";
		summary["boneh"] = "boneh";
		summary["solow"] = "solow";
		summary["shen"] = "shen";
		summary["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialSummary";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialSharedSummary() {
	try {
		sharedsummary["sharedsobs"] = "sharedsobs";
		sharedsummary["sharedchao"] = "sharedchao";
		sharedsummary["sharedace"] = "sharedace";
		sharedsummary["jabund"] = "jabund";
		sharedsummary["sorabund"] = "sorabund";
		sharedsummary["jclass"] = "jclass";
		sharedsummary["sorclass"] = "sorclass";
		sharedsummary["jest"] = "jest";
		sharedsummary["sorest"] = "sorest";
		sharedsummary["thetayc"] = "thetayc";
		sharedsummary["thetan"] = "thetan";
		sharedsummary["kstest"] = "kstest";
		sharedsummary["whittaker"] = "whittaker";
		sharedsummary["sharednseqs"] = "sharednseqs";
		sharedsummary["ochiai"] = "ochiai";
		sharedsummary["anderberg"] = "anderberg";
		sharedsummary["kulczynski"] = "kulczynski";
		sharedsummary["kulczynskicody"] = "kulczynskicody";
		sharedsummary["lennon"] = "lennon";
		sharedsummary["morisitahorn"] = "morisitahorn";
		sharedsummary["braycurtis"] = "braycurtis";
		sharedsummary["odum"] = "odum";
		sharedsummary["canberra"] = "canberra";
		sharedsummary["structeuclidean"] = "structeuclidean";
		sharedsummary["structchord"] = "structchord";
		sharedsummary["hellinger"] = "hellinger";
		sharedsummary["manhattan"] = "manhattan";
		sharedsummary["structpearson"] = "structpearson";
		sharedsummary["structkulczynski"] = "structkulczynski";
		sharedsummary["structchi2"] = "structchi2";
		sharedsummary["soergel"] = "soergel";
		sharedsummary["spearman"] = "spearman";
		sharedsummary["speciesprofile"] = "speciesprofile";
		sharedsummary["hamming"] = "hamming";
		sharedsummary["gower"] = "gower";
		sharedsummary["memchi2"] = "memchi2";
		sharedsummary["memchord"] = "memchord";
		sharedsummary["memeuclidean"] = "memeuclidean";
		sharedsummary["mempearson"] = "mempearson";
		sharedsummary["jsd"] = "jsd";
		sharedsummary["rjsd"] = "rjsd";
		sharedsummary["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialSharedSummary";
		exit(1);
	}
}


/********************************************************************/

void ValidCalculators::initialSharedRarefact() {
	try {
		sharedrarefaction["sharedobserved"] = "sharedobserved";
		sharedrarefaction["sharednseqs"] = "sharednseqs";
		sharedrarefaction["default"] = "default";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialSharedRarefact";
		exit(1);
	}
}


/********************************************************************/
void ValidCalculators::initialVennSingle() {
	vennsingle["sobs"] = "sobs";
	vennsingle["chao"] = "chao";
	vennsingle["ace"] = "ace";
	vennsingle["jack"] = "jack";
	vennsingle["default"] = "default";
}

/********************************************************************/
void ValidCalculators::initialVennShared() {
	vennshared["sharedsobs"] = "sharedsobs";
	vennshared["sharedchao"] = "sharedchao";
	vennshared["sharedace"] = "sharedace";
	vennshared["default"] = "default";
}

/********************************************************************/
void ValidCalculators::initialTreeGroups() {
	try {
		treegroup["sharedsobs"] = "sharedsobs";
		treegroup["sharedchao"] = "sharedchao";
		treegroup["sharedace"] = "sharedace";
		treegroup["jabund"] = "jabund";
		treegroup["sorabund"] = "sorabund";
		treegroup["jclass"] = "jclass";
		treegroup["sorclass"] = "sorclass";
		treegroup["jest"] = "jest";
		treegroup["sorest"] = "sorest";
		treegroup["thetayc"] = "thetayc";
		treegroup["thetan"] = "thetan";
		treegroup["kstest"] = "kstest";
		treegroup["whittaker"] = "whittaker";
		treegroup["sharednseqs"] = "sharednseqs";
		treegroup["ochiai"] = "ochiai";
		treegroup["anderberg"] = "anderberg";
		treegroup["kulczynski"] = "kulczynski";
		treegroup["kulczynskicody"] = "kulczynskicody";
		treegroup["lennon"] = "lennon";
		treegroup["morisitahorn"] = "morisitahorn";
		treegroup["braycurtis"] = "braycurtis";
		treegroup["odum"] = "odum";
		treegroup["canberra"] = "canberra";
		treegroup["structeuclidean"] = "structeuclidean";
		treegroup["structchord"] = "structchord";
		treegroup["hellinger"] = "hellinger";
		treegroup["manhattan"] = "manhattan";
		treegroup["structpearson"] = "structpearson";
		treegroup["structkulczynski"] = "structkulczynski";
		treegroup["structchi2"] = "structchi2";
		treegroup["soergel"] = "soergel";
		treegroup["spearman"] = "spearman";
		treegroup["speciesprofile"] = "speciesprofile";
		treegroup["hamming"] = "hamming";
		treegroup["gower"] = "gower";
		treegroup["memchi2"] = "memchi2";
		treegroup["memchord"] = "memchord";
		treegroup["jsd"] = "jsd";
		treegroup["rjsd"] = "rjsd";
		treegroup["memeuclidean"] = "memeuclidean";
		treegroup["mempearson"] = "mempearson";

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialTreeGroups";
		exit(1);
	}
}
/********************************************************************/
void ValidCalculators::initialHeat() {
	try {
		heat["jabund"] = "jabund";
		heat["sorabund"] = "sorabund";
		heat["jclass"] = "jclass";
		heat["sorclass"] = "sorclass";
		heat["jest"] = "jest";
		heat["sorest"] = "sorest";
		heat["thetayc"] = "thetayc";
		heat["thetan"] = "thetan";
		heat["morisitahorn"] = "morisitahorn";
		heat["braycurtis"] = "braycurtis";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialHeat";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialMatrix() {
	try {
		matrix["sharedsobs"] = "sharedsobs";
		matrix["sharedchao"] = "sharedchao";
		matrix["sharedace"] = "sharedace";
		matrix["jabund"] = "jabund";
		matrix["sorabund"] = "sorabund";
		matrix["jclass"] = "jclass";
		matrix["sorclass"] = "sorclass";
		matrix["jest"] = "jest";
		matrix["sorest"] = "sorest";
		matrix["thetayc"] = "thetayc";
		matrix["thetan"] = "thetan";
		matrix["kstest"] = "kstest";
		matrix["whittaker"] = "whittaker";
		matrix["sharednseqs"] = "sharednseqs";
		matrix["ochiai"] = "ochiai";
		matrix["anderberg"] = "anderberg";
		matrix["kulczynski"] = "kulczynski";
		matrix["kulczynskicody"] = "kulczynskicody";
		matrix["lennon"] = "lennon";
		matrix["morisitahorn"] = "morisitahorn";
		matrix["braycurtis"] = "braycurtis";
		matrix["odum"] = "odum";
		matrix["canberra"] = "canberra";
		matrix["structeuclidean"] = "structeuclidean";
		matrix["structchord"] = "structchord";
		matrix["hellinger"] = "hellinger";
		matrix["manhattan"] = "manhattan";
		matrix["structpearson"] = "structpearson";
		matrix["structkulczynski"] = "structkulczynski";
		matrix["structchi2"] = "structchi2";
		matrix["soergel"] = "soergel";
		matrix["spearman"] = "spearman";
		matrix["speciesprofile"] = "speciesprofile";
		matrix["hamming"] = "hamming";
		matrix["gower"] = "gower";
		matrix["memchi2"] = "memchi2";
		matrix["memchord"] = "memchord";
		matrix["memeuclidean"] = "memeuclidean";
		matrix["mempearson"] = "mempearson";
		matrix["rjsd"] = "rjsd";
		matrix["jsd"] = "jsd";

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialMatrix";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::initialBoot() {
	try {
		boot["jabund"] = "jabund";
		boot["sorabund"] = "sorabund";
		boot["jclass"] = "jclass";
		boot["sorclass"] = "orclass";
		boot["jest"] = "jest";
		boot["sorest"] = "sorest";
		boot["thetayc"] = "thetayc";
		boot["thetan"] = "thetan";
		boot["morisitahorn"] = "morisitahorn";
		boot["braycurtis"] = "braycurtis";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialBoot";
		exit(1);
	}
}
/********************************************************************/
void ValidCalculators::initialDistance() {
	try {
		distance["nogaps"] = "nogaps";
		distance["eachgap"] = "eachgap";
		distance["onegap"] = "onegap";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, initialDistance";
		exit(1);
	}
}

/********************************************************************/
void ValidCalculators::printCalc(string parameter, ostream& out) {
	try {
		out << "The available estimators for calc are ";
		//are you looking for a calculator for a single parameter
		if (parameter == "single") {
			for (it = single.begin(); it != single.end(); it++) {
				out << it->first << ", ";
			}
			//are you looking for a calculator for a shared parameter
		}
		else if (parameter == "shared") {
			for (it = shared.begin(); it != shared.end(); it++) {
				out << it->first << ", ";
			}
			//are you looking for a calculator for a rarefaction parameter
		}
		else if (parameter == "rarefaction") {
			for (it = rarefaction.begin(); it != rarefaction.end(); it++) {
				out << it->first << ", ";
			}
			//are you looking for a calculator for a summary parameter
		}
		else if (parameter == "summary") {
			for (it = summary.begin(); it != summary.end(); it++) {
				out << it->first << ", ";
			}
			//are you looking for a calculator for a sharedsummary parameter
		}
		else if (parameter == "sharedsummary") {
			for (it = sharedsummary.begin(); it != sharedsummary.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "sharedrarefaction") {
			for (it = sharedrarefaction.begin(); it != sharedrarefaction.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "vennsingle") {
			for (it = vennsingle.begin(); it != vennsingle.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "vennshared") {
			for (it = vennshared.begin(); it != vennshared.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "treegroup") {
			for (it = treegroup.begin(); it != treegroup.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "matrix") {
			for (it = matrix.begin(); it != matrix.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "heat") {
			for (it = heat.begin(); it != heat.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "boot") {
			for (it = boot.begin(); it != boot.end(); it++) {
				out << it->first << ", ";
			}
		}
		else if (parameter == "distance") {
			for (it = distance.begin(); it != distance.end(); it++) {
				out << it->first << ", ";
			}
		}

		out << endl;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, printCalc";
		exit(1);
	}
}
/********************************************************************/
string ValidCalculators::printCalc(string parameter) {
	try {
		string output = "The available estimators for calc are ";
		//are you looking for a calculator for a single parameter
		if (parameter == "single") {
			for (it = single.begin(); it != single.end(); it++) {
				output += it->first + ", ";
			}
			//are you looking for a calculator for a shared parameter
		}
		else if (parameter == "shared") {
			for (it = shared.begin(); it != shared.end(); it++) {
				output += it->first + ", ";
			}
			//are you looking for a calculator for a rarefaction parameter
		}
		else if (parameter == "rarefaction") {
			for (it = rarefaction.begin(); it != rarefaction.end(); it++) {
				output += it->first + ", ";
			}
			//are you looking for a calculator for a summary parameter
		}
		else if (parameter == "summary") {
			for (it = summary.begin(); it != summary.end(); it++) {
				output += it->first + ", ";
			}
			//are you looking for a calculator for a sharedsummary parameter
		}
		else if (parameter == "sharedsummary") {
			for (it = sharedsummary.begin(); it != sharedsummary.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "sharedrarefaction") {
			for (it = sharedrarefaction.begin(); it != sharedrarefaction.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "vennsingle") {
			for (it = vennsingle.begin(); it != vennsingle.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "vennshared") {
			for (it = vennshared.begin(); it != vennshared.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "treegroup") {
			for (it = treegroup.begin(); it != treegroup.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "matrix") {
			for (it = matrix.begin(); it != matrix.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "heat") {
			for (it = heat.begin(); it != heat.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "boot") {
			for (it = boot.begin(); it != boot.end(); it++) {
				output += it->first + ", ";
			}
		}
		else if (parameter == "distance") {
			for (it = distance.begin(); it != distance.end(); it++) {
				output += it->first + ", ";
			}
		}

		//rip off comma
		output = output.substr(0, output.length() - 1);
		output += "\n";

		return output;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ValidCalculator, printCalc";
		exit(1);
	}
}
/********************************************************************/



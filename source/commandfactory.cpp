/*
*  commandfactory.cpp
*
*
*  Created by Pat Schloss on 10/25/08.
*  Copyright 2008 Patrick D. Schloss. All rights reserved.
*
*/

#include "command.hpp"
#include "clustercommand.h"
#include "collectcommand.h"
#include "collectsharedcommand.h"
#include "getgroupcommand.h"
#include "getlabelcommand.h"
#include "rarefactcommand.h"
#include "summarycommand.h"
#include "summarysharedcommand.h"
#include "rarefactsharedcommand.h"
#include "quitcommand.h"
#include "helpcommand.h"
#include "commandfactory.hpp"
#include "deconvolutecommand.h"
#include "parsimonycommand.h"
#include "unifracunweightedcommand.h"
#include "unifracweightedcommand.h"
#include "libshuffcommand.h"
#include "heatmapcommand.h"
#include "heatmapsimcommand.h"
#include "filterseqscommand.h"
#include "venncommand.h"
#include "nocommands.h"
#include "binsequencecommand.h"
#include "getoturepcommand.h"
#include "treegroupscommand.h"
#include "distancecommand.h"
#include "aligncommand.h"
#include "matrixoutputcommand.h"
#include "getsabundcommand.h"
#include "getrabundcommand.h"
#include "seqsummarycommand.h"
#include "screenseqscommand.h"
#include "reversecommand.h"
#include "trimseqscommand.h"
#include "mergefilecommand.h"
#include "listseqscommand.h"
#include "getseqscommand.h"
#include "removeseqscommand.h"
#include "systemcommand.h"
#include "secondarystructurecommand.h"
#include "getsharedotucommand.h"
#include "getlistcountcommand.h"
#include "hclustercommand.h"
#include "classifyseqscommand.h"
#include "phylotypecommand.h"
#include "mgclustercommand.h"
#include "preclustercommand.h"
#include "pcoacommand.h"
#include "otuhierarchycommand.h"
#include "setdircommand.h"
#include "parselistscommand.h"
#include "chimeraccodecommand.h"
#include "chimeracheckcommand.h"
#include "chimeraslayercommand.h"
#include "chimerapintailcommand.h"
#include "chimerabellerophoncommand.h"
#include "chimerauchimecommand.h"
#include "setlogfilecommand.h"
#include "phylodiversitycommand.h"
#include "makegroupcommand.h"
#include "chopseqscommand.h"
#include "clearcutcommand.h"
#include "catchallcommand.h"
#include "splitabundcommand.h"
#include "clustersplitcommand.h"
#include "classifyotucommand.h"
#include "degapseqscommand.h"
#include "getrelabundcommand.h"
#include "sensspeccommand.h"
#include "sffinfocommand.h"
#include "seqerrorcommand.h"
#include "normalizesharedcommand.h"
#include "metastatscommand.h"
#include "splitgroupscommand.h"
#include "clusterfragmentscommand.h"
#include "getlineagecommand.h"
#include "removelineagecommand.h"
#include "parsefastaqcommand.h"
#include "pipelinepdscommand.h"
#include "deuniqueseqscommand.h"
#include "pairwiseseqscommand.h"
#include "clusterdoturcommand.h"
#include "subsamplecommand.h"
#include "removegroupscommand.h"
#include "getgroupscommand.h"
#include "getotuscommand.h"
#include "removeotuscommand.h"
#include "indicatorcommand.h"
#include "consensusseqscommand.h"
#include "trimflowscommand.h"
#include "corraxescommand.h"
#include "shhhercommand.h"
#include "pcacommand.h"
#include "nmdscommand.h"
#include "removerarecommand.h"
#include "mergegroupscommand.h"
#include "amovacommand.h"
#include "homovacommand.h"
#include "mantelcommand.h"
#include "makefastqcommand.h"
#include "anosimcommand.h"
#include "getcurrentcommand.h"
#include "setcurrentcommand.h"
#include "sharedcommand.h"
#include "getcommandinfocommand.h"
#include "deuniquetreecommand.h"
#include "countseqscommand.h"
#include "countgroupscommand.h"
#include "clearmemorycommand.h"
#include "summarytaxcommand.h"
#include "chimeraperseuscommand.h"
#include "shhhseqscommand.h"
#include "summaryqualcommand.h"
#include "otuassociationcommand.h"
#include "sortseqscommand.h"
#include "classifytreecommand.h"
#include "cooccurrencecommand.h"
#include "pcrseqscommand.h"
#include "createdatabasecommand.h"
#include "makebiomcommand.h"
#include "getcoremicrobiomecommand.h"
#include "listotulabelscommand.h"
#include "getotulabelscommand.h"
#include "removeotulabelscommand.h"
#include "makecontigscommand.h"
#include "loadlogfilecommand.h"
#include "sffmultiplecommand.h"
#include "classifysvmsharedcommand.h"
#include "classifyrfsharedcommand.h"
#include "filtersharedcommand.h"
#include "primerdesigncommand.h"
#include "getdistscommand.h"
#include "removedistscommand.h"
#include "mergetaxsummarycommand.h"
#include "getmetacommunitycommand.h"
#include "sparcccommand.h"
#include "makelookupcommand.h"
#include "renameseqscommand.h"
#include "makelefsecommand.h"
#include "lefsecommand.h"
#include "kruskalwalliscommand.h"
#include "sracommand.h"
#include "mergesfffilecommand.h"
#include "getmimarkspackagecommand.h"
#include "mimarksattributescommand.h"
#include "setseedcommand.h"
#include "makefilecommand.h"

//needed for testing project
//CommandFactory* CommandFactory::_uniqueInstance;

/*******************************************************/

/******************************************************/
CommandFactory* CommandFactory::getInstance() {
	if (_uniqueInstance == 0) {
		_uniqueInstance = new CommandFactory();
	}
	return _uniqueInstance;
}
/***********************************************************/

/***********************************************************/
//note: This class is resposible for knowing which commands are mpiEnabled,
//If a command is not enabled only process 0 will execute the command.
//This avoids redundant outputs on pieces of code we have not paralellized.
//If you add mpi code to a existing command you need to modify the list below or the code will hang on MPI blocking commands like FIle_open.
//example:  commands["dist.seqs"] = "MPIEnabled";

CommandFactory::CommandFactory() {
	string s = "";
	m = MothurOut::getInstance();

	command = new NoCommand(s);
	shellcommand = new NoCommand(s);
	pipecommand = new NoCommand(s);

	outputDir = ""; inputDir = "";
	logFileName = "";
	append = false;

	//initialize list of valid commands
	commands["make.shared"] = "make.shared";
	commands["bin.seqs"] = "bin.seqs";
	commands["get.oturep"] = "get.oturep";
	commands["cluster"] = "cluster";
	commands["unique.seqs"] = "unique.seqs";
	commands["dist.shared"] = "dist.shared";
	commands["collect.single"] = "collect.single";
	commands["collect.shared"] = "collect.shared";
	commands["rarefaction.single"] = "rarefaction.single";
	commands["rarefaction.shared"] = "rarefaction.shared";
	commands["summary.single"] = "summary.single";
	commands["summary.shared"] = "summary.shared";
	commands["parsimony"] = "parsimony";
	commands["unifrac.weighted"] = "unifrac.weighted";
	commands["unifrac.unweighted"] = "unifrac.unweighted";
	commands["libshuff"] = "libshuff";
	commands["tree.shared"] = "tree.shared";
	commands["heatmap.bin"] = "heatmap.bin";
	commands["heatmap.sim"] = "heatmap.sim";
	commands["venn"] = "venn";
	commands["get.group"] = "get.group";
	commands["get.label"] = "get.label";
	commands["get.sabund"] = "get.sabund";
	commands["get.rabund"] = "get.rabund";
	commands["help"] = "help";
	commands["reverse.seqs"] = "reverse.seqs";
	commands["trim.seqs"] = "trim.seqs";
	commands["trim.flows"] = "trim.flows";
	commands["list.seqs"] = "list.seqs";
	commands["get.seqs"] = "get.seqs";
	commands["remove.seqs"] = "remove.seqs";
	commands["system"] = "system";
	commands["align.check"] = "align.check";
	commands["get.sharedseqs"] = "get.sharedseqs";
	commands["get.otulist"] = "get.otulist";
	commands["hcluster"] = "hcluster";
	commands["phylotype"] = "phylotype";
	commands["mgcluster"] = "mgcluster";
	commands["pre.cluster"] = "pre.cluster";
	commands["pcoa"] = "pcoa";
	commands["otu.hierarchy"] = "otu.hierarchy";
	commands["set.dir"] = "MPIEnabled";
	commands["merge.files"] = "merge.files";
	commands["parse.list"] = "parse.list";
	commands["set.logfile"] = "set.logfile";
	commands["phylo.diversity"] = "phylo.diversity";
	commands["make.group"] = "make.group";
	commands["chop.seqs"] = "chop.seqs";
	commands["clearcut"] = "clearcut";
	commands["catchall"] = "catchall";
	commands["split.abund"] = "split.abund";
	commands["classify.otu"] = "classify.otu";
	commands["degap.seqs"] = "degap.seqs";
	commands["get.relabund"] = "get.relabund";
	commands["sffinfo"] = "sffinfo";
	commands["normalize.shared"] = "normalize.shared";
	commands["metastats"] = "metastats";
	commands["split.groups"] = "split.groups";
	commands["cluster.fragments"] = "cluster.fragments";
	commands["get.lineage"] = "get.lineage";
	commands["remove.lineage"] = "remove.lineage";
	commands["fastq.info"] = "fastq.info";
	commands["deunique.seqs"] = "deunique.seqs";
	commands["cluster.classic"] = "cluster.classic";
	commands["sub.sample"] = "sub.sample";
	commands["remove.groups"] = "remove.groups";
	commands["get.groups"] = "get.groups";
	commands["get.otus"] = "get.otus";
	commands["remove.otus"] = "remove.otus";
	commands["indicator"] = "indicator";
	commands["consensus.seqs"] = "consensus.seqs";
	commands["corr.axes"] = "corr.axes";
	commands["pca"] = "pca";
	commands["nmds"] = "nmds";
	commands["remove.rare"] = "remove.rare";
	commands["amova"] = "amova";
	commands["homova"] = "homova";
	commands["mantel"] = "mantel";
	commands["anosim"] = "anosim";
	commands["make.fastq"] = "make.fastq";
	commands["merge.groups"] = "merge.groups";
	commands["get.current"] = "MPIEnabled";
	commands["set.current"] = "MPIEnabled";
	commands["get.commandinfo"] = "get.commandinfo";
	commands["deunique.tree"] = "deunique.tree";
	commands["count.seqs"] = "count.seqs";
	commands["count.groups"] = "count.groups";
	commands["clear.memory"] = "clear.memory";
	commands["pairwise.seqs"] = "MPIEnabled";
	commands["pipeline.pds"] = "MPIEnabled";
	commands["classify.seqs"] = "MPIEnabled";
	commands["dist.seqs"] = "MPIEnabled";
	commands["filter.seqs"] = "MPIEnabled";
	commands["align.seqs"] = "MPIEnabled";
	commands["chimera.ccode"] = "MPIEnabled";
	commands["chimera.check"] = "MPIEnabled";
	commands["chimera.slayer"] = "MPIEnabled";
	commands["chimera.uchime"] = "chimera.uchime";
	commands["chimera.perseus"] = "chimera.perseus";
	commands["chimera.pintail"] = "MPIEnabled";
	commands["chimera.bellerophon"] = "MPIEnabled";
	commands["screen.seqs"] = "MPIEnabled";
	commands["summary.seqs"] = "summary.seqs";
	commands["cluster.split"] = "MPIEnabled";
	commands["shhh.flows"] = "MPIEnabled";
	commands["sens.spec"] = "sens.spec";
	commands["seq.error"] = "seq.error";
	commands["summary.tax"] = "summary.tax";
	commands["summary.qual"] = "summary.qual";
	commands["shhh.seqs"] = "shhh.seqs";
	commands["otu.association"] = "otu.association";
	commands["sort.seqs"] = "sort.seqs";
	commands["classify.tree"] = "classify.tree";
	commands["cooccurrence"] = "cooccurrence";
	commands["pcr.seqs"] = "pcr.seqs";
	commands["create.database"] = "create.database";
	commands["make.biom"] = "make.biom";
	commands["get.coremicrobiome"] = "get.coremicrobiome";
	commands["list.otulabels"] = "list.otulabels";
	commands["get.otulabels"] = "get.otulabels";
	commands["remove.otulabels"] = "remove.otulabels";
	commands["make.contigs"] = "make.contigs";
	commands["load.logfile"] = "load.logfile";
	commands["make.table"] = "make.table";
	commands["sff.multiple"] = "sff.multiple";
	commands["quit"] = "MPIEnabled";
	commands["classify.rf"] = "classify.rf";
	commands["classify.svm"] = "classify.svm";
	commands["filter.shared"] = "filter.shared";
	commands["primer.design"] = "primer.design";
	commands["get.dists"] = "get.dists";
	commands["remove.dists"] = "remove.dists";
	commands["merge.taxsummary"] = "merge.taxsummary";
	commands["get.communitytype"] = "get.communitytype";
	commands["sparcc"] = "sparcc";
	commands["make.lookup"] = "make.lookup";
	commands["rename.seqs"] = "rename.seqs";
	commands["make.lefse"] = "make.lefse";
	commands["lefse"] = "lefse";
	commands["kruskal.wallis"] = "kruskal.wallis";
	commands["make.sra"] = "make.sra";
	commands["merge.sfffiles"] = "merge.sfffiles";
	commands["get.mimarkspackage"] = "get.mimarkspackage";
	commands["mimarks.attributes"] = "mimarks.attributes";
	commands["make.file"] = "make.file";
	commands["set.seed"] = "set.seed";


}
/***********************************************************/

/***********************************************************/
bool CommandFactory::MPIEnabled(string commandName) {
	bool mpi = false;
	it = commands.find(commandName);
	if (it != commands.end()) {
		if (it->second == "MPIEnabled") { return true; }
	}
	return mpi;
}
/***********************************************************/

/***********************************************************/
CommandFactory::~CommandFactory() {
	_uniqueInstance = 0;
	delete command;
	delete shellcommand;
	delete pipecommand;
}
/***********************************************************/

/***********************************************************/
int CommandFactory::checkForRedirects(string optionString) {
	try {

		int pos = optionString.find("outputdir");
		if (pos != string::npos) { //user has set outputdir in command option string
			string outputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { outputOption += optionString[i]; }
			}
			if (outputOption[0] == '=') { outputOption = outputOption.substr(1); }
			if (m->mkDir(outputOption)) {
				setOutputDirectory(outputOption);
				m->mothurOut("Setting output directory to: " + outputOption); m->mothurOutEndLine();
			}
		}

		pos = optionString.find("inputdir");
		if (pos != string::npos) { //user has set inputdir in command option string
			string intputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { intputOption += optionString[i]; }
			}
			if (intputOption[0] == '=') { intputOption = intputOption.substr(1); }
			if (m->dirCheck(intputOption)) {
				setInputDirectory(intputOption);
				m->mothurOut("Setting input directory to: " + intputOption); m->mothurOutEndLine();
			}
		}

		pos = optionString.find("seed");
		if (pos != string::npos) { //user has set inputdir in command option string
			string intputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { intputOption += optionString[i]; }
			}
			if (intputOption[0] == '=') { intputOption = intputOption.substr(1); }
			bool seed = false; int random;
			if (intputOption == "clear") {
				random = time(NULL);
				seed = true;
			}
			else {
				if (m->isInteger(intputOption)) { m->mothurConvert(intputOption, random); seed = true; }
				else { m->mothurOut("[ERROR]: Seed must be an integer."); m->mothurOutEndLine(); seed = false; }
			}

			if (seed) {
				srand(random);
				m->mothurOut("Setting random seed to " + toString(random) + ".\n\n");
			}

		}


		return 0;
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************/

/***********************************************************/
//This function calls the appropriate command fucntions based on user input.
Command* CommandFactory::getCommand(string commandName, string optionString) {
	try {

		delete command;   //delete the old command

		checkForRedirects(optionString);

		//user has opted to redirect output from dir where input files are located to some other place
		if (outputDir != "") {
			if (optionString != "") { optionString += ", outputdir=" + outputDir; }
			else { optionString += "outputdir=" + outputDir; }
		}

		//user has opted to redirect input from dir where mothur.exe is located to some other place
		if (inputDir != "") {
			if (optionString != "") { optionString += ", inputdir=" + inputDir; }
			else { optionString += "inputdir=" + inputDir; }
		}

		if (commandName == "cluster") { return command = new ClusterCommand(optionString); }
		if (commandName == "unique.seqs") { return command = new DeconvoluteCommand(optionString); }
		if (commandName == "parsimony") { return command = new ParsimonyCommand(optionString); }
		if (commandName == "help") { return command = new HelpCommand(optionString); }
		if (commandName == "quit") { return command = new QuitCommand(optionString); }
		if (commandName == "collect.single") { return command = new CollectCommand(optionString); }
		if (commandName == "collect.shared") { return command = new CollectSharedCommand(optionString); }
		if (commandName == "rarefaction.single") { return command = new RareFactCommand(optionString); }
		if (commandName == "rarefaction.shared") { return command = new RareFactSharedCommand(optionString); }
		if (commandName == "summary.single") { return command = new SummaryCommand(optionString); }
		if (commandName == "summary.shared") { return command = new SummarySharedCommand(optionString); }
		if (commandName == "unifrac.weighted") { return command = new UnifracWeightedCommand(optionString); }
		if (commandName == "unifrac.unweighted") { return command = new UnifracUnweightedCommand(optionString); }
		if (commandName == "get.group") { return command = new GetgroupCommand(optionString); }
		if (commandName == "get.label") { return command = new GetlabelCommand(optionString); }
		if (commandName == "get.sabund") { return command = new GetSAbundCommand(optionString); }
		if (commandName == "get.rabund") { return command = new GetRAbundCommand(optionString); }
		if (commandName == "libshuff") { return command = new LibShuffCommand(optionString); }
		if (commandName == "heatmap.bin") { return command = new HeatMapCommand(optionString); }
		if (commandName == "heatmap.sim") { return command = new HeatMapSimCommand(optionString); }
		if (commandName == "filter.seqs") { return command = new FilterSeqsCommand(optionString); }
		if (commandName == "venn") { return command = new VennCommand(optionString); }
		if (commandName == "bin.seqs") { return command = new BinSeqCommand(optionString); }
		if (commandName == "get.oturep") { return command = new GetOTURepCommand(optionString); }
		if (commandName == "tree.shared") { return command = new TreeGroupCommand(optionString); }
		if (commandName == "dist.shared") { return command = new MatrixOutputCommand(optionString); }
		if (commandName == "dist.seqs") { return command = new DistanceCommand(optionString); }
		if (commandName == "align.seqs") { return command = new AlignCommand(optionString); }
		if (commandName == "summary.seqs") { return command = new SeqSummaryCommand(optionString); }
		if (commandName == "screen.seqs") { return command = new ScreenSeqsCommand(optionString); }
		if (commandName == "reverse.seqs") { return command = new ReverseSeqsCommand(optionString); }
		if (commandName == "trim.seqs") { return command = new TrimSeqsCommand(optionString); }
		if (commandName == "trim.flows") { return command = new TrimFlowsCommand(optionString); }
		if (commandName == "shhh.flows") { return command = new ShhherCommand(optionString); }
		if (commandName == "list.seqs") { return command = new ListSeqsCommand(optionString); }
		if (commandName == "get.seqs") { return command = new GetSeqsCommand(optionString); }
		if (commandName == "remove.seqs") { return command = new RemoveSeqsCommand(optionString); }
		if (commandName == "merge.files") { return command = new MergeFileCommand(optionString); }
		if (commandName == "system") { return command = new SystemCommand(optionString); }
		if (commandName == "align.check") { return command = new AlignCheckCommand(optionString); }
		if (commandName == "get.sharedseqs") { return command = new GetSharedOTUCommand(optionString); }
		if (commandName == "get.otulist") { return command = new GetListCountCommand(optionString); }
		if (commandName == "hcluster") { return command = new HClusterCommand(optionString); }
		if (commandName == "classify.seqs") { return command = new ClassifySeqsCommand(optionString); }
		if (commandName == "chimera.ccode") { return command = new ChimeraCcodeCommand(optionString); }
		if (commandName == "chimera.check") { return command = new ChimeraCheckCommand(optionString); }
		if (commandName == "chimera.slayer") { return command = new ChimeraSlayerCommand(optionString); }
		if (commandName == "chimera.uchime") { return command = new ChimeraUchimeCommand(optionString); }
		if (commandName == "chimera.pintail") { return command = new ChimeraPintailCommand(optionString); }
		if (commandName == "chimera.bellerophon") { return command = new ChimeraBellerophonCommand(optionString); }
		if (commandName == "phylotype") { return command = new PhylotypeCommand(optionString); }
		if (commandName == "mgcluster") { return command = new MGClusterCommand(optionString); }
		if (commandName == "pre.cluster") { return command = new PreClusterCommand(optionString); }
		if (commandName == "pcoa") { return command = new PCOACommand(optionString); }
		if (commandName == "pca") { return command = new PCACommand(optionString); }
		if (commandName == "nmds") { return command = new NMDSCommand(optionString); }
		if (commandName == "otu.hierarchy") { return command = new OtuHierarchyCommand(optionString); }
		if (commandName == "set.dir") { return command = new SetDirectoryCommand(optionString); }
		if (commandName == "set.logfile") { return command = new SetLogFileCommand(optionString); }
		if (commandName == "parse.list") { return command = new ParseListCommand(optionString); }
		if (commandName == "phylo.diversity") { return command = new PhyloDiversityCommand(optionString); }
		if (commandName == "make.group") { return command = new MakeGroupCommand(optionString); }
		if (commandName == "chop.seqs") { return command = new ChopSeqsCommand(optionString); }
		if (commandName == "clearcut") { return command = new ClearcutCommand(optionString); }
		if (commandName == "catchall") { return command = new CatchAllCommand(optionString); }
		if (commandName == "split.abund") { return command = new SplitAbundCommand(optionString); }
		if (commandName == "cluster.split") { return command = new ClusterSplitCommand(optionString); }
		if (commandName == "classify.otu") { return command = new ClassifyOtuCommand(optionString); }
		if (commandName == "degap.seqs") { return command = new DegapSeqsCommand(optionString); }
		if (commandName == "get.relabund") { return command = new GetRelAbundCommand(optionString); }
		if (commandName == "sens.spec") { return command = new SensSpecCommand(optionString); }
		if (commandName == "seq.error") { return command = new SeqErrorCommand(optionString); }
		if (commandName == "sffinfo") { return command = new SffInfoCommand(optionString); }
		if (commandName == "normalize.shared") { return command = new NormalizeSharedCommand(optionString); }
		if (commandName == "metastats") { return command = new MetaStatsCommand(optionString); }
		if (commandName == "split.groups") { return command = new SplitGroupCommand(optionString); }
		if (commandName == "cluster.fragments") { return command = new ClusterFragmentsCommand(optionString); }
		if (commandName == "get.lineage") { return command = new GetLineageCommand(optionString); }
		if (commandName == "remove.lineage") { return command = new RemoveLineageCommand(optionString); }
		if (commandName == "get.groups") { return command = new GetGroupsCommand(optionString); }
		if (commandName == "remove.groups") { return command = new RemoveGroupsCommand(optionString); }
		if (commandName == "get.otus") { return command = new GetOtusCommand(optionString); }
		if (commandName == "remove.otus") { return command = new RemoveOtusCommand(optionString); }
		if (commandName == "fastq.info") { return command = new ParseFastaQCommand(optionString); }
		if (commandName == "pipeline.pds") { return command = new PipelineCommand(optionString); }
		if (commandName == "deunique.seqs") { return command = new DeUniqueSeqsCommand(optionString); }
		if (commandName == "pairwise.seqs") { return command = new PairwiseSeqsCommand(optionString); }
		if (commandName == "cluster.classic") { return command = new ClusterDoturCommand(optionString); }
		if (commandName == "sub.sample") { return command = new SubSampleCommand(optionString); }
		if (commandName == "indicator") { return command = new IndicatorCommand(optionString); }
		if (commandName == "consensus.seqs") { return command = new ConsensusSeqsCommand(optionString); }
		if (commandName == "corr.axes") { return command = new CorrAxesCommand(optionString); }
		if (commandName == "remove.rare") { return command = new RemoveRareCommand(optionString); }
		if (commandName == "merge.groups") { return command = new MergeGroupsCommand(optionString); }
		if (commandName == "amova") { return command = new AmovaCommand(optionString); }
		if (commandName == "homova") { return command = new HomovaCommand(optionString); }
		if (commandName == "mantel") { return command = new MantelCommand(optionString); }
		if (commandName == "make.fastq") { return command = new MakeFastQCommand(optionString); }
		if (commandName == "get.current") { return command = new GetCurrentCommand(optionString); }
		if (commandName == "set.current") { return command = new SetCurrentCommand(optionString); }
		if (commandName == "anosim") { return command = new AnosimCommand(optionString); }
		if (commandName == "make.shared") { return command = new SharedCommand(optionString); }
		if (commandName == "get.commandinfo") { return command = new GetCommandInfoCommand(optionString); }
		if (commandName == "deunique.tree") { return command = new DeuniqueTreeCommand(optionString); }
		if ((commandName == "count.seqs") || (commandName == "make.table")) { return command = new CountSeqsCommand(optionString); }
		if (commandName == "count.groups") { return command = new CountGroupsCommand(optionString); }
		if (commandName == "clear.memory") { return command = new ClearMemoryCommand(optionString); }
		if (commandName == "summary.tax") { return command = new SummaryTaxCommand(optionString); }
		if (commandName == "summary.qual") { return command = new SummaryQualCommand(optionString); }
		if (commandName == "chimera.perseus") { return command = new ChimeraPerseusCommand(optionString); }
		if (commandName == "shhh.seqs") { return command = new ShhhSeqsCommand(optionString); }
		if (commandName == "otu.association") { return command = new OTUAssociationCommand(optionString); }
		if (commandName == "sort.seqs") { return command = new SortSeqsCommand(optionString); }
		if (commandName == "classify.tree") { return command = new ClassifyTreeCommand(optionString); }
		if (commandName == "cooccurrence") { return command = new CooccurrenceCommand(optionString); }
		if (commandName == "pcr.seqs") { return command = new PcrSeqsCommand(optionString); }
		if (commandName == "create.database") { return command = new CreateDatabaseCommand(optionString); }
		if (commandName == "make.biom") { return command = new MakeBiomCommand(optionString); }
		if (commandName == "get.coremicrobiome") { return command = new GetCoreMicroBiomeCommand(optionString); }
		if (commandName == "list.otulabels") { return command = new ListOtuLabelsCommand(optionString); }
		if (commandName == "get.otulabels") { return command = new GetOtuLabelsCommand(optionString); }
		if (commandName == "remove.otulabels") { return command = new RemoveOtuLabelsCommand(optionString); }
		if (commandName == "make.contigs") { return command = new MakeContigsCommand(optionString); }
		if (commandName == "load.logfile") { return command = new LoadLogfileCommand(optionString); }
		if (commandName == "sff.multiple") { return command = new SffMultipleCommand(optionString); }
		if (commandName == "classify.svm") { return command = new ClassifySvmSharedCommand(optionString); }
		if (commandName == "classify.rf") { return command = new ClassifyRFSharedCommand(optionString); }
		if (commandName == "filter.shared") { return command = new FilterSharedCommand(optionString); }
		if (commandName == "primer.design") { return command = new PrimerDesignCommand(optionString); }
		if (commandName == "get.dists") { return command = new GetDistsCommand(optionString); }
		if (commandName == "remove.dists") { return command = new RemoveDistsCommand(optionString); }
		if (commandName == "merge.taxsummary") { return command = new MergeTaxSummaryCommand(optionString); }
		if (commandName == "get.communitytype") { return command = new GetMetaCommunityCommand(optionString); }
		if (commandName == "sparcc") { return command = new SparccCommand(optionString); }
		if (commandName == "make.lookup") { return command = new MakeLookupCommand(optionString); }
		if (commandName == "rename.seqs") { return command = new RenameSeqsCommand(optionString); }
		if (commandName == "make.lefse") { return command = new MakeLefseCommand(optionString); }
		if (commandName == "lefse") { return command = new LefseCommand(optionString); }
		if (commandName == "kruskal.wallis") { return command = new KruskalWallisCommand(optionString); }
		if (commandName == "make.sra") { return command = new SRACommand(optionString); }
		if (commandName == "merge.sfffiles") { return command = new MergeSfffilesCommand(optionString); }
		if (commandName == "get.mimarkspackage") { return command = new GetMIMarksPackageCommand(optionString); }
		if (commandName == "mimarks.attributes") { return command = new MimarksAttributesCommand(optionString); }
		if (commandName == "set.seed") { return command = new SetSeedCommand(optionString); }
		if (commandName == "make.file") { return command = new MakeFileCommand(optionString); }
		return command = new NoCommand(optionString);
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************/

/***********************************************************/
//This function calls the appropriate command fucntions based on user input.
Command* CommandFactory::getCommand(string commandName, string optionString, string mode) {
	try {
		delete pipecommand;   //delete the old command

		checkForRedirects(optionString);

		//user has opted to redirect output from dir where input files are located to some other place
		if (outputDir != "") {
			if (optionString != "") { optionString += ", outputdir=" + outputDir; }
			else { optionString += "outputdir=" + outputDir; }
		}

		//user has opted to redirect input from dir where mothur.exe is located to some other place
		if (inputDir != "") {
			if (optionString != "") { optionString += ", inputdir=" + inputDir; }
			else { optionString += "inputdir=" + inputDir; }
		}

		if (commandName == "cluster") { return pipecommand = new ClusterCommand(optionString); }
		if (commandName == "unique.seqs") { return pipecommand = new DeconvoluteCommand(optionString); }
		if (commandName == "parsimony") { return pipecommand = new ParsimonyCommand(optionString); }
		if (commandName == "help") { return pipecommand = new HelpCommand(optionString); }
		if (commandName == "quit") { return pipecommand = new QuitCommand(optionString); }
		if (commandName == "collect.single") { return pipecommand = new CollectCommand(optionString); }
		if (commandName == "collect.shared") { return pipecommand = new CollectSharedCommand(optionString); }
		if (commandName == "rarefaction.single") { return pipecommand = new RareFactCommand(optionString); }
		if (commandName == "rarefaction.shared") { return pipecommand = new RareFactSharedCommand(optionString); }
		if (commandName == "summary.single") { return pipecommand = new SummaryCommand(optionString); }
		if (commandName == "summary.shared") { return pipecommand = new SummarySharedCommand(optionString); }
		if (commandName == "unifrac.weighted") { return pipecommand = new UnifracWeightedCommand(optionString); }
		if (commandName == "unifrac.unweighted") { return pipecommand = new UnifracUnweightedCommand(optionString); }
		if (commandName == "get.group") { return pipecommand = new GetgroupCommand(optionString); }
		if (commandName == "get.label") { return pipecommand = new GetlabelCommand(optionString); }
		if (commandName == "get.sabund") { return pipecommand = new GetSAbundCommand(optionString); }
		if (commandName == "get.rabund") { return pipecommand = new GetRAbundCommand(optionString); }
		if (commandName == "libshuff") { return pipecommand = new LibShuffCommand(optionString); }
		if (commandName == "heatmap.bin") { return pipecommand = new HeatMapCommand(optionString); }
		if (commandName == "heatmap.sim") { return pipecommand = new HeatMapSimCommand(optionString); }
		if (commandName == "filter.seqs") { return pipecommand = new FilterSeqsCommand(optionString); }
		if (commandName == "venn") { return pipecommand = new VennCommand(optionString); }
		if (commandName == "bin.seqs") { return pipecommand = new BinSeqCommand(optionString); }
		if (commandName == "get.oturep") { return pipecommand = new GetOTURepCommand(optionString); }
		if (commandName == "tree.shared") { return pipecommand = new TreeGroupCommand(optionString); }
		if (commandName == "dist.shared") { return pipecommand = new MatrixOutputCommand(optionString); }
		if (commandName == "dist.seqs") { return pipecommand = new DistanceCommand(optionString); }
		if (commandName == "align.seqs") { return pipecommand = new AlignCommand(optionString); }
		if (commandName == "summary.seqs") { return pipecommand = new SeqSummaryCommand(optionString); }
		if (commandName == "screen.seqs") { return pipecommand = new ScreenSeqsCommand(optionString); }
		if (commandName == "reverse.seqs") { return pipecommand = new ReverseSeqsCommand(optionString); }
		if (commandName == "trim.seqs") { return pipecommand = new TrimSeqsCommand(optionString); }
		if (commandName == "trim.flows") { return pipecommand = new TrimFlowsCommand(optionString); }
		if (commandName == "shhh.flows") { return pipecommand = new ShhherCommand(optionString); }
		if (commandName == "list.seqs") { return pipecommand = new ListSeqsCommand(optionString); }
		if (commandName == "get.seqs") { return pipecommand = new GetSeqsCommand(optionString); }
		if (commandName == "remove.seqs") { return pipecommand = new RemoveSeqsCommand(optionString); }
		if (commandName == "merge.files") { return pipecommand = new MergeFileCommand(optionString); }
		if (commandName == "system") { return pipecommand = new SystemCommand(optionString); }
		if (commandName == "align.check") { return pipecommand = new AlignCheckCommand(optionString); }
		if (commandName == "get.sharedseqs") { return pipecommand = new GetSharedOTUCommand(optionString); }
		if (commandName == "get.otulist") { return pipecommand = new GetListCountCommand(optionString); }
		if (commandName == "hcluster") { return pipecommand = new HClusterCommand(optionString); }
		if (commandName == "classify.seqs") { return pipecommand = new ClassifySeqsCommand(optionString); }
		if (commandName == "chimera.ccode") { return pipecommand = new ChimeraCcodeCommand(optionString); }
		if (commandName == "chimera.check") { return pipecommand = new ChimeraCheckCommand(optionString); }
		if (commandName == "chimera.uchime") { return pipecommand = new ChimeraUchimeCommand(optionString); }
		if (commandName == "chimera.slayer") { return pipecommand = new ChimeraSlayerCommand(optionString); }
		if (commandName == "chimera.pintail") { return pipecommand = new ChimeraPintailCommand(optionString); }
		if (commandName == "chimera.bellerophon") { return pipecommand = new ChimeraBellerophonCommand(optionString); }
		if (commandName == "phylotype") { return pipecommand = new PhylotypeCommand(optionString); }
		if (commandName == "mgcluster") { return pipecommand = new MGClusterCommand(optionString); }
		if (commandName == "pre.cluster") { return pipecommand = new PreClusterCommand(optionString); }
		if (commandName == "pcoa") { return pipecommand = new PCOACommand(optionString); }
		if (commandName == "pca") { return pipecommand = new PCACommand(optionString); }
		if (commandName == "nmds") { return pipecommand = new NMDSCommand(optionString); }
		if (commandName == "otu.hierarchy") { return pipecommand = new OtuHierarchyCommand(optionString); }
		if (commandName == "set.dir") { return pipecommand = new SetDirectoryCommand(optionString); }
		if (commandName == "set.logfile") { return pipecommand = new SetLogFileCommand(optionString); }
		if (commandName == "parse.list") { return pipecommand = new ParseListCommand(optionString); }
		if (commandName == "phylo.diversity") { return pipecommand = new PhyloDiversityCommand(optionString); }
		if (commandName == "make.group") { return pipecommand = new MakeGroupCommand(optionString); }
		if (commandName == "chop.seqs") { return pipecommand = new ChopSeqsCommand(optionString); }
		if (commandName == "clearcut") { return pipecommand = new ClearcutCommand(optionString); }
		if (commandName == "catchall") { return pipecommand = new CatchAllCommand(optionString); }
		if (commandName == "split.abund") { return pipecommand = new SplitAbundCommand(optionString); }
		if (commandName == "cluster.split") { return pipecommand = new ClusterSplitCommand(optionString); }
		if (commandName == "classify.otu") { return pipecommand = new ClassifyOtuCommand(optionString); }
		if (commandName == "degap.seqs") { return pipecommand = new DegapSeqsCommand(optionString); }
		if (commandName == "get.relabund") { return pipecommand = new GetRelAbundCommand(optionString); }
		if (commandName == "sens.spec") { return pipecommand = new SensSpecCommand(optionString); }
		if (commandName == "seq.error") { return pipecommand = new SeqErrorCommand(optionString); }
		if (commandName == "sffinfo") { return pipecommand = new SffInfoCommand(optionString); }
		if (commandName == "normalize.shared") { return pipecommand = new NormalizeSharedCommand(optionString); }
		if (commandName == "metastats") { return pipecommand = new MetaStatsCommand(optionString); }
		if (commandName == "split.groups") { return pipecommand = new SplitGroupCommand(optionString); }
		if (commandName == "cluster.fragments") { return pipecommand = new ClusterFragmentsCommand(optionString); }
		if (commandName == "get.lineage") { return pipecommand = new GetLineageCommand(optionString); }
		if (commandName == "get.groups") { return pipecommand = new GetGroupsCommand(optionString); }
		if (commandName == "remove.lineage") { return pipecommand = new RemoveLineageCommand(optionString); }
		if (commandName == "remove.groups") { return pipecommand = new RemoveGroupsCommand(optionString); }
		if (commandName == "get.otus") { return pipecommand = new GetOtusCommand(optionString); }
		if (commandName == "remove.otus") { return pipecommand = new RemoveOtusCommand(optionString); }
		if (commandName == "fastq.info") { return pipecommand = new ParseFastaQCommand(optionString); }
		if (commandName == "deunique.seqs") { return pipecommand = new DeUniqueSeqsCommand(optionString); }
		if (commandName == "pairwise.seqs") { return pipecommand = new PairwiseSeqsCommand(optionString); }
		if (commandName == "cluster.classic") { return pipecommand = new ClusterDoturCommand(optionString); }
		if (commandName == "sub.sample") { return pipecommand = new SubSampleCommand(optionString); }
		if (commandName == "indicator") { return pipecommand = new IndicatorCommand(optionString); }
		if (commandName == "consensus.seqs") { return pipecommand = new ConsensusSeqsCommand(optionString); }
		if (commandName == "corr.axes") { return pipecommand = new CorrAxesCommand(optionString); }
		if (commandName == "remove.rare") { return pipecommand = new RemoveRareCommand(optionString); }
		if (commandName == "merge.groups") { return pipecommand = new MergeGroupsCommand(optionString); }
		if (commandName == "amova") { return pipecommand = new AmovaCommand(optionString); }
		if (commandName == "homova") { return pipecommand = new HomovaCommand(optionString); }
		if (commandName == "mantel") { return pipecommand = new MantelCommand(optionString); }
		if (commandName == "anosim") { return pipecommand = new AnosimCommand(optionString); }
		if (commandName == "make.fastq") { return pipecommand = new MakeFastQCommand(optionString); }
		if (commandName == "get.current") { return pipecommand = new GetCurrentCommand(optionString); }
		if (commandName == "set.current") { return pipecommand = new SetCurrentCommand(optionString); }
		if (commandName == "make.shared") { return pipecommand = new SharedCommand(optionString); }
		if (commandName == "get.commandinfo") { return pipecommand = new GetCommandInfoCommand(optionString); }
		if (commandName == "deunique.tree") { return pipecommand = new DeuniqueTreeCommand(optionString); }
		if ((commandName == "count.seqs") || (commandName == "make.table")) { return pipecommand = new CountSeqsCommand(optionString); }
		if (commandName == "count.groups") { return pipecommand = new CountGroupsCommand(optionString); }
		if (commandName == "clear.memory") { return pipecommand = new ClearMemoryCommand(optionString); }
		if (commandName == "summary.tax") { return pipecommand = new SummaryTaxCommand(optionString); }
		if (commandName == "summary.qual") { return pipecommand = new SummaryQualCommand(optionString); }
		if (commandName == "chimera.perseus") { return pipecommand = new ChimeraPerseusCommand(optionString); }
		if (commandName == "shhh.seqs") { return pipecommand = new ShhhSeqsCommand(optionString); }
		if (commandName == "otu.association") { return pipecommand = new OTUAssociationCommand(optionString); }
		if (commandName == "sort.seqs") { return pipecommand = new SortSeqsCommand(optionString); }
		if (commandName == "classify.tree") { return pipecommand = new ClassifyTreeCommand(optionString); }
		if (commandName == "cooccurrence") { return pipecommand = new CooccurrenceCommand(optionString); }
		if (commandName == "pcr.seqs") { return pipecommand = new PcrSeqsCommand(optionString); }
		if (commandName == "create.database") { return pipecommand = new CreateDatabaseCommand(optionString); }
		if (commandName == "make.biom") { return pipecommand = new MakeBiomCommand(optionString); }
		if (commandName == "get.coremicrobiome") { return pipecommand = new GetCoreMicroBiomeCommand(optionString); }
		if (commandName == "list.otulabels") { return pipecommand = new ListOtuLabelsCommand(optionString); }
		if (commandName == "get.otulabels") { return pipecommand = new GetOtuLabelsCommand(optionString); }
		if (commandName == "remove.otulabels") { return pipecommand = new RemoveOtuLabelsCommand(optionString); }
		if (commandName == "make.contigs") { return pipecommand = new MakeContigsCommand(optionString); }
		if (commandName == "load.logfile") { return pipecommand = new LoadLogfileCommand(optionString); }
		if (commandName == "sff.multiple") { return pipecommand = new SffMultipleCommand(optionString); }
		if (commandName == "classify.rf") { return pipecommand = new ClassifyRFSharedCommand(optionString); }
		if (commandName == "filter.shared") { return pipecommand = new FilterSharedCommand(optionString); }
		if (commandName == "primer.design") { return pipecommand = new PrimerDesignCommand(optionString); }
		if (commandName == "get.dists") { return pipecommand = new GetDistsCommand(optionString); }
		if (commandName == "remove.dists") { return pipecommand = new RemoveDistsCommand(optionString); }
		if (commandName == "merge.taxsummary") { return pipecommand = new MergeTaxSummaryCommand(optionString); }
		if (commandName == "get.communitytype") { return pipecommand = new GetMetaCommunityCommand(optionString); }
		if (commandName == "sparcc") { return pipecommand = new SparccCommand(optionString); }
		if (commandName == "make.lookup") { return pipecommand = new MakeLookupCommand(optionString); }
		if (commandName == "rename.seqs") { return pipecommand = new RenameSeqsCommand(optionString); }
		if (commandName == "make.lefse") { return pipecommand = new MakeLefseCommand(optionString); }
		if (commandName == "lefse") { return pipecommand = new LefseCommand(optionString); }
		if (commandName == "kruskal.wallis") { return pipecommand = new KruskalWallisCommand(optionString); }
		if (commandName == "make.sra") { return pipecommand = new SRACommand(optionString); }
		if (commandName == "merge.sfffiles") { return pipecommand = new MergeSfffilesCommand(optionString); }
		if (commandName == "classify.svm") { return pipecommand = new ClassifySvmSharedCommand(optionString); }
		if (commandName == "get.mimarkspackage") { return pipecommand = new GetMIMarksPackageCommand(optionString); }
		if (commandName == "mimarks.attributes") { return pipecommand = new MimarksAttributesCommand(optionString); }
		if (commandName == "set.seed") { return pipecommand = new SetSeedCommand(optionString); }
		if (commandName == "make.file") { return pipecommand = new MakeFileCommand(optionString); }
		return pipecommand = new NoCommand(optionString);

	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************/

/***********************************************************/
//This function calls the appropriate command fucntions based on user input, this is used by the pipeline command to check a users piepline for errors before running
Command* CommandFactory::getCommand(string commandName) {
	try {
		delete shellcommand;   //delete the old command

		if (commandName == "cluster") { return shellcommand = new ClusterCommand(); }
		if (commandName == "unique.seqs") { return shellcommand = new DeconvoluteCommand(); }
		if (commandName == "parsimony") { return shellcommand = new ParsimonyCommand(); }
		if (commandName == "help") { return shellcommand = new HelpCommand(); }
		if (commandName == "quit") { return shellcommand = new QuitCommand(); }
		if (commandName == "collect.single") { return shellcommand = new CollectCommand(); }
		if (commandName == "collect.shared") { return shellcommand = new CollectSharedCommand(); }
		if (commandName == "rarefaction.single") { return shellcommand = new RareFactCommand(); }
		if (commandName == "rarefaction.shared") { return shellcommand = new RareFactSharedCommand(); }
		if (commandName == "summary.single") { return shellcommand = new SummaryCommand(); }
		if (commandName == "summary.shared") { return shellcommand = new SummarySharedCommand(); }
		if (commandName == "unifrac.weighted") { return shellcommand = new UnifracWeightedCommand(); }
		if (commandName == "unifrac.unweighted") { return shellcommand = new UnifracUnweightedCommand(); }
		if (commandName == "get.group") { return shellcommand = new GetgroupCommand(); }
		if (commandName == "get.label") { return shellcommand = new GetlabelCommand(); }
		if (commandName == "get.sabund") { return shellcommand = new GetSAbundCommand(); }
		if (commandName == "get.rabund") { return shellcommand = new GetRAbundCommand(); }
		if (commandName == "libshuff") { return shellcommand = new LibShuffCommand(); }
		if (commandName == "heatmap.bin") { return shellcommand = new HeatMapCommand(); }
		if (commandName == "heatmap.sim") { return shellcommand = new HeatMapSimCommand(); }
		if (commandName == "filter.seqs") { return shellcommand = new FilterSeqsCommand(); }
		if (commandName == "venn") { return shellcommand = new VennCommand(); }
		if (commandName == "bin.seqs") { return shellcommand = new BinSeqCommand(); }
		if (commandName == "get.oturep") { return shellcommand = new GetOTURepCommand(); }
		if (commandName == "tree.shared") { return shellcommand = new TreeGroupCommand(); }
		if (commandName == "dist.shared") { return shellcommand = new MatrixOutputCommand(); }
		if (commandName == "dist.seqs") { return shellcommand = new DistanceCommand(); }
		if (commandName == "align.seqs") { return shellcommand = new AlignCommand(); }
		if (commandName == "summary.seqs") { return shellcommand = new SeqSummaryCommand(); }
		if (commandName == "screen.seqs") { return shellcommand = new ScreenSeqsCommand(); }
		if (commandName == "reverse.seqs") { return shellcommand = new ReverseSeqsCommand(); }
		if (commandName == "trim.seqs") { return shellcommand = new TrimSeqsCommand(); }
		if (commandName == "trim.flows") { return shellcommand = new TrimFlowsCommand(); }
		if (commandName == "shhh.flows") { return shellcommand = new ShhherCommand(); }
		if (commandName == "list.seqs") { return shellcommand = new ListSeqsCommand(); }
		if (commandName == "get.seqs") { return shellcommand = new GetSeqsCommand(); }
		if (commandName == "remove.seqs") { return shellcommand = new RemoveSeqsCommand(); }
		if (commandName == "merge.files") { return shellcommand = new MergeFileCommand(); }
		if (commandName == "system") { return shellcommand = new SystemCommand(); }
		if (commandName == "align.check") { return shellcommand = new AlignCheckCommand(); }
		if (commandName == "get.sharedseqs") { return shellcommand = new GetSharedOTUCommand(); }
		if (commandName == "get.otulist") { return shellcommand = new GetListCountCommand(); }
		if (commandName == "hcluster") { return shellcommand = new HClusterCommand(); }
		if (commandName == "classify.seqs") { return shellcommand = new ClassifySeqsCommand(); }
		if (commandName == "chimera.ccode") { return shellcommand = new ChimeraCcodeCommand(); }
		if (commandName == "chimera.check") { return shellcommand = new ChimeraCheckCommand(); }
		if (commandName == "chimera.slayer") { return shellcommand = new ChimeraSlayerCommand(); }
		if (commandName == "chimera.uchime") { return shellcommand = new ChimeraUchimeCommand(); }
		if (commandName == "chimera.pintail") { return shellcommand = new ChimeraPintailCommand(); }
		if (commandName == "chimera.bellerophon") { return shellcommand = new ChimeraBellerophonCommand(); }
		if (commandName == "phylotype") { return shellcommand = new PhylotypeCommand(); }
		if (commandName == "mgcluster") { return shellcommand = new MGClusterCommand(); }
		if (commandName == "pre.cluster") { return shellcommand = new PreClusterCommand(); }
		if (commandName == "pcoa") { return shellcommand = new PCOACommand(); }
		if (commandName == "pca") { return shellcommand = new PCACommand(); }
		if (commandName == "nmds") { return shellcommand = new NMDSCommand(); }
		if (commandName == "otu.hierarchy") { return shellcommand = new OtuHierarchyCommand(); }
		if (commandName == "set.dir") { return shellcommand = new SetDirectoryCommand(); }
		if (commandName == "set.logfile") { return shellcommand = new SetLogFileCommand(); }
		if (commandName == "parse.list") { return shellcommand = new ParseListCommand(); }
		if (commandName == "phylo.diversity") { return shellcommand = new PhyloDiversityCommand(); }
		if (commandName == "make.group") { return shellcommand = new MakeGroupCommand(); }
		if (commandName == "chop.seqs") { return shellcommand = new ChopSeqsCommand(); }
		if (commandName == "clearcut") { return shellcommand = new ClearcutCommand(); }
		if (commandName == "catchall") { return shellcommand = new CatchAllCommand(); }
		if (commandName == "split.abund") { return shellcommand = new SplitAbundCommand(); }
		if (commandName == "cluster.split") { return shellcommand = new ClusterSplitCommand(); }
		if (commandName == "classify.otu") { return shellcommand = new ClassifyOtuCommand(); }
		if (commandName == "degap.seqs") { return shellcommand = new DegapSeqsCommand(); }
		if (commandName == "get.relabund") { return shellcommand = new GetRelAbundCommand(); }
		if (commandName == "sens.spec") { return shellcommand = new SensSpecCommand(); }
		if (commandName == "seq.error") { return shellcommand = new SeqErrorCommand(); }
		if (commandName == "sffinfo") { return shellcommand = new SffInfoCommand(); }
		if (commandName == "normalize.shared") { return shellcommand = new NormalizeSharedCommand(); }
		if (commandName == "metastats") { return shellcommand = new MetaStatsCommand(); }
		if (commandName == "split.groups") { return shellcommand = new SplitGroupCommand(); }
		if (commandName == "cluster.fragments") { return shellcommand = new ClusterFragmentsCommand(); }
		if (commandName == "get.lineage") { return shellcommand = new GetLineageCommand(); }
		if (commandName == "remove.lineage") { return shellcommand = new RemoveLineageCommand(); }
		if (commandName == "get.groups") { return shellcommand = new GetGroupsCommand(); }
		if (commandName == "remove.groups") { return shellcommand = new RemoveGroupsCommand(); }
		if (commandName == "get.otus") { return shellcommand = new GetOtusCommand(); }
		if (commandName == "remove.otus") { return shellcommand = new RemoveOtusCommand(); }
		if (commandName == "fastq.info") { return shellcommand = new ParseFastaQCommand(); }
		if (commandName == "deunique.seqs") { return shellcommand = new DeUniqueSeqsCommand(); }
		if (commandName == "pairwise.seqs") { return shellcommand = new PairwiseSeqsCommand(); }
		if (commandName == "cluster.classic") { return shellcommand = new ClusterDoturCommand(); }
		if (commandName == "sub.sample") { return shellcommand = new SubSampleCommand(); }
		if (commandName == "indicator") { return shellcommand = new IndicatorCommand(); }
		if (commandName == "consensus.seqs") { return shellcommand = new ConsensusSeqsCommand(); }
		if (commandName == "corr.axes") { return shellcommand = new CorrAxesCommand(); }
		if (commandName == "remove.rare") { return shellcommand = new RemoveRareCommand(); }
		if (commandName == "merge.groups") { return shellcommand = new MergeGroupsCommand(); }
		if (commandName == "amova") { return shellcommand = new AmovaCommand(); }
		if (commandName == "homova") { return shellcommand = new HomovaCommand(); }
		if (commandName == "mantel") { return shellcommand = new MantelCommand(); }
		if (commandName == "anosim") { return shellcommand = new AnosimCommand(); }
		if (commandName == "make.fastq") { return shellcommand = new MakeFastQCommand(); }
		if (commandName == "get.current") { return shellcommand = new GetCurrentCommand(); }
		if (commandName == "set.current") { return shellcommand = new SetCurrentCommand(); }
		if (commandName == "make.shared") { return shellcommand = new SharedCommand(); }
		if (commandName == "get.commandinfo") { return shellcommand = new GetCommandInfoCommand(); }
		if (commandName == "deunique.tree") { return shellcommand = new DeuniqueTreeCommand(); }
		if ((commandName == "count.seqs") || (commandName == "make.table")) { return shellcommand = new CountSeqsCommand(); }
		if (commandName == "count.groups") { return shellcommand = new CountGroupsCommand(); }
		if (commandName == "clear.memory") { return shellcommand = new ClearMemoryCommand(); }
		if (commandName == "summary.tax") { return shellcommand = new SummaryTaxCommand(); }
		if (commandName == "summary.qual") { return shellcommand = new SummaryQualCommand(); }
		if (commandName == "chimera.perseus") { return shellcommand = new ChimeraPerseusCommand(); }
		if (commandName == "shhh.seqs") { return shellcommand = new ShhhSeqsCommand(); }
		if (commandName == "otu.association") { return shellcommand = new OTUAssociationCommand(); }
		if (commandName == "sort.seqs") { return shellcommand = new SortSeqsCommand(); }
		if (commandName == "classify.tree") { return shellcommand = new ClassifyTreeCommand(); }
		if (commandName == "cooccurrence") { return shellcommand = new CooccurrenceCommand(); }
		if (commandName == "pcr.seqs") { return shellcommand = new PcrSeqsCommand(); }
		if (commandName == "create.database") { return shellcommand = new CreateDatabaseCommand(); }
		if (commandName == "make.biom") { return shellcommand = new MakeBiomCommand(); }
		if (commandName == "get.coremicrobiome") { return shellcommand = new GetCoreMicroBiomeCommand(); }
		if (commandName == "list.otulabels") { return shellcommand = new ListOtuLabelsCommand(); }
		if (commandName == "get.otulabels") { return shellcommand = new GetOtuLabelsCommand(); }
		if (commandName == "remove.otulabels") { return shellcommand = new RemoveOtuLabelsCommand(); }
		if (commandName == "make.contigs") { return shellcommand = new MakeContigsCommand(); }
		if (commandName == "load.logfile") { return shellcommand = new LoadLogfileCommand(); }
		if (commandName == "sff.multiple") { return shellcommand = new SffMultipleCommand(); }
		if (commandName == "classify.rf") { return shellcommand = new ClassifyRFSharedCommand(); }
		if (commandName == "filter.shared") { return shellcommand = new FilterSharedCommand(); }
		if (commandName == "primer.design") { return shellcommand = new PrimerDesignCommand(); }
		if (commandName == "get.dists") { return shellcommand = new GetDistsCommand(); }
		if (commandName == "remove.dists") { return shellcommand = new RemoveDistsCommand(); }
		if (commandName == "merge.taxsummary") { return shellcommand = new MergeTaxSummaryCommand(); }
		if (commandName == "get.communitytype") { return shellcommand = new GetMetaCommunityCommand(); }
		if (commandName == "sparcc") { return shellcommand = new SparccCommand(); }
		if (commandName == "make.lookup") { return shellcommand = new MakeLookupCommand(); }
		if (commandName == "rename.seqs") { return shellcommand = new RenameSeqsCommand(); }
		if (commandName == "make.lefse") { return shellcommand = new MakeLefseCommand(); }
		if (commandName == "lefse") { return shellcommand = new LefseCommand(); }
		if (commandName == "kruskal.wallis") { return shellcommand = new KruskalWallisCommand(); }
		if (commandName == "classify.svm") { return shellcommand = new ClassifySvmSharedCommand(); }
		if (commandName == "make.sra") { return shellcommand = new SRACommand(); }
		if (commandName == "merge.sfffiles") { return shellcommand = new MergeSfffilesCommand(); }
		if (commandName == "get.mimarkspackage") { return shellcommand = new GetMIMarksPackageCommand(); }
		if (commandName == "mimarks.attributes") { return shellcommand = new MimarksAttributesCommand(); }
		if (commandName == "set.seed") { return shellcommand = new SetSeedCommand(); }
		if (commandName == "make.file") { return shellcommand = new MakeFileCommand(); }
		return shellcommand = new NoCommand();
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************
//This function is used to interrupt a command
Command* CommandFactory::getCommand(){
try {
delete command;   //delete the old command

string s = "";
command = new NoCommand(s);

return command;
}
catch(exception& e) {
m->errorOut(e, "CommandFactory", "getCommand");
exit(1);
}
}
***********************************************************************/
bool CommandFactory::isValidCommand(string command) {
	try {

		//is the command in the map
		if ((commands.find(command)) != (commands.end())) {
			return true;
		}
		else {
			m->mothurOut(command + " is not a valid command in Mothur.  Valid commands are ");
			for (it = commands.begin(); it != commands.end(); it++) {
				m->mothurOut(it->first + ", ");
			}
			m->mothurOutEndLine();
			return false;
		}

	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "isValidCommand");
		exit(1);
	}
}
/***********************************************************************/
bool CommandFactory::isValidCommand(string command, string noError) {
	try {

		//is the command in the map
		if ((commands.find(command)) != (commands.end())) {
			return true;
		}
		else {
			return false;
		}

	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "isValidCommand");
		exit(1);
	}
}
/***********************************************************************/
void CommandFactory::printCommands(ostream& out) {
	try {
		out << "Valid commands are: ";
		for (it = commands.begin(); it != commands.end(); it++) {
			out << it->first << ",";
		}
		out << endl;
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "printCommands");
		exit(1);
	}
}
/***********************************************************************/
void CommandFactory::printCommandsCategories(ostream& out) {
	try {
		map<string, string> commands = getListCommands();
		map<string, string>::iterator it;

		map<string, string> categories;
		map<string, string>::iterator itCat;
		//loop through each command outputting info
		for (it = commands.begin(); it != commands.end(); it++) {

			Command* thisCommand = getCommand(it->first);

			//don't add hidden commands
			if (thisCommand->getCommandCategory() != "Hidden") {
				itCat = categories.find(thisCommand->getCommandCategory());
				if (itCat == categories.end()) {
					categories[thisCommand->getCommandCategory()] = thisCommand->getCommandName();
				}
				else {
					categories[thisCommand->getCommandCategory()] += ", " + thisCommand->getCommandName();
				}
			}
		}

		for (itCat = categories.begin(); itCat != categories.end(); itCat++) {
			out << itCat->first << " commmands include: " << itCat->second << endl;
		}

	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "printCommandsCategories");
		exit(1);
	}
}

/***********************************************************************/

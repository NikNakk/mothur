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
#include "biominfocommand.h"

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
CommandFactory::CommandFactory():
append(false)
{
	m = MothurOut::getInstance();
	// Register commands
	Register("cluster", unique_ptr<CommandMakerBase>(new CommandMaker<ClusterCommand>));
	Register("unique.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<DeconvoluteCommand>));
	Register("parsimony", unique_ptr<CommandMakerBase>(new CommandMaker<ParsimonyCommand>));
	Register("help", unique_ptr<CommandMakerBase>(new CommandMaker<HelpCommand>));
	Register("quit", unique_ptr<CommandMakerBase>(new CommandMaker<QuitCommand>));
	Register("collect.single", unique_ptr<CommandMakerBase>(new CommandMaker<CollectCommand>));
	Register("collect.shared", unique_ptr<CommandMakerBase>(new CommandMaker<CollectSharedCommand>));
	Register("rarefaction.single", unique_ptr<CommandMakerBase>(new CommandMaker<RareFactCommand>));
	Register("rarefaction.shared", unique_ptr<CommandMakerBase>(new CommandMaker<RareFactSharedCommand>));
	Register("summary.single", unique_ptr<CommandMakerBase>(new CommandMaker<SummaryCommand>));
	Register("summary.shared", unique_ptr<CommandMakerBase>(new CommandMaker<SummarySharedCommand>));
	Register("unifrac.weighted", unique_ptr<CommandMakerBase>(new CommandMaker<UnifracWeightedCommand>));
	Register("unifrac.unweighted", unique_ptr<CommandMakerBase>(new CommandMaker<UnifracUnweightedCommand>));
	Register("get.group", unique_ptr<CommandMakerBase>(new CommandMaker<GetgroupCommand>));
	Register("get.label", unique_ptr<CommandMakerBase>(new CommandMaker<GetlabelCommand>));
	Register("get.sabund", unique_ptr<CommandMakerBase>(new CommandMaker<GetSAbundCommand>));
	Register("get.rabund", unique_ptr<CommandMakerBase>(new CommandMaker<GetRAbundCommand>));
	Register("libshuff", unique_ptr<CommandMakerBase>(new CommandMaker<LibShuffCommand>));
	Register("heatmap.bin", unique_ptr<CommandMakerBase>(new CommandMaker<HeatMapCommand>));
	Register("heatmap.sim", unique_ptr<CommandMakerBase>(new CommandMaker<HeatMapSimCommand>));
	Register("filter.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<FilterSeqsCommand>));
	Register("venn", unique_ptr<CommandMakerBase>(new CommandMaker<VennCommand>));
	Register("bin.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<BinSeqCommand>));
	Register("get.oturep", unique_ptr<CommandMakerBase>(new CommandMaker<GetOTURepCommand>));
	Register("tree.shared", unique_ptr<CommandMakerBase>(new CommandMaker<TreeGroupCommand>));
	Register("dist.shared", unique_ptr<CommandMakerBase>(new CommandMaker<MatrixOutputCommand>));
	Register("dist.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<DistanceCommand>));
	Register("align.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<AlignCommand>));
	Register("summary.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<SeqSummaryCommand>));
	Register("screen.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ScreenSeqsCommand>));
	Register("reverse.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ReverseSeqsCommand>));
	Register("trim.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<TrimSeqsCommand>));
	Register("trim.flows", unique_ptr<CommandMakerBase>(new CommandMaker<TrimFlowsCommand>));
	Register("shhh.flows", unique_ptr<CommandMakerBase>(new CommandMaker<ShhherCommand>));
	Register("list.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ListSeqsCommand>));
	Register("get.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<GetSeqsCommand>));
	Register("remove.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveSeqsCommand>));
	Register("merge.files", unique_ptr<CommandMakerBase>(new CommandMaker<MergeFileCommand>));
	Register("system", unique_ptr<CommandMakerBase>(new CommandMaker<SystemCommand>));
	Register("align.check", unique_ptr<CommandMakerBase>(new CommandMaker<AlignCheckCommand>));
	Register("get.sharedseqs", unique_ptr<CommandMakerBase>(new CommandMaker<GetSharedOTUCommand>));
	Register("get.otulist", unique_ptr<CommandMakerBase>(new CommandMaker<GetListCountCommand>));
	Register("hcluster", unique_ptr<CommandMakerBase>(new CommandMaker<HClusterCommand>));
	Register("classify.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ClassifySeqsCommand>));
	Register("chimera.ccode", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraCcodeCommand>));
	Register("chimera.check", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraCheckCommand>));
	Register("chimera.slayer", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraSlayerCommand>));
	Register("chimera.uchime", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraUchimeCommand>));
	Register("chimera.pintail", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraPintailCommand>));
	Register("chimera.bellerophon", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraBellerophonCommand>));
	Register("phylotype", unique_ptr<CommandMakerBase>(new CommandMaker<PhylotypeCommand>));
	Register("mgcluster", unique_ptr<CommandMakerBase>(new CommandMaker<MGClusterCommand>));
	Register("pre.cluster", unique_ptr<CommandMakerBase>(new CommandMaker<PreClusterCommand>));
	Register("pcoa", unique_ptr<CommandMakerBase>(new CommandMaker<PCOACommand>));
	Register("pca", unique_ptr<CommandMakerBase>(new CommandMaker<PCACommand>));
	Register("nmds", unique_ptr<CommandMakerBase>(new CommandMaker<NMDSCommand>));
	Register("otu.hierarchy", unique_ptr<CommandMakerBase>(new CommandMaker<OtuHierarchyCommand>));
	Register("set.dir", unique_ptr<CommandMakerBase>(new CommandMaker<SetDirectoryCommand>));
	Register("set.logfile", unique_ptr<CommandMakerBase>(new CommandMaker<SetLogFileCommand>));
	Register("parse.list", unique_ptr<CommandMakerBase>(new CommandMaker<ParseListCommand>));
	Register("phylo.diversity", unique_ptr<CommandMakerBase>(new CommandMaker<PhyloDiversityCommand>));
	Register("make.group", unique_ptr<CommandMakerBase>(new CommandMaker<MakeGroupCommand>));
	Register("chop.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ChopSeqsCommand>));
	Register("clearcut", unique_ptr<CommandMakerBase>(new CommandMaker<ClearcutCommand>));
	Register("catchall", unique_ptr<CommandMakerBase>(new CommandMaker<CatchAllCommand>));
	Register("split.abund", unique_ptr<CommandMakerBase>(new CommandMaker<SplitAbundCommand>));
	Register("cluster.split", unique_ptr<CommandMakerBase>(new CommandMaker<ClusterSplitCommand>));
	Register("classify.otu", unique_ptr<CommandMakerBase>(new CommandMaker<ClassifyOtuCommand>));
	Register("degap.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<DegapSeqsCommand>));
	Register("get.relabund", unique_ptr<CommandMakerBase>(new CommandMaker<GetRelAbundCommand>));
	Register("sens.spec", unique_ptr<CommandMakerBase>(new CommandMaker<SensSpecCommand>));
	Register("seq.error", unique_ptr<CommandMakerBase>(new CommandMaker<SeqErrorCommand>));
	Register("sffinfo", unique_ptr<CommandMakerBase>(new CommandMaker<SffInfoCommand>));
	Register("normalize.shared", unique_ptr<CommandMakerBase>(new CommandMaker<NormalizeSharedCommand>));
	Register("metastats", unique_ptr<CommandMakerBase>(new CommandMaker<MetaStatsCommand>));
	Register("split.groups", unique_ptr<CommandMakerBase>(new CommandMaker<SplitGroupCommand>));
	Register("cluster.fragments", unique_ptr<CommandMakerBase>(new CommandMaker<ClusterFragmentsCommand>));
	Register("get.lineage", unique_ptr<CommandMakerBase>(new CommandMaker<GetLineageCommand>));
	Register("remove.lineage", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveLineageCommand>));
	Register("get.groups", unique_ptr<CommandMakerBase>(new CommandMaker<GetGroupsCommand>));
	Register("remove.groups", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveGroupsCommand>));
	Register("get.otus", unique_ptr<CommandMakerBase>(new CommandMaker<GetOtusCommand>));
	Register("remove.otus", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveOtusCommand>));
	Register("fastq.info", unique_ptr<CommandMakerBase>(new CommandMaker<ParseFastaQCommand>));
	Register("pipeline.pds", unique_ptr<CommandMakerBase>(new CommandMaker<PipelineCommand>));
	Register("deunique.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<DeUniqueSeqsCommand>));
	Register("pairwise.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<PairwiseSeqsCommand>));
	Register("cluster.classic", unique_ptr<CommandMakerBase>(new CommandMaker<ClusterDoturCommand>));
	Register("sub.sample", unique_ptr<CommandMakerBase>(new CommandMaker<SubSampleCommand>));
	Register("indicator", unique_ptr<CommandMakerBase>(new CommandMaker<IndicatorCommand>));
	Register("consensus.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ConsensusSeqsCommand>));
	Register("corr.axes", unique_ptr<CommandMakerBase>(new CommandMaker<CorrAxesCommand>));
	Register("remove.rare", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveRareCommand>));
	Register("merge.groups", unique_ptr<CommandMakerBase>(new CommandMaker<MergeGroupsCommand>));
	Register("amova", unique_ptr<CommandMakerBase>(new CommandMaker<AmovaCommand>));
	Register("homova", unique_ptr<CommandMakerBase>(new CommandMaker<HomovaCommand>));
	Register("mantel", unique_ptr<CommandMakerBase>(new CommandMaker<MantelCommand>));
	Register("make.fastq", unique_ptr<CommandMakerBase>(new CommandMaker<MakeFastQCommand>));
	Register("get.current", unique_ptr<CommandMakerBase>(new CommandMaker<GetCurrentCommand>));
	Register("set.current", unique_ptr<CommandMakerBase>(new CommandMaker<SetCurrentCommand>));
	Register("anosim", unique_ptr<CommandMakerBase>(new CommandMaker<AnosimCommand>));
	Register("make.shared", unique_ptr<CommandMakerBase>(new CommandMaker<SharedCommand>));
	Register("get.commandinfo", unique_ptr<CommandMakerBase>(new CommandMaker<GetCommandInfoCommand>));
	Register("deunique.tree", unique_ptr<CommandMakerBase>(new CommandMaker<DeuniqueTreeCommand>));
	Register("count.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<CountSeqsCommand>));
	Register("make.table", unique_ptr<CommandMakerBase>(new CommandMaker<CountSeqsCommand>));
	Register("count.groups", unique_ptr<CommandMakerBase>(new CommandMaker<CountGroupsCommand>));
	Register("clear.memory", unique_ptr<CommandMakerBase>(new CommandMaker<ClearMemoryCommand>));
	Register("summary.tax", unique_ptr<CommandMakerBase>(new CommandMaker<SummaryTaxCommand>));
	Register("summary.qual", unique_ptr<CommandMakerBase>(new CommandMaker<SummaryQualCommand>));
	Register("chimera.perseus", unique_ptr<CommandMakerBase>(new CommandMaker<ChimeraPerseusCommand>));
	Register("shhh.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<ShhhSeqsCommand>));
	Register("otu.association", unique_ptr<CommandMakerBase>(new CommandMaker<OTUAssociationCommand>));
	Register("sort.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<SortSeqsCommand>));
	Register("classify.tree", unique_ptr<CommandMakerBase>(new CommandMaker<ClassifyTreeCommand>));
	Register("cooccurrence", unique_ptr<CommandMakerBase>(new CommandMaker<CooccurrenceCommand>));
	Register("pcr.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<PcrSeqsCommand>));
	Register("create.database", unique_ptr<CommandMakerBase>(new CommandMaker<CreateDatabaseCommand>));
	Register("make.biom", unique_ptr<CommandMakerBase>(new CommandMaker<MakeBiomCommand>));
	Register("get.coremicrobiome", unique_ptr<CommandMakerBase>(new CommandMaker<GetCoreMicroBiomeCommand>));
	Register("list.otulabels", unique_ptr<CommandMakerBase>(new CommandMaker<ListOtuLabelsCommand>));
	Register("get.otulabels", unique_ptr<CommandMakerBase>(new CommandMaker<GetOtuLabelsCommand>));
	Register("remove.otulabels", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveOtuLabelsCommand>));
	Register("make.contigs", unique_ptr<CommandMakerBase>(new CommandMaker<MakeContigsCommand>));
	Register("load.logfile", unique_ptr<CommandMakerBase>(new CommandMaker<LoadLogfileCommand>));
	Register("sff.multiple", unique_ptr<CommandMakerBase>(new CommandMaker<SffMultipleCommand>));
	Register("classify.svm", unique_ptr<CommandMakerBase>(new CommandMaker<ClassifySvmSharedCommand>));
	Register("classify.rf", unique_ptr<CommandMakerBase>(new CommandMaker<ClassifyRFSharedCommand>));
	Register("filter.shared", unique_ptr<CommandMakerBase>(new CommandMaker<FilterSharedCommand>));
	Register("primer.design", unique_ptr<CommandMakerBase>(new CommandMaker<PrimerDesignCommand>));
	Register("get.dists", unique_ptr<CommandMakerBase>(new CommandMaker<GetDistsCommand>));
	Register("remove.dists", unique_ptr<CommandMakerBase>(new CommandMaker<RemoveDistsCommand>));
	Register("merge.taxsummary", unique_ptr<CommandMakerBase>(new CommandMaker<MergeTaxSummaryCommand>));
	Register("get.communitytype", unique_ptr<CommandMakerBase>(new CommandMaker<GetMetaCommunityCommand>));
	Register("sparcc", unique_ptr<CommandMakerBase>(new CommandMaker<SparccCommand>));
	Register("make.lookup", unique_ptr<CommandMakerBase>(new CommandMaker<MakeLookupCommand>));
	Register("rename.seqs", unique_ptr<CommandMakerBase>(new CommandMaker<RenameSeqsCommand>));
	Register("make.lefse", unique_ptr<CommandMakerBase>(new CommandMaker<MakeLefseCommand>));
	Register("lefse", unique_ptr<CommandMakerBase>(new CommandMaker<LefseCommand>));
	Register("kruskal.wallis", unique_ptr<CommandMakerBase>(new CommandMaker<KruskalWallisCommand>));
	Register("make.sra", unique_ptr<CommandMakerBase>(new CommandMaker<SRACommand>));
	Register("merge.sfffiles", unique_ptr<CommandMakerBase>(new CommandMaker<MergeSfffilesCommand>));
	Register("get.mimarkspackage", unique_ptr<CommandMakerBase>(new CommandMaker<GetMIMarksPackageCommand>));
	Register("mimarks.attributes", unique_ptr<CommandMakerBase>(new CommandMaker<MimarksAttributesCommand>));
	Register("set.seed", unique_ptr<CommandMakerBase>(new CommandMaker<SetSeedCommand>));
	Register("biom.info", unique_ptr<CommandMakerBase>(new CommandMaker<BiomInfoCommand>));
	Register("make.file", unique_ptr<CommandMakerBase>(new CommandMaker<MakeFileCommand>));
}

/***********************************************************/
CommandFactory::~CommandFactory() {
	_uniqueInstance = 0;
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

        pos = optionString.find("seed=");
        if (pos != string::npos) { //user has set seed in command option string
            string intputOption = "";
            bool foundEquals = false;
            for(int i=pos;i<optionString.length();i++){
                if(optionString[i] == ',')       { break;               }
                else if(optionString[i] == '=')  { foundEquals = true;	}
                if (foundEquals)       {   intputOption += optionString[i]; }
            }
            if (intputOption[0] == '=') { intputOption = intputOption.substr(1); }
            bool seed = false; int random;
            if (intputOption == "clear") {
                random = time(NULL);
                seed = true;
            }else {
                if (m->isNumeric1(intputOption)) { m->mothurConvert(intputOption, random); seed=true; }
                else { m->mothurOut("[ERROR]: Seed must be an integer."); m->mothurOutEndLine(); seed = false;}
            }

            if (seed)  {
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
unique_ptr<Command> CommandFactory::getCommand(string commandName, string optionString) {
	try {
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

		return Create(commandName, optionString);
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************/

/***********************************************************/
//This function calls the appropriate command fucntions based on user input.
unique_ptr<Command> CommandFactory::getCommand(string commandName, string optionString, string mode) {
	return getCommand(commandName, optionString);
}
/***********************************************************/

/***********************************************************/
//This function calls the appropriate command fucntions based on user input, this is used by the pipeline command to check a users piepline for errors before running
unique_ptr<Command> CommandFactory::getCommand(string commandName) {
	try {
		return Create(commandName);
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************************/
bool CommandFactory::isValidCommand(string command) {
	try {

		//is the command in the map
		if ((commandMakers.find(command)) != (commandMakers.end())) {
			return true;
		}
		else {
			m->mothurOut(command + " is not a valid command in Mothur.  Valid commands are ");
			for (TMapCommands::iterator it = commandMakers.begin(); it != commandMakers.end(); it++) {
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
		if ((commandMakers.find(command)) != (commandMakers.end())) {
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
		for (TMapCommands::iterator it = commandMakers.begin(); it != commandMakers.end(); it++) {
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
		unique_ptr<vector<string>> commands = getListCommands();
		vector<string>::iterator it;

		map<string, string> categories;
		map<string, string>::iterator itCat;
		//loop through each command outputting info
		for (it = commands->begin(); it != commands->end(); it++) {

			unique_ptr<Command> thisCommand = getCommand(*it);

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

void CommandFactory::Register(string command, unique_ptr<CommandMakerBase> creator)
{
	commandMakers[command] = move(creator);
}
/***********************************************************************/

/***********************************************************************/
unique_ptr<Command> CommandFactory::Create(string command, string optionString)
{
	TMapCommands::iterator it = commandMakers.find(command);
	if (it == commandMakers.end()) {
		return unique_ptr<Command>(new NoCommand(optionString));
	}
	else {
		return it->second->Create(optionString);
	}
}
/***********************************************************************/
unique_ptr<Command> CommandFactory::Create(string command)
{
	TMapCommands::iterator it = commandMakers.find(command);
	if (it == commandMakers.end()) {
		return unique_ptr<Command>(new NoCommand());
	}
	else {
		return it->second->Create();
	}
}
/***********************************************************************/
unique_ptr<vector<string>> CommandFactory::getListCommands()
{
	unique_ptr<vector<string>> commandList(new vector<string>(commandMakers.size()));
	for(TMapCommands::iterator it = commandMakers.begin(); it != commandMakers.end(); ++it) {
		commandList->push_back(it->first);
	}
	return commandList;
}
/***********************************************************************/

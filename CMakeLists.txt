cmake_minimum_required (VERSION 2.8.8)
project (mothur)

# The version number.
set (MOTHUR_VERSION 1.37.0)
set (MOTHUR_RELEASE_DATE 7/23/2015)

option(USE_READLINE 
  "Use readline library" ON)
option(USE_BOOST
  "Use boost library" ON)
option(USE_COMPRESSION
  "Use compression (may only work on *nix systems)" OFF)
set(MOTHUR_FILES ""
  CACHE FILEPATH "Default path for mothur files")

if(CMAKE_COMPILER_IS_GNUCXX)
   execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
		message(STATUS "C++11 activated.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   else ()
		message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.7 is needed.")   
   endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
 	message(STATUS "C++11 activated.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)
  
enable_testing()

include_directories ("${PROJECT_BINARY_DIR}/source")

add_executable(mothur
# source/averagelinkage.cpp
# source/calcsparcc.cpp
# source/calculators/ace.cpp
# source/calculators/bergerparker.cpp
# source/calculators/boneh.cpp
# source/calculators/bootstrap.cpp
# source/calculators/bstick.cpp
# source/calculators/calculator.cpp
# source/calculators/canberra.cpp
# source/calculators/chao1.cpp
# source/calculators/coverage.cpp
# source/calculators/efron.cpp
# source/calculators/geom.cpp
# source/calculators/goodscoverage.cpp
# source/calculators/gower.cpp
# source/calculators/hamming.cpp
# source/calculators/heip.cpp
# source/calculators/hellinger.cpp
# source/calculators/invsimpson.cpp
# source/calculators/jackknife.cpp
# source/calculators/logsd.cpp
# source/calculators/manhattan.cpp
# source/calculators/memchi2.cpp
# source/calculators/memchord.cpp
# source/calculators/memeuclidean.cpp
# source/calculators/mempearson.cpp
# source/calculators/npshannon.cpp
# source/calculators/odum.cpp
# source/calculators/parsimony.cpp
# source/calculators/prng.cpp
# source/calculators/qstat.cpp
# source/calculators/shannon.cpp
# source/calculators/shannoneven.cpp
# source/calculators/shannonrange.cpp
# source/calculators/sharedace.cpp
# source/calculators/sharedanderbergs.cpp
# source/calculators/sharedbraycurtis.cpp
# source/calculators/sharedchao1.cpp
# source/calculators/sharedjabund.cpp
# source/calculators/sharedjackknife.cpp
# source/calculators/sharedjclass.cpp
# source/calculators/sharedjest.cpp
# source/calculators/sharedjsd.cpp
# source/calculators/sharedkstest.cpp
# source/calculators/sharedkulczynski.cpp
# source/calculators/sharedkulczynskicody.cpp
# source/calculators/sharedlennon.cpp
# source/calculators/sharedmarczewski.cpp
# source/calculators/sharedmorisitahorn.cpp
# source/calculators/sharedochiai.cpp
# source/calculators/sharedrjsd.cpp
# source/calculators/sharedsobs.cpp
# source/calculators/sharedsobscollectsummary.cpp
# source/calculators/sharedsorabund.cpp
# source/calculators/sharedsorclass.cpp
# source/calculators/sharedsorest.cpp
# source/calculators/sharedthetan.cpp
# source/calculators/sharedthetayc.cpp
# source/calculators/shen.cpp
# source/calculators/simpson.cpp
# source/calculators/simpsoneven.cpp
# source/calculators/smithwilson.cpp
# source/calculators/soergel.cpp
# source/calculators/solow.cpp
# source/calculators/spearman.cpp
# source/calculators/speciesprofile.cpp
# source/calculators/structchi2.cpp
# source/calculators/structchord.cpp
# source/calculators/structeuclidean.cpp
# source/calculators/structkulczynski.cpp
# source/calculators/structpearson.cpp
# source/calculators/unweighted.cpp
# source/calculators/uvest.cpp
# source/calculators/weighted.cpp
# source/calculators/whittaker.cpp
# source/chimera/bellerophon.cpp
# source/chimera/ccode.cpp
# source/chimera/chimera.cpp
# source/chimera/chimeracheckrdp.cpp
# source/chimera/chimerarealigner.cpp
# source/chimera/chimeraslayer.cpp
# source/chimera/decalc.cpp
# source/chimera/maligner.cpp
# source/chimera/myPerseus.cpp
# source/chimera/pintail.cpp
# source/chimera/slayer.cpp
# source/classifier/alignnode.cpp
# source/classifier/aligntree.cpp
# source/classifier/bayesian.cpp
# source/classifier/classify.cpp
# source/classifier/kmernode.cpp
# source/classifier/kmertree.cpp
# source/classifier/knn.cpp
# source/classifier/phylosummary.cpp
# source/classifier/phylotree.cpp
# source/classifier/taxonomyequalizer.cpp
# source/classifier/taxonomynode.cpp
# source/clearcut/clearcut.cpp
# source/clearcut/cmdargs.cpp
# source/clearcut/distclearcut.cpp
# source/clearcut/dmat.cpp
# source/clearcut/fasta.cpp
# source/clearcut/getopt_long.cpp
# source/cluster.cpp
# source/clusterclassic.cpp
# source/collect.cpp
source/commandfactory.cpp
# source/commandoptionparser.cpp
source/commandparameters/booleanparameter.cpp
source/commandparameters/commandparametercollection.cpp
source/commandparameters/countfileparameter.cpp
source/commandparameters/directoryparameter.cpp
source/commandparameters/fastafileparameter.cpp
source/commandparameters/groupfileparameter.cpp
source/commandparameters/inputdirectoryparameter.cpp
source/commandparameters/inputtypeparameter.cpp
source/commandparameters/integerparameter.cpp
source/commandparameters/multipleparameter.cpp
source/commandparameters/namefileparameter.cpp
source/commandparameters/numberparameter.cpp
source/commandparameters/outputdirectoryparameter.cpp
source/commandparameters/outputfileparameter.cpp
source/commandparameters/processorsparameter.cpp
source/commandparameters/seedparameter.cpp
# source/commands/aligncommand.cpp
# source/commands/amovacommand.cpp
# source/commands/anosimcommand.cpp
# source/commands/binsequencecommand.cpp
# source/commands/biominfocommand.cpp
# source/commands/catchallcommand.cpp
# source/commands/chimerabellerophoncommand.cpp
# source/commands/chimeraccodecommand.cpp
# source/commands/chimeracheckcommand.cpp
# source/commands/chimeraperseuscommand.cpp
# source/commands/chimerapintailcommand.cpp
# source/commands/chimeraslayercommand.cpp
# source/commands/chimerauchimecommand.cpp
# source/commands/chopseqscommand.cpp
# source/commands/classifyotucommand.cpp
# source/commands/classifyrfsharedcommand.cpp
# source/commands/classifyseqscommand.cpp
# source/commands/classifysvmsharedcommand.cpp
# source/commands/classifytreecommand.cpp
# source/commands/clearcutcommand.cpp
# source/commands/clearmemorycommand.cpp
# source/commands/clustercommand.cpp
# source/commands/clusterdoturcommand.cpp
# source/commands/clusterfragmentscommand.cpp
# source/commands/clustersplitcommand.cpp
# source/commands/collectcommand.cpp
# source/commands/collectsharedcommand.cpp
source/commands/command.cpp
# source/commands/consensusseqscommand.cpp
# source/commands/cooccurrencecommand.cpp
# source/commands/corraxescommand.cpp
# source/commands/countgroupscommand.cpp
# source/commands/countseqscommand.cpp
# source/commands/createdatabasecommand.cpp
source/commands/deconvolutecommand.cpp
# source/commands/degapseqscommand.cpp
# source/commands/deuniqueseqscommand.cpp
# source/commands/deuniquetreecommand.cpp
# source/commands/distancecommand.cpp
# source/commands/filterseqscommand.cpp
# source/commands/filtersharedcommand.cpp
# source/commands/getcommandinfocommand.cpp
# source/commands/getcoremicrobiomecommand.cpp
# source/commands/getcurrentcommand.cpp
# source/commands/getdistscommand.cpp
# source/commands/getgroupcommand.cpp
# source/commands/getgroupscommand.cpp
# source/commands/getlabelcommand.cpp
# source/commands/getlineagecommand.cpp
# source/commands/getlistcountcommand.cpp
# source/commands/getmetacommunitycommand.cpp
# source/commands/getmimarkspackagecommand.cpp
# source/commands/getotulabelscommand.cpp
# source/commands/getoturepcommand.cpp
# source/commands/getotuscommand.cpp
# source/commands/getrabundcommand.cpp
# source/commands/getrelabundcommand.cpp
# source/commands/getsabundcommand.cpp
# source/commands/getseqscommand.cpp
# source/commands/getsharedotucommand.cpp
# source/commands/hclustercommand.cpp
# source/commands/heatmapcommand.cpp
# source/commands/heatmapsimcommand.cpp
source/commands/helpcommand.cpp
# source/commands/homovacommand.cpp
# source/commands/indicatorcommand.cpp
# source/commands/kruskalwalliscommand.cpp
# source/commands/lefsecommand.cpp
# source/commands/libshuffcommand.cpp
# source/commands/listotulabelscommand.cpp
# source/commands/listseqscommand.cpp
# source/commands/loadlogfilecommand.cpp
# source/commands/makebiomcommand.cpp
# source/commands/makecontigscommand.cpp
# source/commands/makefastqcommand.cpp
# source/commands/makefilecommand.cpp
# source/commands/makegroupcommand.cpp
# source/commands/makelefsecommand.cpp
# source/commands/makelookupcommand.cpp
# source/commands/mantelcommand.cpp
# source/commands/matrixoutputcommand.cpp
# source/commands/mergefilecommand.cpp
# source/commands/mergegroupscommand.cpp
# source/commands/mergesfffilecommand.cpp
# source/commands/mergetaxsummarycommand.cpp
# source/commands/metastatscommand.cpp
# source/commands/mgclustercommand.cpp
# source/commands/mimarksattributescommand.cpp
# source/commands/newcommandtemplate.cpp
# source/commands/nmdscommand.cpp
source/commands/nocommands.cpp
# source/commands/normalizesharedcommand.cpp
# source/commands/otuassociationcommand.cpp
# source/commands/otuhierarchycommand.cpp
# source/commands/pairwiseseqscommand.cpp
# source/commands/parsefastaqcommand.cpp
# source/commands/parselistscommand.cpp
# source/commands/parsimonycommand.cpp
# source/commands/pcacommand.cpp
# source/commands/pcoacommand.cpp
# source/commands/pcrseqscommand.cpp
# source/commands/phylodiversitycommand.cpp
# source/commands/phylotypecommand.cpp
# source/commands/pipelinepdscommand.cpp
# source/commands/preclustercommand.cpp
# source/commands/primerdesigncommand.cpp
# source/commands/rarefactcommand.cpp
# source/commands/rarefactsharedcommand.cpp
# source/commands/removedistscommand.cpp
# source/commands/removegroupscommand.cpp
# source/commands/removelineagecommand.cpp
# source/commands/removeotulabelscommand.cpp
# source/commands/removeotuscommand.cpp
# source/commands/removerarecommand.cpp
# source/commands/removeseqscommand.cpp
# source/commands/renameseqscommand.cpp
# source/commands/reversecommand.cpp
source/commands/screenseqscommand.cpp
# source/commands/secondarystructurecommand.cpp
# source/commands/sensspeccommand.cpp
# source/commands/seqerrorcommand.cpp
source/commands/seqsummarycommand.cpp
# source/commands/setcurrentcommand.cpp
source/commands/setdircommand.cpp
source/commands/setlogfilecommand.cpp
source/commands/setseedcommand.cpp
# source/commands/sffinfocommand.cpp
# source/commands/sffmultiplecommand.cpp
# source/commands/sharedcommand.cpp
# source/commands/shhhercommand.cpp
# source/commands/shhhseqscommand.cpp
# source/commands/sortseqscommand.cpp
# source/commands/sparcccommand.cpp
# source/commands/splitabundcommand.cpp
# source/commands/splitgroupscommand.cpp
# source/commands/sracommand.cpp
# source/commands/subsamplecommand.cpp
# source/commands/summarycommand.cpp
# source/commands/summaryqualcommand.cpp
# source/commands/summarysharedcommand.cpp
# source/commands/summarytaxcommand.cpp
# source/commands/systemcommand.cpp
# source/commands/treegroupscommand.cpp
# source/commands/trimflowscommand.cpp
# source/commands/trimseqscommand.cpp
# source/commands/unifracunweightedcommand.cpp
# source/commands/unifracweightedcommand.cpp
# source/commands/venncommand.cpp
# source/communitytype/communitytype.cpp
# source/communitytype/kmeans.cpp
# source/communitytype/pam.cpp
# source/communitytype/qFinderDMM.cpp
# source/completelinkage.cpp
# source/consensus.cpp
# source/datastructures/alignment.cpp
# source/datastructures/alignmentcell.cpp
# source/datastructures/alignmentdb.cpp
# source/datastructures/blastalign.cpp
# source/datastructures/blastdb.cpp
source/datastructures/counttable.cpp
# source/datastructures/database.cpp
# source/datastructures/designmap.cpp
# source/datastructures/distancedb.cpp
# source/datastructures/fastamap.cpp
# source/datastructures/fastqread.cpp
# source/datastructures/flowdata.cpp
# source/datastructures/fullmatrix.cpp
source/datastructures/groupmap.cpp
# source/datastructures/isequenceparser.cpp
# source/datastructures/kmer.cpp
# source/datastructures/kmeralign.cpp
# source/datastructures/kmerdb.cpp
# source/datastructures/listvector.cpp
# source/datastructures/nameassignment.cpp
source/datastructures/namemap.cpp
# source/datastructures/oligos.cpp
# source/datastructures/ordervector.cpp
# source/datastructures/outputtype.cpp
# source/datastructures/qualityscores.cpp
# source/datastructures/rabundvector.cpp
# source/datastructures/referencedb.cpp
# source/datastructures/reportfile.cpp
# source/datastructures/sabundvector.cpp
source/datastructures/sequence.cpp
# source/datastructures/sequencecountparser.cpp
# source/datastructures/sequencedb.cpp
# source/datastructures/sequenceparser.cpp
# source/datastructures/sharedlistvector.cpp
# source/datastructures/sharedordervector.cpp
# source/datastructures/sharedrabundfloatvector.cpp
# source/datastructures/sharedrabundvector.cpp
# source/datastructures/sharedsabundvector.cpp
# source/datastructures/sparsedistancematrix.cpp
# source/datastructures/sparsematrix.cpp
# source/datastructures/suffixdb.cpp
# source/datastructures/suffixnodes.cpp
# source/datastructures/suffixtree.cpp
# source/datastructures/tree.cpp
# source/datastructures/treemap.cpp
# source/datastructures/treenode.cpp
# source/dlibshuff.cpp
source/engine.cpp
source/filehandling/countfileread.cpp
source/filehandling/countfilewrite.cpp
source/filehandling/fastafileread.cpp
source/filehandling/fastafilewrite.cpp
source/filehandling/groupfileread.cpp
source/filehandling/groupfilewrite.cpp
source/filehandling/namefileread.cpp
source/filehandling/namefilewrite.cpp
source/filehandling/file.cpp
source/filehandling/summaryfileread.cpp
source/filehandling/summaryfilewrite.cpp
source/filehandling/textfileread.cpp
source/filehandling/textfilewrite.cpp
# source/fileoutput.cpp
# source/gotohoverlap.cpp
# source/hcluster.cpp
# source/heatmap.cpp
# source/heatmapsim.cpp
# source/inputdata.cpp
# source/libshuff.cpp
# source/linearalgebra.cpp
source/logsinks.cpp
# source/metastats/mothurfisher.cpp
# source/metastats/mothurmetastats.cpp
source/mothur.cpp
# source/mothurout.cpp
# source/myseqdist.cpp
# source/nast.cpp
# source/nastreport.cpp
# source/needlemanoverlap.cpp
# source/noalign.cpp
# source/optionparser.cpp
# source/overlap.cpp
# source/progress.cpp
# source/randomforest/abstractdecisiontree.cpp
# source/randomforest/abstractrandomforest.cpp
# source/randomforest/decisiontree.cpp
# source/randomforest/forest.cpp
# source/randomforest/randomforest.cpp
# source/randomforest/regularizeddecisiontree.cpp
# source/randomforest/regularizedrandomforest.cpp
# source/randomforest/rftreenode.cpp
# source/randomnumber.cpp
# source/rarecalc.cpp
# source/raredisplay.cpp
# source/rarefact.cpp
# source/read/formatcolumn.cpp
# source/read/formatphylip.cpp
# source/read/readblast.cpp
# source/read/readcluster.cpp
# source/read/readcolumn.cpp
# source/read/readphylip.cpp
# source/read/readphylipvector.cpp
# source/read/readtree.cpp
# source/read/splitmatrix.cpp
# source/read/treereader.cpp
# source/refchimeratest.cpp
# source/seqnoise.cpp
source/settings.cpp
# source/sharedutilities.cpp
# source/singlelinkage.cpp
# source/slibshuff.cpp
# source/subsample.cpp
# source/svm/svm.cpp
# source/trialSwap2.cpp
# source/trimoligos.cpp
source/utility.cpp
# source/validcalculator.cpp
# source/validparameter.cpp
# source/venn.cpp
# source/weightedlinkage.cpp
# source/wilcox.cpp
source/calcsparcc.h
source/calculators/ace.h
source/calculators/bergerparker.h
source/calculators/boneh.h
source/calculators/bootstrap.h
source/calculators/bstick.h
source/calculators/calculator.h
source/calculators/canberra.h
source/calculators/chao1.h
source/calculators/coverage.h
source/calculators/dayhoff.h
source/calculators/dist.h
source/calculators/eachgapdist.h
source/calculators/eachgapdistignorens.h
source/calculators/eachgapignore.h
source/calculators/efron.h
source/calculators/filters.h
source/calculators/geom.h
source/calculators/goodscoverage.h
source/calculators/gower.h
source/calculators/hamming.h
source/calculators/heip.h
source/calculators/hellinger.h
source/calculators/ignoregaps.h
source/calculators/invsimpson.h
source/calculators/jackknife.h
source/calculators/logsd.h
source/calculators/manhattan.h
source/calculators/memchi2.h
source/calculators/memchord.h
source/calculators/memeuclidean.h
source/calculators/mempearson.h
source/calculators/npshannon.h
source/calculators/nseqs.h
source/calculators/odum.h
source/calculators/onegapdist.h
source/calculators/onegapignore.h
source/calculators/parsimony.h
source/calculators/prng.h
source/calculators/qstat.h
source/calculators/shannon.h
source/calculators/shannoneven.h
source/calculators/shannonrange.h
source/calculators/sharedace.h
source/calculators/sharedanderbergs.h
source/calculators/sharedbraycurtis.h
source/calculators/sharedchao1.h
source/calculators/sharedjabund.h
source/calculators/sharedjackknife.h
source/calculators/sharedjclass.h
source/calculators/sharedjest.h
source/calculators/sharedjsd.h
source/calculators/sharedkstest.h
source/calculators/sharedkulczynski.h
source/calculators/sharedkulczynskicody.h
source/calculators/sharedlennon.h
source/calculators/sharedmarczewski.h
source/calculators/sharedmorisitahorn.h
source/calculators/sharednseqs.h
source/calculators/sharedochiai.h
source/calculators/sharedrjsd.h
source/calculators/sharedsobs.h
source/calculators/sharedsobscollectsummary.h
source/calculators/sharedsorabund.h
source/calculators/sharedsorclass.h
source/calculators/sharedsorest.h
source/calculators/sharedthetan.h
source/calculators/sharedthetayc.h
source/calculators/shen.h
source/calculators/simpson.h
source/calculators/simpsoneven.h
source/calculators/smithwilson.h
source/calculators/sobs.h
source/calculators/soergel.h
source/calculators/solow.h
source/calculators/spearman.h
source/calculators/speciesprofile.h
source/calculators/structchi2.h
source/calculators/structchord.h
source/calculators/structeuclidean.h
source/calculators/structkulczynski.h
source/calculators/structpearson.h
source/calculators/treecalculator.h
source/calculators/unweighted.h
source/calculators/uvest.h
source/calculators/weighted.h
source/calculators/whittaker.h
source/chimera/bellerophon.h
source/chimera/ccode.h
source/chimera/chimera.h
source/chimera/chimeracheckrdp.h
source/chimera/chimerarealigner.h
source/chimera/chimeraslayer.h
source/chimera/decalc.h
source/chimera/maligner.h
source/chimera/myPerseus.h
source/chimera/pintail.h
source/chimera/slayer.h
source/classifier/alignnode.h
source/classifier/aligntree.h
source/classifier/bayesian.h
source/classifier/classify.h
source/classifier/kmernode.h
source/classifier/kmertree.h
source/classifier/knn.h
source/classifier/phylosummary.h
source/classifier/phylotree.h
source/classifier/taxonomyequalizer.h
source/classifier/taxonomynode.h
source/clearcut/clearcut.h
source/clearcut/cmdargs.h
source/clearcut/common.h
source/clearcut/distclearcut.h
source/clearcut/dmat.h
source/clearcut/fasta.h
source/clearcut/getopt_long.h
source/cluster.hpp
source/clusterclassic.h
source/collect.h
source/collectdisplay.h
source/collectorscurvedata.h
source/commandfactory.hpp
source/commandmaker.h
source/commandoptionparser.hpp
source/commandparameter.h
source/commandparameters/booleanparameter.h
source/commandparameters/commandparameterbase.h
source/commandparameters/commandparametercollection.h
source/commandparameters/countfileparameter.h
source/commandparameters/directoryparameter.h
source/commandparameters/fastafileparameter.h
source/commandparameters/groupfileparameter.h
source/commandparameters/inputdirectoryparameter.h
source/commandparameters/inputtypeparameter.h
source/commandparameters/integerparameter.h
source/commandparameters/multipleparameter.h
source/commandparameters/namefileparameter.h
source/commandparameters/numberparameter.h
source/commandparameters/outputdirectoryparameter.h
source/commandparameters/outputfileparameter.h
source/commandparameters/parametertypes.h
source/commandparameters/seedparameter.h
source/commandparameters/stringparameter.h
source/commands/aligncommand.h
source/commands/amovacommand.h
source/commands/anosimcommand.h
source/commands/binsequencecommand.h
source/commands/biominfocommand.h
source/commands/catchallcommand.h
source/commands/chimerabellerophoncommand.h
source/commands/chimeraccodecommand.h
source/commands/chimeracheckcommand.h
source/commands/chimeraperseuscommand.h
source/commands/chimerapintailcommand.h
source/commands/chimeraslayercommand.h
source/commands/chimerauchimecommand.h
source/commands/chopseqscommand.h
source/commands/classifyotucommand.h
source/commands/classifyrfsharedcommand.h
source/commands/classifyseqscommand.h
source/commands/classifysvmsharedcommand.h
source/commands/classifytreecommand.h
source/commands/clearcutcommand.h
source/commands/clearmemorycommand.h
source/commands/clustercommand.h
source/commands/clusterdoturcommand.h
source/commands/clusterfragmentscommand.h
source/commands/clustersplitcommand.h
source/commands/collectcommand.h
source/commands/collectsharedcommand.h
source/commands/command.hpp
source/commands/consensusseqscommand.h
source/commands/cooccurrencecommand.h
source/commands/corraxescommand.h
source/commands/countgroupscommand.h
source/commands/countseqscommand.h
source/commands/createdatabasecommand.h
source/commands/deconvolutecommand.h
source/commands/degapseqscommand.h
source/commands/deuniqueseqscommand.h
source/commands/deuniquetreecommand.h
source/commands/distancecommand.h
source/commands/filterseqscommand.h
source/commands/filtersharedcommand.h
source/commands/getcommandinfocommand.h
source/commands/getcoremicrobiomecommand.h
source/commands/getcurrentcommand.h
source/commands/getdistscommand.h
source/commands/getgroupcommand.h
source/commands/getgroupscommand.h
source/commands/getlabelcommand.h
source/commands/getlineagecommand.h
source/commands/getlistcountcommand.h
source/commands/getmetacommunitycommand.h
source/commands/getmimarkspackagecommand.h
source/commands/getotulabelscommand.h
source/commands/getoturepcommand.h
source/commands/getotuscommand.h
source/commands/getrabundcommand.h
source/commands/getrelabundcommand.h
source/commands/getsabundcommand.h
source/commands/getseqscommand.h
source/commands/getsharedotucommand.h
source/commands/hclustercommand.h
source/commands/heatmapcommand.h
source/commands/heatmapsimcommand.h
source/commands/helpcommand.h
source/commands/homovacommand.h
source/commands/indicatorcommand.h
source/commands/kruskalwalliscommand.h
source/commands/lefsecommand.h
source/commands/libshuffcommand.h
source/commands/listotulabelscommand.h
source/commands/listseqscommand.h
source/commands/loadlogfilecommand.h
source/commands/makebiomcommand.h
source/commands/makecontigscommand.h
source/commands/makefastqcommand.h
source/commands/makefilecommand.h
source/commands/makegroupcommand.h
source/commands/makelefsecommand.h
source/commands/makelookupcommand.h
source/commands/mantelcommand.h
source/commands/matrixoutputcommand.h
source/commands/mergefilecommand.h
source/commands/mergegroupscommand.h
source/commands/mergesfffilecommand.h
source/commands/mergetaxsummarycommand.h
source/commands/metastatscommand.h
source/commands/mgclustercommand.h
source/commands/mimarksattributescommand.h
source/commands/newcommandtemplate.h
source/commands/nmdscommand.h
source/commands/nocommands.h
source/commands/normalizesharedcommand.h
source/commands/otuassociationcommand.h
source/commands/otuhierarchycommand.h
source/commands/pairwiseseqscommand.h
source/commands/parsefastaqcommand.h
source/commands/parselistscommand.h
source/commands/parsimonycommand.h
source/commands/pcacommand.h
source/commands/pcoacommand.h
source/commands/pcrseqscommand.h
source/commands/phylodiversitycommand.h
source/commands/phylotypecommand.h
source/commands/pipelinepdscommand.h
source/commands/preclustercommand.h
source/commands/primerdesigncommand.h
source/commands/quitcommand.h
source/commands/rarefactcommand.h
source/commands/rarefactsharedcommand.h
source/commands/removedistscommand.h
source/commands/removegroupscommand.h
source/commands/removelineagecommand.h
source/commands/removeotulabelscommand.h
source/commands/removeotuscommand.h
source/commands/removerarecommand.h
source/commands/removeseqscommand.h
source/commands/renameseqscommand.h
source/commands/reversecommand.h
source/commands/screenseqscommand.h
source/commands/secondarystructurecommand.h
source/commands/sensspeccommand.h
source/commands/seqerrorcommand.h
source/commands/seqsummarycommand.h
source/commands/setcurrentcommand.h
source/commands/setdircommand.h
source/commands/setlogfilecommand.h
source/commands/setseedcommand.h
source/commands/sffinfocommand.h
source/commands/sffmultiplecommand.h
source/commands/sharedcommand.h
source/commands/shhhercommand.h
source/commands/shhhseqscommand.h
source/commands/sortseqscommand.h
source/commands/sparcccommand.h
source/commands/splitabundcommand.h
source/commands/splitgroupscommand.h
source/commands/sracommand.h
source/commands/subsamplecommand.h
source/commands/summarycommand.h
source/commands/summaryqualcommand.h
source/commands/summarysharedcommand.h
source/commands/summarytaxcommand.h
source/commands/systemcommand.h
source/commands/treegroupscommand.h
source/commands/trimflowscommand.h
source/commands/trimseqscommand.h
source/commands/unifracunweightedcommand.h
source/commands/unifracweightedcommand.h
source/commands/venncommand.h
source/communitytype/communitytype.h
source/communitytype/kmeans.h
source/communitytype/pam.h
source/communitytype/qFinderDMM.h
source/datastructures/groupmap.h
source/consensus.h
source/display.h
source/dlibshuff.h
source/endiannessmacros.h
source/engine.hpp
source/filehandling/file.h
source/filehandling/summaryfileread.h
source/filehandling/summaryfilewrite.h
source/fileoutput.h
source/gotohoverlap.hpp
source/hcluster.h
source/heatmap.h
source/heatmapsim.h
source/inputdata.h
source/libshuff.h
source/linearalgebra.h
source/logsinks.h
source/metastats/mothurfisher.h
source/metastats/mothurmetastats.h
source/mothur.h
source/mothurConfig.h
source/mothurConfig.h.in
source/mothurout.h
source/myseqdist.h
source/nast.hpp
source/nastreport.hpp
source/needlemanoverlap.hpp
source/noalign.hpp
source/observable.h
source/optionparser.h
source/overlap.hpp
source/progress.hpp
source/randomforest/abstractdecisiontree.hpp
source/randomforest/abstractrandomforest.hpp
source/randomforest/decisiontree.hpp
source/randomforest/forest.h
source/randomforest/macros.h
source/randomforest/randomforest.hpp
source/randomforest/regularizeddecisiontree.h
source/randomforest/regularizedrandomforest.h
source/randomforest/rftreenode.hpp
source/randomnumber.h
source/rarecalc.h
source/raredisplay.h
source/rarefact.h
source/rarefactioncurvedata.h
source/read/formatcolumn.h
source/read/formatmatrix.h
source/read/formatphylip.h
source/read/readblast.h
source/read/readcluster.h
source/read/readcolumn.h
source/read/readmatrix.hpp
source/read/readphylip.h
source/read/readphylipvector.h
source/read/readtree.h
source/read/splitmatrix.h
source/read/treereader.h
source/refchimeratest.h
source/seqnoise.h
source/settings.h
source/sharedutilities.h
source/slibshuff.h
source/subsample.h
source/svm/svm.hpp
source/trialswap2.h
source/trimoligos.h
source/utility.h
source/validcalculator.h
source/validparameter.h
source/venn.h
source/wilcox.h
)

include_directories(
  source
  source/calculators
  source/chimera
  source/classifier
  source/clearcut
  source/commands
  source/commandparameters
  source/communitytype
  source/datastructures
  source/g3log
  source/metastats
  source/randomforest
  source/read
  source/svm
)

find_package(Threads REQUIRED)
target_link_libraries(mothur ${CMAKE_THREAD_LIBS_INIT})

if (USE_READLINE)
  if (WIN32)
	message(STATUS "Using wineditline on Win32")
	add_subdirectory(source/wineditline)
	target_include_directories(mothur PRIVATE "${CMAKE_SOURCE_DIR}/source/wineditline/src")
	target_include_directories(mothur PRIVATE "${CMAKE_BINARY_DIR}/source/wineditline")
	target_link_libraries(mothur edit_static)
	target_compile_definitions(mothur PRIVATE -DUSE_EDITLINE)
  else (WIN32)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/source/cmake/Modules")
	find_package(Readline)
	if (READLINE_FOUND)
	  target_include_directories(mothur PRIVATE ${READLINE_INCLUDE_DIR})
	  target_link_libraries(mothur ${READLINE_LIBRARIES})
	  target_compile_definitions(mothur PRIVATE -DUSE_READLINE)
	else (READLINE_FOUND)
	  message(STATUS "Readline not found")
	endif (READLINE_FOUND)
  endif (WIN32)
endif (USE_READLINE)

if (USE_BOOST)
  find_package(Boost 1.50.0
	COMPONENTS iostreams)
  find_package(ZLIB)
  if (Boost_FOUND AND ZLIB_FOUND)
	target_include_directories(mothur PRIVATE ${Boost_INCLUDE_DIRS})
	target_include_directories(mothur PRIVATE ${ZLIB_INCLUDE_DIRS})
	target_link_libraries(mothur ${Boost_LIBRARIES})
	target_link_libraries(mothur ${ZLIB_LIBRARIES})
	target_compile_definitions(mothur PRIVATE -DUSE_BOOST)
  endif (Boost_FOUND AND ZLIB_FOUND)
endif (USE_BOOST)

if (USE_COMPRESSION)
  target_compile_definitions(mothur PRIVATE -DUSE_COMPRESSION)
endif (USE_COMPRESSION)

if (MOTHUR_FILES)
  target_compile_definitions(mothur PRIVATE -DMOTHUR_FILES="${MOTHUR_FILES}")
endif (MOTHUR_FILES)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/source/mothurConfig.h.in"
  "${PROJECT_BINARY_DIR}/source/mothurConfig.h"
  )

add_executable(uchime
  source/uchime_src/addtargets2.cpp
  source/uchime_src/alignchime.cpp
  source/uchime_src/alignchimel.cpp
  source/uchime_src/alnparams.cpp
  source/uchime_src/alpha.cpp
  source/uchime_src/alpha2.cpp
  source/uchime_src/fractid.cpp
  source/uchime_src/getparents.cpp
  source/uchime_src/globalalign2.cpp
  source/uchime_src/make3way.cpp
  source/uchime_src/mx.cpp
  source/uchime_src/myutils.cpp
  source/uchime_src/path.cpp
  source/uchime_src/searchchime.cpp
  source/uchime_src/seqdb.cpp
  source/uchime_src/setnucmx.cpp
  source/uchime_src/sfasta.cpp
  source/uchime_src/tracebackbit.cpp
  source/uchime_src/uchime_main.cpp
  source/uchime_src/usort.cpp
  source/uchime_src/viterbifast.cpp
  source/uchime_src/writechhit.cpp
)
target_compile_definitions(uchime PRIVATE "-DUCHIMES=1")

source_group("Header Files" REGULAR_EXPRESSION "\\.h(pp)?$")

# add the install targets
install(TARGETS mothur DESTINATION bin)
install(TARGETS uchime DESTINATION bin)
 
# does the application run
add_test(mothurVersion mothur --version)
set_tests_properties(mothurVersion
  PROPERTIES
  PASS_REGULAR_EXPRESSION "Mothur version=${MOTHUR_VERSION}"
)
add_test(mothurHelp mothur --help)
set_tests_properties(mothurHelp
  PROPERTIES
  PASS_REGULAR_EXPRESSION "Valid commands are"
)

add_test(uchimeUsage uchime)
set_tests_properties(uchimeUsage
  PROPERTIES
  PASS_REGULAR_EXPRESSION "Usage"
)

add_subdirectory(source/g3log)
target_link_libraries(mothur g3logger)
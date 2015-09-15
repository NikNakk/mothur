cmake_minimum_required (VERSION 2.8)
project (mothur)
 
# The version number.
set (MOTHUR_VERSION 1.36.0)
set (MOTHUR_RELEASE_DATE 7/23/2015)

option(USE_MPI 
  "Use MPI" OFF)
option(USE_READLINE 
  "Use readline library" ON)
option(USE_BOOST
  "Use boost library" ON)
option(USE_COMPRESSION
  "Use compression (may only work on *nix systems)" OFF)
set(MOTHUR_FILES ""
  CACHE FILEPATH "Default path for mothur files")
  
enable_testing()

include_directories ("${PROJECT_BINARY_DIR}/source")

add_executable(mothur
  source/calculators/ace.cpp
  source/calculators/bergerparker.cpp
  source/calculators/boneh.cpp
  source/calculators/bootstrap.cpp
  source/calculators/bstick.cpp
  source/calculators/calculator.cpp
  source/calculators/canberra.cpp
  source/calculators/chao1.cpp
  source/calculators/coverage.cpp
  source/calculators/efron.cpp
  source/calculators/geom.cpp
  source/calculators/goodscoverage.cpp
  source/calculators/gower.cpp
  source/calculators/hamming.cpp
  source/calculators/heip.cpp
  source/calculators/hellinger.cpp
  source/calculators/invsimpson.cpp
  source/calculators/jackknife.cpp
  source/calculators/logsd.cpp
  source/calculators/manhattan.cpp
  source/calculators/memchi2.cpp
  source/calculators/memchord.cpp
  source/calculators/memeuclidean.cpp
  source/calculators/mempearson.cpp
  source/calculators/npshannon.cpp
  source/calculators/odum.cpp
  source/calculators/parsimony.cpp
  source/calculators/prng.cpp
  source/calculators/qstat.cpp
  source/calculators/shannon.cpp
  source/calculators/shannoneven.cpp
  source/calculators/shannonrange.cpp
  source/calculators/sharedace.cpp
  source/calculators/sharedanderbergs.cpp
  source/calculators/sharedbraycurtis.cpp
  source/calculators/sharedchao1.cpp
  source/calculators/sharedjabund.cpp
  source/calculators/sharedjackknife.cpp
  source/calculators/sharedjclass.cpp
  source/calculators/sharedjest.cpp
  source/calculators/sharedjsd.cpp
  source/calculators/sharedkstest.cpp
  source/calculators/sharedkulczynski.cpp
  source/calculators/sharedkulczynskicody.cpp
  source/calculators/sharedlennon.cpp
  source/calculators/sharedmarczewski.cpp
  source/calculators/sharedmorisitahorn.cpp
  source/calculators/sharedochiai.cpp
  source/calculators/sharedrjsd.cpp
  source/calculators/sharedsobs.cpp
  source/calculators/sharedsobscollectsummary.cpp
  source/calculators/sharedsorabund.cpp
  source/calculators/sharedsorclass.cpp
  source/calculators/sharedsorest.cpp
  source/calculators/sharedthetan.cpp
  source/calculators/sharedthetayc.cpp
  source/calculators/shen.cpp
  source/calculators/simpson.cpp
  source/calculators/simpsoneven.cpp
  source/calculators/smithwilson.cpp
  source/calculators/soergel.cpp
  source/calculators/solow.cpp
  source/calculators/spearman.cpp
  source/calculators/speciesprofile.cpp
  source/calculators/structchi2.cpp
  source/calculators/structchord.cpp
  source/calculators/structeuclidean.cpp
  source/calculators/structkulczynski.cpp
  source/calculators/structpearson.cpp
  source/calculators/unweighted.cpp
  source/calculators/uvest.cpp
  source/calculators/weighted.cpp
  source/calculators/whittaker.cpp
  source/chimera/bellerophon.cpp
  source/chimera/ccode.cpp
  source/chimera/chimera.cpp
  source/chimera/chimeracheckrdp.cpp
  source/chimera/chimerarealigner.cpp
  source/chimera/chimeraslayer.cpp
  source/chimera/decalc.cpp
  source/chimera/maligner.cpp
  source/chimera/myPerseus.cpp
  source/chimera/pintail.cpp
  source/chimera/slayer.cpp
  source/classifier/alignnode.cpp
  source/classifier/aligntree.cpp
  source/classifier/bayesian.cpp
  source/classifier/classify.cpp
  source/classifier/kmernode.cpp
  source/classifier/kmertree.cpp
  source/classifier/knn.cpp
  source/classifier/phylosummary.cpp
  source/classifier/phylotree.cpp
  source/classifier/taxonomyequalizer.cpp
  source/classifier/taxonomynode.cpp
  source/clearcut/clearcut.cpp
  source/clearcut/cmdargs.cpp
  source/clearcut/distclearcut.cpp
  source/clearcut/dmat.cpp
  source/clearcut/fasta.cpp
  source/clearcut/getopt_long.cpp
  source/commands/aligncommand.cpp
  source/commands/amovacommand.cpp
  source/commands/anosimcommand.cpp
  source/commands/binsequencecommand.cpp
  source/commands/catchallcommand.cpp
  source/commands/chimerabellerophoncommand.cpp
  source/commands/chimeraccodecommand.cpp
  source/commands/chimeracheckcommand.cpp
  source/commands/chimeraperseuscommand.cpp
  source/commands/chimerapintailcommand.cpp
  source/commands/chimeraslayercommand.cpp
  source/commands/chimerauchimecommand.cpp
  source/commands/chopseqscommand.cpp
  source/commands/classifyotucommand.cpp
  source/commands/classifyrfsharedcommand.cpp
  source/commands/classifyseqscommand.cpp
  source/commands/classifysvmsharedcommand.cpp
  source/commands/classifytreecommand.cpp
  source/commands/clearcutcommand.cpp
  source/commands/clearmemorycommand.cpp
  source/commands/clustercommand.cpp
  source/commands/clusterdoturcommand.cpp
  source/commands/clusterfragmentscommand.cpp
  source/commands/clustersplitcommand.cpp
  source/commands/collectcommand.cpp
  source/commands/collectsharedcommand.cpp
  source/commands/consensusseqscommand.cpp
  source/commands/cooccurrencecommand.cpp
  source/commands/corraxescommand.cpp
  source/commands/countgroupscommand.cpp
  source/commands/countseqscommand.cpp
  source/commands/createdatabasecommand.cpp
  source/commands/deconvolutecommand.cpp
  source/commands/degapseqscommand.cpp
  source/commands/deuniqueseqscommand.cpp
  source/commands/deuniquetreecommand.cpp
  source/commands/distancecommand.cpp
  source/commands/filterseqscommand.cpp
  source/commands/filtersharedcommand.cpp
  source/commands/getcommandinfocommand.cpp
  source/commands/getcoremicrobiomecommand.cpp
  source/commands/getcurrentcommand.cpp
  source/commands/getdistscommand.cpp
  source/commands/getgroupcommand.cpp
  source/commands/getgroupscommand.cpp
  source/commands/getlabelcommand.cpp
  source/commands/getlineagecommand.cpp
  source/commands/getlistcountcommand.cpp
  source/commands/getmetacommunitycommand.cpp
  source/commands/getmimarkspackagecommand.cpp
  source/commands/getotulabelscommand.cpp
  source/commands/getoturepcommand.cpp
  source/commands/getotuscommand.cpp
  source/commands/getrabundcommand.cpp
  source/commands/getrelabundcommand.cpp
  source/commands/getsabundcommand.cpp
  source/commands/getseqscommand.cpp
  source/commands/getsharedotucommand.cpp
  source/commands/hclustercommand.cpp
  source/commands/heatmapcommand.cpp
  source/commands/heatmapsimcommand.cpp
  source/commands/helpcommand.cpp
  source/commands/homovacommand.cpp
  source/commands/indicatorcommand.cpp
  source/commands/kruskalwalliscommand.cpp
  source/commands/lefsecommand.cpp
  source/commands/libshuffcommand.cpp
  source/commands/listotulabelscommand.cpp
  source/commands/listseqscommand.cpp
  source/commands/loadlogfilecommand.cpp
  source/commands/makebiomcommand.cpp
  source/commands/makecontigscommand.cpp
  source/commands/makefastqcommand.cpp
  source/commands/makefilecommand.cpp
  source/commands/makegroupcommand.cpp
  source/commands/makelefsecommand.cpp
  source/commands/makelookupcommand.cpp
  source/commands/mantelcommand.cpp
  source/commands/matrixoutputcommand.cpp
  source/commands/mergefilecommand.cpp
  source/commands/mergegroupscommand.cpp
  source/commands/mergesfffilecommand.cpp
  source/commands/mergetaxsummarycommand.cpp
  source/commands/metastatscommand.cpp
  source/commands/mgclustercommand.cpp
  source/commands/mimarksattributescommand.cpp
  source/commands/newcommandtemplate.cpp
  source/commands/nmdscommand.cpp
  source/commands/nocommands.cpp
  source/commands/normalizesharedcommand.cpp
  source/commands/otuassociationcommand.cpp
  source/commands/otuhierarchycommand.cpp
  source/commands/pairwiseseqscommand.cpp
  source/commands/parsefastaqcommand.cpp
  source/commands/parselistscommand.cpp
  source/commands/parsimonycommand.cpp
  source/commands/pcacommand.cpp
  source/commands/pcoacommand.cpp
  source/commands/pcrseqscommand.cpp
  source/commands/phylodiversitycommand.cpp
  source/commands/phylotypecommand.cpp
  source/commands/pipelinepdscommand.cpp
  source/commands/preclustercommand.cpp
  source/commands/primerdesigncommand.cpp
  source/commands/quitcommand.cpp
  source/commands/rarefactcommand.cpp
  source/commands/rarefactsharedcommand.cpp
  source/commands/removedistscommand.cpp
  source/commands/removegroupscommand.cpp
  source/commands/removelineagecommand.cpp
  source/commands/removeotulabelscommand.cpp
  source/commands/removeotuscommand.cpp
  source/commands/removerarecommand.cpp
  source/commands/removeseqscommand.cpp
  source/commands/renameseqscommand.cpp
  source/commands/reversecommand.cpp
  source/commands/screenseqscommand.cpp
  source/commands/secondarystructurecommand.cpp
  source/commands/sensspeccommand.cpp
  source/commands/seqerrorcommand.cpp
  source/commands/seqsummarycommand.cpp
  source/commands/setcurrentcommand.cpp
  source/commands/setdircommand.cpp
  source/commands/setlogfilecommand.cpp
  source/commands/setseedcommand.cpp
  source/commands/sffinfocommand.cpp
  source/commands/sffmultiplecommand.cpp
  source/commands/sharedcommand.cpp
  source/commands/shhhercommand.cpp
  source/commands/shhhseqscommand.cpp
  source/commands/sortseqscommand.cpp
  source/commands/sparcccommand.cpp
  source/commands/splitabundcommand.cpp
  source/commands/splitgroupscommand.cpp
  source/commands/sracommand.cpp
  source/commands/subsamplecommand.cpp
  source/commands/summarycommand.cpp
  source/commands/summaryqualcommand.cpp
  source/commands/summarysharedcommand.cpp
  source/commands/summarytaxcommand.cpp
  source/commands/systemcommand.cpp
  source/commands/treegroupscommand.cpp
  source/commands/trimflowscommand.cpp
  source/commands/trimseqscommand.cpp
  source/commands/unifracunweightedcommand.cpp
  source/commands/unifracweightedcommand.cpp
  source/commands/venncommand.cpp
  source/communitytype/communitytype.cpp
  source/communitytype/kmeans.cpp
  source/communitytype/pam.cpp
  source/communitytype/qFinderDMM.cpp
  source/datastructures/alignment.cpp
  source/datastructures/alignmentcell.cpp
  source/datastructures/alignmentdb.cpp
  source/datastructures/blastalign.cpp
  source/datastructures/blastdb.cpp
  source/datastructures/counttable.cpp
  source/datastructures/database.cpp
  source/datastructures/designmap.cpp
  source/datastructures/distancedb.cpp
  source/datastructures/fastamap.cpp
  source/datastructures/fastqread.cpp
  source/datastructures/flowdata.cpp
  source/datastructures/fullmatrix.cpp
  source/datastructures/groupmap.cpp
  source/datastructures/kmer.cpp
  source/datastructures/kmeralign.cpp
  source/datastructures/kmerdb.cpp
  source/datastructures/listvector.cpp
  source/datastructures/nameassignment.cpp
  source/datastructures/oligos.cpp
  source/datastructures/ordervector.cpp
  source/datastructures/qualityscores.cpp
  source/datastructures/rabundvector.cpp
  source/datastructures/referencedb.cpp
  source/datastructures/reportfile.cpp
  source/datastructures/sabundvector.cpp
  source/datastructures/sequence.cpp
  source/datastructures/sequencecountparser.cpp
  source/datastructures/sequencedb.cpp
  source/datastructures/sequenceparser.cpp
  source/datastructures/sharedlistvector.cpp
  source/datastructures/sharedordervector.cpp
  source/datastructures/sharedrabundfloatvector.cpp
  source/datastructures/sharedrabundvector.cpp
  source/datastructures/sharedsabundvector.cpp
  source/datastructures/sparsedistancematrix.cpp
  source/datastructures/sparsematrix.cpp
  source/datastructures/suffixdb.cpp
  source/datastructures/suffixnodes.cpp
  source/datastructures/suffixtree.cpp
  source/datastructures/tree.cpp
  source/datastructures/treemap.cpp
  source/datastructures/treenode.cpp
  source/metastats/mothurfisher.cpp
  source/metastats/mothurmetastats.cpp
  source/randomforest/abstractdecisiontree.cpp
  source/randomforest/abstractrandomforest.cpp
  source/randomforest/decisiontree.cpp
  source/randomforest/forest.cpp
  source/randomforest/randomforest.cpp
  source/randomforest/regularizeddecisiontree.cpp
  source/randomforest/regularizedrandomforest.cpp
  source/randomforest/rftreenode.cpp
  source/read/formatcolumn.cpp
  source/read/formatphylip.cpp
  source/read/readblast.cpp
  source/read/readcluster.cpp
  source/read/readcolumn.cpp
  source/read/readphylip.cpp
  source/read/readphylipvector.cpp
  source/read/readtree.cpp
  source/read/splitmatrix.cpp
  source/read/treereader.cpp
  source/svm/svm.cpp
  source/averagelinkage.cpp
  source/calcsparcc.cpp
  source/cluster.cpp
  source/clusterclassic.cpp
  source/collect.cpp
  source/commandfactory.cpp
  source/commandoptionparser.cpp
  source/completelinkage.cpp
  source/consensus.cpp
  source/dlibshuff.cpp
  source/engine.cpp
  source/fileoutput.cpp
  source/gotohoverlap.cpp
  source/hcluster.cpp
  source/heatmap.cpp
  source/heatmapsim.cpp
  source/inputdata.cpp
  source/libshuff.cpp
  source/linearalgebra.cpp
  source/mothur.cpp
  source/mothurout.cpp
  source/myseqdist.cpp
  source/nast.cpp
  source/nastreport.cpp
  source/needlemanoverlap.cpp
  source/noalign.cpp
  source/optionparser.cpp
  source/overlap.cpp
  source/progress.cpp
  source/randomnumber.cpp
  source/rarecalc.cpp
  source/raredisplay.cpp
  source/rarefact.cpp
  source/refchimeratest.cpp
  source/seqnoise.cpp
  source/sharedutilities.cpp
  source/singlelinkage.cpp
  source/slibshuff.cpp
  source/subsample.cpp
  source/trialSwap2.cpp
  source/trimoligos.cpp
  source/validcalculator.cpp
  source/validparameter.cpp
  source/venn.cpp
  source/weightedlinkage.cpp
  source/wilcox.cpp
)

include_directories(
  source
  source/calculators
  source/chimera
  source/classifier
  source/clearcut
  source/commands
  source/communitytype
  source/datastructures
  source/metastats
  source/randomforest
  source/read
  source/svm
)

if (USE_READLINE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/source/cmake/Modules")
  find_package(Readline)
  if (READLINE_FOUND)
    target_include_directories(mothur PRIVATE ${READLINE_INCLUDE_DIR})
    target_link_libraries(mothur ${READLINE_LIBRARIES})
    target_compile_definitions(mothur PRIVATE -DUSE_READLINE)
  else (READLINE_FOUND)
    message(STATUS "Readline not found")
  endif (READLINE_FOUND)
endif (USE_READLINE)

if (USE_MPI)
  find_package(MPI)
  if (MPI_FOUND)
    target_include_directories(mothur PRIVATE ${MPI_INCLUDE_PATH})
    target_link_libraries(mothur ${MPI_LIBRARIES})
    target_compile_definitions(mothur PRIVATE -DUSE_MPI)
    if(MPI_COMPILE_FLAGS)
      set_target_properties(mothur PROPERTIES
        COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()
    if(MPI_LINK_FLAGS)
      set_target_properties(mothur PROPERTIES
        LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()
  endif (MPI_FOUND)
endif (USE_MPI)

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
//
//  sparsedistancematrix.cpp
//  Mothur
//
//  Created by Sarah Westcott on 7/16/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "sparsedistancematrix.h"


/***********************************************************************/

SparseDistanceMatrix::SparseDistanceMatrix() : numNodes(0), smallDist(1e6) { sorted = false; aboveCutoff = 1e6; }

/***********************************************************************/

int SparseDistanceMatrix::getNNodes() {
	return numNodes;
}
/***********************************************************************/

void SparseDistanceMatrix::clear() {
	for (int i = 0; i < seqVec.size(); i++) { seqVec[i].clear(); }
	seqVec.clear();
}

/***********************************************************************/

float SparseDistanceMatrix::getSmallDist() {
	return smallDist;
}
/***********************************************************************/

int SparseDistanceMatrix::updateCellCompliment(ull row, ull col) {

	ull vrow = seqVec[row][col].index;
	ull vcol = 0;

	//find the columns entry for this cell as well
	for (int i = 0; i < seqVec[vrow].size(); i++) {
		if (seqVec[vrow][i].index == row) { vcol = i;  break; }
	}

	seqVec[vrow][vcol].dist = seqVec[row][col].dist;

	return 0;
}
/***********************************************************************/

int SparseDistanceMatrix::rmCell(ull row, ull col) {
	numNodes -= 2;

	ull vrow = seqVec[row][col].index;
	ull vcol = 0;

	//find the columns entry for this cell as well
	for (int i = 0; i < seqVec[vrow].size(); i++) { if (seqVec[vrow][i].index == row) { vcol = i;  break; } }

	seqVec[vrow].erase(seqVec[vrow].begin() + vcol);
	seqVec[row].erase(seqVec[row].begin() + col);

	return(0);
}
/***********************************************************************/
void SparseDistanceMatrix::addCell(ull row, PDistCell cell) {
	numNodes += 2;
	if (cell.dist < smallDist) { smallDist = cell.dist; }

	seqVec[row].push_back(cell);
	PDistCell temp(row, cell.dist);
	seqVec[cell.index].push_back(temp);
}
/***********************************************************************/
int SparseDistanceMatrix::addCellSorted(ull row, PDistCell cell) {
	numNodes += 2;
	if (cell.dist < smallDist) { smallDist = cell.dist; }

	seqVec[row].push_back(cell);
	PDistCell temp(row, cell.dist);
	seqVec[cell.index].push_back(temp);

	sortSeqVec(row);
	sortSeqVec(cell.index);

	int location = -1; //find location of new cell when sorted
	for (int i = 0; i < seqVec[row].size(); i++) { if (seqVec[row][i].index == cell.index) { location = i; break; } }

	return location;
}

/***********************************************************************/

ull SparseDistanceMatrix::getSmallestCell(ull& row) {
	if (!sorted) { sortSeqVec(); sorted = true; }

	vector<PDistCellMin> mins;
	smallDist = 1e6;

	for (int i = 0; i < seqVec.size(); i++) {
		for (int j = 0; j < seqVec[i].size(); j++) {

			if (ctrlc_pressed) { return smallDist; }

			//already checked everyone else in row
			if (i < seqVec[i][j].index) {

				float dist = seqVec[i][j].dist;

				if (dist < smallDist) {  //found a new smallest distance
					mins.clear();
					smallDist = dist;
					PDistCellMin temp(i, seqVec[i][j].index);
					mins.push_back(temp);
				}
				else if (dist == smallDist) {  //if a subsequent distance is the same as mins distance add the new iterator to the mins vector
					PDistCellMin temp(i, seqVec[i][j].index);
					mins.push_back(temp);
				}
			}
			else { j += seqVec[i].size(); } //stop looking 
		}
	}

	random_shuffle(mins.begin(), mins.end());  //randomize the order of the iterators in the mins vector

	row = mins[0].row;
	ull col = mins[0].col;

	return col;
}
/***********************************************************************/

int SparseDistanceMatrix::sortSeqVec() {

	//saves time in getSmallestCell, by making it so you dont search the repeats
	for (int i = 0; i < seqVec.size(); i++) { sort(seqVec[i].begin(), seqVec[i].end(), compareIndexes); }

	return 0;
}
/***********************************************************************/

int SparseDistanceMatrix::sortSeqVec(int index) {

	//saves time in getSmallestCell, by making it so you dont search the repeats
	sort(seqVec[index].begin(), seqVec[index].end(), compareIndexes);

	return 0;
}
/***********************************************************************/


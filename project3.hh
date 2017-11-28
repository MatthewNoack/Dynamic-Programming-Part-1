///////////////////////////////////////////////////////////////////////////////
// maxprotein.hh
//
// Compute the set of foods that maximizes protein, within a calorie budget,
// with the greedy method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

// Simple structure for a single protein
struct Protein {
	Protein() {
		description = "";
		sequence = "";
	}
	Protein(std::string desc, std::string seq) {
		description = desc;
		sequence = seq;
	}
	std::string		description;
	std::string 	sequence;
};

// Alias for a vector of shared pointers to Protein objects.
typedef std::vector<std::shared_ptr<Protein>> ProteinVector;


// -------------------------------------------------------------------------
// Load all the proteins from a standard FASTA format file with one line
// per sequence (multi-line sequences are not allowed).
// Returns false on I/O error.
// -------------------------------------------------------------------------
bool load_proteins(ProteinVector & proteins, const std::string& path) 
{
  //std::cout << "Loading proteins from [" << path << "]" << std::endl;
  proteins.clear();
  std::ifstream ifs(path.c_str());
  if (!ifs.is_open() || !ifs.good()) {
    std::cout << "Failed to open [" << path << "]" << std::endl;
    return false;
  }
  int proteinsLoaded = 0;
  bool have_description = false;
  std::shared_ptr<Protein> newProtein = nullptr;
  while (!ifs.eof()) {
    std::string lineBuffer;
    std::getline(ifs, lineBuffer);
    if (ifs.eof()) {
      break;
    }
    if (lineBuffer.size() == 0) {
		continue;
	}
    if (lineBuffer[0] == '>') {
		newProtein = std::shared_ptr<Protein>(new Protein);
		newProtein->description = lineBuffer.substr(1);
        have_description = true;
    } else if (have_description) {
		newProtein->sequence = lineBuffer;
	    proteins.push_back(newProtein);
        proteinsLoaded++;
        have_description = false;
    }
  }

	ifs.close();
  //std::cout << "Loaded " << proteinsLoaded << " proteins from [" << path << "]" << std::endl;

  return true;
}


// -------------------------------------------------------------------------
int dynamicprogramming_longest_common_subsequence(const std::string & string1, 
												  const std::string & string2)
{
	int n = string1.size();
	int m = string2.size();
	int D[n+1][m+1];
 	int i,j,up,left,diag,max;
	for (i = 0; i < n+1; i++){
		D[i][0] = 0;
	}
	for (j = 0; j < m+1; j++){
		D[0][j] = 0;
	}
	for(i = 1; i < n+1; i++){
		for(j = 1; j < m+1; j++){
			up = D[i-1][j];
			left = D[i][j-1];
			diag = D[i-1][j-1];
			if (string1[i-1] == string2[j-1]){
				diag = diag + 1;
			}				
			D[i][j] = std::max(up, (std::max(left, diag)));
		}
	}
	return D[n][m];
}

// -------------------------------------------------------------------------
std::unique_ptr<std::vector<std::string>>  generate_all_subsequences(const std::string & sequence)
{
	std::unique_ptr<std::vector<std::string>> subsequences(new std::vector<std::string>);
	int n = pow(2,sequence.size());
	int bits,j;	
	for (bits = 0; bits < n; bits++){
		std::string subsequence = "";
		for (j = 0; j < sequence.size(); j++){
			if (((bits >> j) & 1) == 1){
				subsequence = subsequence + sequence[j];
			}
		}		 
		subsequences->push_back(subsequence);
		//std::cout << subsequence << std::endl;
	}
	return subsequences;
}


// -------------------------------------------------------------------------
int exhaustive_longest_common_subsequence(const std::string & string1, 
										  const std::string & string2)
{
	
	std::unique_ptr<std::vector<std::string>> all_subseqs1(new std::vector<std::string>);
	std::unique_ptr<std::vector<std::string>> all_subseqs2(new std::vector<std::string>);
	//finds all subsequences in string1 and string2
	*(all_subseqs1) = *(generate_all_subsequences(string1));
	*(all_subseqs2) = *(generate_all_subsequences(string2));
	int best_score = 0;
	int i,j;
	//comparies every element of all_subseqs1 to all_subseqs2
	for (i = 0; i < all_subseqs1->size(); i++){
		for (j = 0; j < all_subseqs2->size(); j++){
			if ((*(all_subseqs1))[i] == (*(all_subseqs2))[j] && (*(all_subseqs1))[i].size() > best_score){
				best_score = (*(all_subseqs1))[i].size();
			}
		}
	} 
	return best_score;
}


// -------------------------------------------------------------------------
std::shared_ptr<Protein> exhaustive_best_match(ProteinVector & proteins, const std::string & string1)
{
	std::shared_ptr<Protein> best_protein = nullptr;
	int best_score = 0;
	int i,best_i,score;
	for (i = 0; i < proteins.size(); i++){
		score = exhaustive_longest_common_subsequence(proteins[i]->sequence, string1);
		if (score > best_score){
			best_score = score;
			best_i = i;
		}
	}
	return proteins[best_i];
}

// -------------------------------------------------------------------------
std::shared_ptr<Protein> dynamicprogramming_best_match(ProteinVector & proteins, const std::string & string1)
{
	std::shared_ptr<Protein> best_protein = nullptr;
	int best_i = 0;
	int i,score;
	int best_score = 0;

	for (i = 0; i < proteins.size(); i++){
		score = dynamicprogramming_longest_common_subsequence(proteins[i]->sequence, string1);
		if (score > best_score){
			best_score = score;
			best_i = i;
		}
	}
	best_protein = proteins[best_i];
	
	return best_protein;
}



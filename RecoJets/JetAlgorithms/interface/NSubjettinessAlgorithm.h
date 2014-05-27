#ifndef RECOJETS_JETALGORITHMS_NSUBJETTINESSALGORITHM_H
#define RECOJETS_JETALGORITHMS_NSUBJETTINESSALGORITHM_H 1


/*
  Implementation of the N Subjettiness jet shape variable
  which is described in: http://arxiv.org/abs/arXiv:1011.2268
*/


#include <vector>

#include "RecoJets/JetAlgorithms/interface/CompoundPseudoJet.h"
#include "FWCore/Framework/interface/Event.h"

#include <fastjet/PseudoJet.hh>




class NSubjettinessAlgorithm
{
  //
  // construction / destruction
  //
public:
  NSubjettinessAlgorithm(const double rParam_, const int N_min=1, const int N_max=3);
  virtual ~NSubjettinessAlgorithm();
  
  
  //
  // member functions
  //
public:
  void run(const std::vector<CompoundPseudoJet>& fatJets);
  void resetResultVector();
  std::vector<double> getNSubjettiness(const int N);
  std::vector< std::vector<double> > getNSubjettiness();
  size_t getNmin();
  size_t getNmax();
  
  //
  // member data
  //
private:
  std::string								 	moduleLabel_;
  bool										 	verbose_;

  double											rParam;
  size_t 										N_min;
  size_t											N_max;
  
  std::vector< std::vector<double> >	NSubjettiness;
};


#endif

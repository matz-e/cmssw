////////////////////////////////////////////////////////////////////////////////
//
// NSubjettinessAlgorithm
// ---------------------
//
// This algorithm implements the Subjet/Filter jet reconstruction
// which was first proposed here: http://arxiv.org/abs/arXiv:1011.2268
////////////////////////////////////////////////////////////////////////////////


#include "RecoJets/JetAlgorithms/interface/NSubjettinessAlgorithm.h"

#include <cmath>


using namespace std;

////////////////////////////////////////////////////////////////////////////////
// construction / destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
NSubjettinessAlgorithm::NSubjettinessAlgorithm(const double rParam_, const int N_min_, const int N_max_):
			rParam(rParam_), N_min(N_min_), N_max(N_max_)
{
			resetResultVector();
}


//______________________________________________________________________________
NSubjettinessAlgorithm::~NSubjettinessAlgorithm()
{
}
  

////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void NSubjettinessAlgorithm::run(const std::vector<CompoundPseudoJet>& fatJets)
{
	resetResultVector();
	
	vector<CompoundPseudoJet>::const_iterator itFatJet(fatJets.begin());
	vector<CompoundPseudoJet>::const_iterator itFatJetEnd(fatJets.end());
	
	vector<fastjet::PseudoJet> NSubjets;
	vector<fastjet::PseudoJet> Constituents;
	
	for(;itFatJet!=itFatJetEnd;++itFatJet){
		
		Constituents = itFatJet->hardJet().constituents();
		
		vector<fastjet::PseudoJet>::const_iterator itConstituentsBegin(Constituents.begin());
		vector<fastjet::PseudoJet>::const_iterator itConstituentsEnd(Constituents.end());
		vector<fastjet::PseudoJet>::const_iterator itConstituents(itConstituentsBegin);
		
		for(size_t N=N_min;N<=N_max;N++){
			
			if(N>Constituents.size()){
				NSubjettiness[N-N_min].push_back(-1.);
				continue;
			}
			
			NSubjets = itFatJet->hardJet().exclusive_subjets_up_to((int) N);
			
			if(NSubjets.size()<N){
				NSubjettiness[N-N_min].push_back(-1.);
				continue;
			}
			
			double Tau_N = 0;
			double PTSum = 0;
			
			itConstituents = itConstituentsBegin;
			
			for(;itConstituents!=itConstituentsEnd;++itConstituents){
				
				double DeltaRMin = 9999999;
				double DeltaR = 0;
				
				for(size_t iSubjets=0;iSubjets<NSubjets.size();iSubjets++){
			
					DeltaR = itConstituents->delta_R(NSubjets[iSubjets]);
					
					if(DeltaR<DeltaRMin) DeltaRMin = DeltaR;
				}
				
				Tau_N += itConstituents->pt()*DeltaRMin;
				PTSum += itConstituents->pt();
			}
			
			Tau_N *= 1/(rParam*PTSum);
	
			NSubjettiness[N-N_min].push_back(Tau_N);
		}
	}
}


std::vector<double> NSubjettinessAlgorithm::getNSubjettiness(const int N){
	
	return NSubjettiness[N-N_min];

}


std::vector< std::vector<double> > NSubjettinessAlgorithm::getNSubjettiness(){
	
	return NSubjettiness;

}


void NSubjettinessAlgorithm::resetResultVector(){

	NSubjettiness.clear();
	
	for(size_t N=N_min;N<=N_max;N++) NSubjettiness.push_back(std::vector<double>());
}


size_t NSubjettinessAlgorithm::getNmin(){
	
	return N_min;

}


size_t NSubjettinessAlgorithm::getNmax(){
	
	return N_max;

}

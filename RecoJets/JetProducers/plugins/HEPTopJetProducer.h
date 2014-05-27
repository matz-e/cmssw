#ifndef RECOJETS_JETPRODUCERS_HEPTOPJETPRODUCER_H
#define RECOJETS_JETPRODUCERS_HEPTOPJETPRODUCER_H 1







#include "VirtualJetProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "RecoJets/JetAlgorithms/interface/CompoundPseudoJet.h"
#include "RecoJets/JetAlgorithms/interface/HEPTopTaggerAlgorithm.h"
#include "RecoJets/JetAlgorithms/interface/NSubjettinessAlgorithm.h"

class HEPTopJetProducer : public VirtualJetProducer
{
	//
  	// construction / destruction
  	//
	public:
		HEPTopJetProducer(const edm::ParameterSet& ps);
		virtual ~HEPTopJetProducer();
		
		
	//
	// member functions
	//
	public:	
		void produce( edm::Event& iEvent, const edm::EventSetup& iSetup );
		void endJob();
		void runAlgorithm( edm::Event& iEvent, const edm::EventSetup& iSetup );
		void inputTowers();
		void output(edm::Event& iEvent,const edm::EventSetup& iSetup);
		template<class T>
		void writeCompoundJets(edm::Event& iEvent,const edm::EventSetup& iSetup);
		
	//
	// member data
	//
	private:
		HEPTopTaggerAlgorithm				alg_;         		/// The algorithm to do the work
		NSubjettinessAlgorithm				nSubJet_;
		std::vector<CompoundPseudoJet>  	fjCompoundJets_;
		std::vector<bool>					topTags_;
};


#endif

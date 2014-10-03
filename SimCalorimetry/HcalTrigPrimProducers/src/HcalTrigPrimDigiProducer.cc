#include "SimCalorimetry/HcalTrigPrimProducers/src/HcalTrigPrimDigiProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HBHEDataFrame.h"
#include "DataFormats/HcalDigi/interface/HFDataFrame.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CalibFormats/HcalObjects/interface/HcalTPGRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalTPGCoder.h"
#include "CalibFormats/CaloTPG/interface/HcalTPGCompressor.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGRecord.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"
#include "CondFormats/HcalObjects/interface/HcalLutMetadata.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include <algorithm>

HcalTrigPrimDigiProducer::HcalTrigPrimDigiProducer(const edm::ParameterSet& ps)
: 
  theAlgo_(ps.getParameter<bool>("peakFilter"),
	  ps.getParameter<std::vector<double> >("weights"),
	  ps.getParameter<int>("latency"),
	  ps.getParameter<uint32_t>("FG_threshold"),
      ps.getParameter<uint32_t>("ZS_threshold"),
	  ps.getParameter<int>("numberOfSamples"),
	  ps.getParameter<int>("numberOfPresamples"),
      ps.getParameter<uint32_t>("MinSignalThreshold"),
      ps.getParameter<uint32_t>("PMTNoiseThreshold"),
      ps.getParameter<bool>("upgrade")
   ),
  inputLabel_(ps.getParameter<std::vector<edm::InputTag> >("inputLabel")),
  inputTagFEDRaw_(ps.getParameter<edm::InputTag> ("InputTagFEDRaw")),
  runZS_(ps.getParameter<bool>("RunZS")),
  runFrontEndFormatError_(ps.getParameter<bool>("FrontEndFormatError")),
  upgrade_(ps.getParameter<bool>("upgrade"))
{
   produces<HcalTrigPrimDigiCollection>();
   produces<HcalUpgradeTrigPrimDigiCollection>();
   theAlgo_.setPeakFinderAlgorithm(ps.getParameter<int>("PeakFinderAlgorithm"));
}


void HcalTrigPrimDigiProducer::produce(edm::Event& iEvent, const edm::EventSetup& eventSetup) {

  // Step A: get the conditions, for the decoding
  edm::ESHandle<HcalTPGCoder> inputCoder;
  eventSetup.get<HcalTPGRecord>().get(inputCoder);

  edm::ESHandle<CaloTPGTranscoder> outTranscoder;
  eventSetup.get<CaloTPGRecord>().get(outTranscoder);
  outTranscoder->setup(eventSetup,CaloTPGTranscoder::HcalTPG);

  edm::ESHandle<HcalLutMetadata> lutMetadata;
  eventSetup.get<HcalLutMetadataRcd>().get(lutMetadata);
  float rctlsb = lutMetadata->getRctLsb();

  edm::ESHandle<HcalTrigTowerGeometry> pG;
  eventSetup.get<CaloGeometryRecord>().get(pG);
  
  // Step B: Create empty output
  std::auto_ptr<HcalTrigPrimDigiCollection> result(new HcalTrigPrimDigiCollection());
  std::auto_ptr<HcalUpgradeTrigPrimDigiCollection> up_result(new HcalUpgradeTrigPrimDigiCollection());

  edm::Handle<HBHEDigiCollection> hbheDigis;
  edm::Handle<HFDigiCollection>   hfDigis;
  edm::Handle<HBHEUpgradeDigiCollection> hbheUpDigis;
  edm::Handle<HFUpgradeDigiCollection>   hfUpDigis;

  if (upgrade_) {
    iEvent.getByLabel(inputLabel_[0],hbheUpDigis);
    iEvent.getByLabel(inputLabel_[1],hfUpDigis);
  } else {
    iEvent.getByLabel(inputLabel_[0],hbheDigis);
    iEvent.getByLabel(inputLabel_[1],hfDigis);
  }

  // protect here against missing input collections
  // there is no protection in HcalTriggerPrimitiveAlgo

  if (!hbheDigis.isValid() && !(upgrade_ && hbheUpDigis.isValid())) {
      edm::LogInfo("HcalTrigPrimDigiProducer")
              << "\nWarning: HBHE(Upgrade)DigiCollection with input tag "
              << inputLabel_[0]
              << "\nrequested in configuration, but not found in the event."
              << "\nQuit returning empty product." << std::endl;

      // put empty HcalTrigPrimDigiCollection in the event
      iEvent.put(result);

      outTranscoder->releaseSetup();

      return;
  }

  if (!hfDigis.isValid() && !(upgrade_ && hfUpDigis.isValid())) {
      edm::LogInfo("HcalTrigPrimDigiProducer")
              << "\nWarning: HF(Upgrade)DigiCollection with input tag "
              << inputLabel_[1]
              << "\nrequested in configuration, but not found in the event."
              << "\nQuit returning empty product." << std::endl;

      // put empty HcalTrigPrimDigiCollection in the event
      iEvent.put(result);

      outTranscoder->releaseSetup();

      return;
  }

  // Step C: Invoke the algorithm, passing in inputs and getting back outputs.
  if (upgrade_) {
    theAlgo_.run(inputCoder.product(),outTranscoder->getHcalCompressor().get(), outTranscoder.product(),
             *hbheUpDigis,  *hfUpDigis, *up_result, &(*pG), rctlsb);
  } else {
    theAlgo_.run(inputCoder.product(),outTranscoder->getHcalCompressor().get(), outTranscoder.product(),
             *hbheDigis,  *hfDigis, *result, &(*pG), rctlsb);
  }

  // Step C.1: Run FE Format Error / ZS for real data.
  if (runFrontEndFormatError_) {

        edm::ESHandle < HcalDbService > pSetup;
        eventSetup.get<HcalDbRecord> ().get(pSetup);
        const HcalElectronicsMap *emap = pSetup->getHcalMapping();

        edm::Handle < FEDRawDataCollection > fedHandle;
        iEvent.getByLabel(inputTagFEDRaw_, fedHandle);

        if (fedHandle.isValid() && emap != 0) {
            theAlgo_.runFEFormatError(fedHandle.product(), emap, *result);
        } else {
            edm::LogInfo("HcalTrigPrimDigiProducer")
                    << "\nWarning: FEDRawDataCollection with input tag "
                    << inputTagFEDRaw_
                    << "\nrequested in configuration, but not found in the event."
                    << "\nQuit returning empty product." << std::endl;

            // produce empty HcalTrigPrimDigiCollection and put it in the event
            std::auto_ptr < HcalTrigPrimDigiCollection > emptyResult(
                    new HcalTrigPrimDigiCollection());

            iEvent.put(emptyResult);

            outTranscoder->releaseSetup();

            return;
        }

  }

  if (runZS_) theAlgo_.runZS(*result);

  //  edm::LogInfo("HcalTrigPrimDigiProducer") << "HcalTrigPrims: " << result->size();

  // Step D: Put outputs into event
  iEvent.put(result);
  iEvent.put(up_result);

  outTranscoder->releaseSetup();
}



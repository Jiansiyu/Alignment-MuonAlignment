#include <string>
#include <map>
#include <vector>

// Framework
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "FWCore/Framework/interface/MakerMacros.h"

// Database
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// Alignment
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrors.h"
#include "CondFormats/DataRecord/interface/DTAlignmentRcd.h"
#include "CondFormats/DataRecord/interface/DTAlignmentErrorRcd.h"
#include "CondFormats/DataRecord/interface/CSCAlignmentRcd.h"
#include "CondFormats/DataRecord/interface/CSCAlignmentErrorRcd.h"


class  MuonAlignmentDBCopy : public edm::EDAnalyzer {
 public:
  explicit  MuonAlignmentDBCopy(const edm::ParameterSet& iConfig ) {};
  ~MuonAlignmentDBCopy() ;

  virtual void analyze( const edm::Event& evt, const edm::EventSetup& evtSetup);

 private:

  Alignments* myDTAlignments;
  AlignmentErrors* myDTAlignmentErrors;
  Alignments* myCSCAlignments;
  AlignmentErrors* myCSCAlignmentErrors;

};


MuonAlignmentDBCopy::~MuonAlignmentDBCopy( )
{

}


void MuonAlignmentDBCopy::analyze( const edm::Event& evt, const edm::EventSetup& iSetup)
{

  // 1. Retrieve DT alignment[Error]s from DBase
  edm::ESHandle<Alignments> dtAlignments;
  iSetup.get<DTAlignmentRcd>().get( dtAlignments );
  edm::ESHandle<AlignmentErrors> dtAlignmentErrors;
  iSetup.get<DTAlignmentErrorRcd>().get( dtAlignmentErrors );

  //   Create new objects we own
  myDTAlignments = new Alignments( *dtAlignments );
  myDTAlignmentErrors = new AlignmentErrors( *dtAlignmentErrors );

  // 2. Retrieve CSC alignment[Error]s from DBase
  edm::ESHandle<Alignments> cscAlignments;
  iSetup.get<CSCAlignmentRcd>().get( cscAlignments );
  edm::ESHandle<AlignmentErrors> cscAlignmentErrors;
  iSetup.get<CSCAlignmentErrorRcd>().get( cscAlignmentErrors );

  //   Create new objects we own
  myCSCAlignments = new Alignments( *cscAlignments );
  myCSCAlignmentErrors = new AlignmentErrors( *cscAlignmentErrors );

  // 2. Store alignment[Error]s to DB
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  // Call service
  if( !poolDbService.isAvailable() ) // Die if not available
	throw cms::Exception("NotAvailable") << "PoolDBOutputService not available";
	  
  // Define callback tokens for the records 
  size_t dtAlignmentsToken = poolDbService->callbackToken("dtAlignments");
  size_t dtAlignmentErrorsToken  = poolDbService->callbackToken("dtAlignmentErrors");

  size_t cscAlignmentsToken = poolDbService->callbackToken("cscAlignments");
  size_t cscAlignmentErrorsToken = poolDbService->callbackToken("cscAlignmentErrors");
	  
  // Store
  poolDbService->newValidityForNewPayload<Alignments>( myDTAlignments, 
                                                       poolDbService->endOfTime(), 
                                                       dtAlignmentsToken );
  poolDbService->newValidityForNewPayload<AlignmentErrors>( myDTAlignmentErrors, 
                                                            poolDbService->endOfTime(), 
                                                            dtAlignmentErrorsToken );

  poolDbService->newValidityForNewPayload<Alignments>( myCSCAlignments, 
                                                       poolDbService->endOfTime(), 
                                                       cscAlignmentsToken );
  poolDbService->newValidityForNewPayload<AlignmentErrors>( myCSCAlignmentErrors, 
                                                            poolDbService->endOfTime(), 
                                                            cscAlignmentErrorsToken );

}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonAlignmentDBCopy);



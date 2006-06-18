// -*- C++ -*-
//
// Package:    TestAnalyzer
// Class:      TestAnalyzer
// 
//
// Description: Module to test the Alignment software
//
//
// Original Author:  Frederic Ronga
//         Created:  March 16, 2006
//        Modified:  June   8, 2006
//


// system include files
#include <string>
#include <TTree.h>
#include <TFile.h>
#include <TRotMatrix.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Alignment/MuonAlignment/interface/AlignableMuon.h"
#include "Alignment/MuonAlignment/interface/AlignableDTChamber.h"
#include "Alignment/MuonAlignment/interface/AlignableCSCChamber.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/DTGeometry/interface/DTChamber.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

//
//
// class declaration
//

class TestAnalyzer : public edm::EDAnalyzer {
public:
  explicit TestAnalyzer( const edm::ParameterSet& );
  ~TestAnalyzer();
  
  
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
private:
  // ----------member data ---------------------------
  TTree* theTree;
  TFile* theFile;
  float x,y,z,phi,theta,length,thick,width;
  TRotMatrix* rot;

};

//
// constructors and destructor
//
TestAnalyzer::TestAnalyzer( const edm::ParameterSet& iConfig ) 
{ 

  // Open root file and define tree
  std::string fileName = iConfig.getUntrackedParameter<std::string>("fileName","test.root");
  theFile = new TFile( fileName.c_str(), "RECREATE" );
  theTree = new TTree( "theTree", "Detector units positions" );
  
  theTree->Branch("x",      &x,      "x/F"      );
  theTree->Branch("y",      &y,      "y/F"      );
  theTree->Branch("z",      &z,      "z/F"      );
  theTree->Branch("phi",    &phi,    "phi/F"    );
  theTree->Branch("theta",  &theta,  "theta/F"  );
  theTree->Branch("length", &length, "length/F" );
  theTree->Branch("width",  &width,  "width/F"  );
  theTree->Branch("thick",  &thick,  "thick/F"  );
  rot = 0;
  theTree->Branch("rot",    "TRotMatrix", &rot  );

}


TestAnalyzer::~TestAnalyzer()
{ 
  
  theTree->Write();
  theFile->Close();
  
}


void
TestAnalyzer::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{

   
  edm::LogInfo("MuonAlignment") << "Starting!";

  //
  // Retrieve muon geometry from event setup
  //
  edm::ESHandle<DTGeometry> pDT;
  edm::ESHandle<CSCGeometry> pCSC;

  iSetup.get<MuonGeometryRecord>().get( pDT );     
//  iSetup.get<MuonGeometryRecord>().get( pCSC ); 


  // Now loop on detector units, and store position and orientation
  for ( std::vector<DTChamber*>::const_iterator iGeomDet = pDT->chambers().begin();
		iGeomDet != pDT->chambers().end(); iGeomDet++ )
	{
	  x      = (*iGeomDet)->position().x();
	  y      = (*iGeomDet)->position().y();
	  z      = (*iGeomDet)->position().z();
	  phi    = (*iGeomDet)->surface().normalVector().phi();
	  theta  = (*iGeomDet)->surface().normalVector().theta();
	  length = (*iGeomDet)->surface().bounds().length();
	  width  = (*iGeomDet)->surface().bounds().width();
	  thick  = (*iGeomDet)->surface().bounds().thickness();

       std::cout << "x=" << x << ", y=" << y << ", theta=" << theta <<std::endl;

	  double matrix[9] = { 
		(*iGeomDet)->rotation().xx(),
		(*iGeomDet)->rotation().xy(),
		(*iGeomDet)->rotation().xz(),
		(*iGeomDet)->rotation().yx(),
		(*iGeomDet)->rotation().yy(),
		(*iGeomDet)->rotation().yz(),
		(*iGeomDet)->rotation().zx(),
		(*iGeomDet)->rotation().zy(),
		(*iGeomDet)->rotation().zz()
	  };
	  rot = new TRotMatrix( "rot", "rot", matrix );

	  theTree->Fill();

	}
  
  edm::LogInfo("MuonAlignment") << "Done!";

}

//define this as a plug-in
DEFINE_FWK_MODULE(TestAnalyzer)

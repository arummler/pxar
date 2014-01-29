#include <stdlib.h>     /* atof, atoi */
#include <algorithm>    // std::find
#include <iostream>
#include "PixTestAlive.hh"
#include "log.h"

#include <TH2.h>

using namespace std;
using namespace pxar;

ClassImp(PixTestAlive)

// ----------------------------------------------------------------------
PixTestAlive::PixTestAlive(PixSetup *a, std::string name) : PixTest(a, name), fParNtrig(-1), fParVcal(-1) {
  PixTest::init(a, name);
  init(); 
  LOG(logINFO) << "PixTestAlive ctor(PixSetup &a, string, TGTab *)";
}


//----------------------------------------------------------
PixTestAlive::PixTestAlive() : PixTest() {
  LOG(logINFO) << "PixTestAlive ctor()";
}

// ----------------------------------------------------------------------
bool PixTestAlive::setParameter(string parName, string sval) {
  bool found(false);
  for (map<string,string>::iterator imap = fParameters.begin(); imap != fParameters.end(); ++imap) {
    LOG(logINFO) << "---> " << imap->first;
    if (0 == imap->first.compare(parName)) {
      found = true; 

      fParameters[parName] = sval;
      LOG(logINFO) << "  ==> parName: " << parName;
      LOG(logINFO) << "  ==> sval:    " << sval;
      if (!parName.compare("Ntrig")) {
	fParNtrig = atoi(sval.c_str()); 
	setToolTips();
      }
      if (!parName.compare("Vcal")) {
	fParVcal = atoi(sval.c_str()); 
	setToolTips();
      }
      break;
    }
  }
  return found; 
}


// ----------------------------------------------------------------------
void PixTestAlive::init() {
  LOG(logINFO) << "PixTestAlive::init()";

  setToolTips();
  fDirectory = gFile->GetDirectory(fName.c_str()); 
  if (!fDirectory) {
    fDirectory = gFile->mkdir(fName.c_str()); 
  } 
  fDirectory->cd(); 

}

// ----------------------------------------------------------------------
void PixTestAlive::setToolTips() {
  fTestTip    = string("send Ntrig \"calibrates\" and count how many hits were measured\n")
    + string("the result is a hitmap, not an efficiency map")
    ;
  fSummaryTip = string("all ROCs are displayed side-by-side. Note the orientation:\n")
    + string("the canvas bottom corresponds to the narrow module side with the cable")
    ;
}


// ----------------------------------------------------------------------
void PixTestAlive::bookHist(string name) {
  fDirectory->cd(); 
  LOG(logINFO) << "nothing done with " << name; 
}


//----------------------------------------------------------
PixTestAlive::~PixTestAlive() {
  LOG(logINFO) << "PixTestAlive dtor";
}


// ----------------------------------------------------------------------
void PixTestAlive::doTest() {
  LOG(logINFO) << "PixTestAlive::doTest() ntrig = " << fParNtrig;
  PixTest::update(); 

  if (fApi) fApi->_dut->testAllPixels(true);
  vector<TH2D*> test2 = efficiencyMaps("PixelAlive", fParNtrig); 
  copy(test2.begin(), test2.end(), back_inserter(fHistList));

  TH2D *h = (TH2D*)(*fHistList.begin());

  h->Draw();
  fDisplayedHist = find(fHistList.begin(), fHistList.end(), h);
  PixTest::update(); 
  LOG(logINFO) << "PixTestAlive::doTest() done";
}

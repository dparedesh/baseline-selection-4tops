#include "SM4topsSSML/ElectronDecorator.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include "TopEvent/EventTools.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include <AsgTools/MessageCheck.h>
#include <AthContainers/exceptions.h>

#include "TTHbbLeptonic/EventSaverHelper.h"
#include "TTHbbObjects/Event.h"

#include "TTHbbLeptonic/TTHbbAsgToolHelper.h"


using namespace asg::msgUserCode;


static const float Pi = 3.14159;
static const float PTTOCURVATURE = -0.301; // ATLAS B=+2T (in MeV/mm)
static const float SEPCUT      = 1;
static const float DCTCUT      = 0.02;
static const float SIMINHITS   = 7; // requiring min 7 Si hits
static const float MAXZ0DIFF = 0.5; // (in mm)

namespace SM4topsSSML {

ElectronDecorator::ElectronDecorator(const std::string &params, std::shared_ptr<top::TopConfig> config)
  : m_trkseltool("InDetTrackSelectionToolLoose") {

  auto *esh = EventSaverHelper::get();

  top::check( m_trkseltool.setProperty("CutLevel","Loose"),
	      "InDetTrackSelectionTool: Failed to set track working point.");
  top::check( m_trkseltool.setProperty("minPt"   ,1000),
	      "InDetTrackSelectionTool: Failed to set track pt.");
  top::check( m_trkseltool.setProperty("maxZ0SinTheta",3.), 
	      "InDetTrackSelectionTool: Failed to set track maxZ0SinTheta");
  top::check( m_trkseltool.initialize(), 
	      "InDetTrackSelectionTool: Failed to initialize.");

  // electron track related info
  esh->addElecVariableToBeSaved("PromptLeptonVeto", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("deltaPhi2", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("leadTrackPt", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("subleadTrackPt", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("leadSubleadDr", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("leadSubleadDeta", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("leadSubleadDphi", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("sumTrackPt", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  // adding new variable for CO studies
  esh->addElecVariableToBeSaved("MeeCO", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("RadiusCO", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTracknIL", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackeIL", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTracknNIL", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackPt", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackEta", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackPhi", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackD0", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackZ0", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("closestSiTrackQ", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackPt", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackEta", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackPhi", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackD0", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackZ0", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("bestmatchSiTrackQ", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");
  esh->addElecVariableToBeSaved("separationMinDCT", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");

  esh->addElecVariableToBeSaved("ECIDSResult_float", TTHbb::DecorationType::Float, false,
                                "ElectronDecorator");

  if ( asg::ToolStore::contains<TTHbbAsgToolHelper>("TTHbbAsgHelper") ) {
    m_asgHelper = asg::ToolStore::get<TTHbbAsgToolHelper>("TTHbbAsgHelper");
    m_isRemote=true; 
  }
  else {
    m_asgHelper = new TTHbbAsgToolHelper("TTHbbAsgHelper");
    top::check( m_asgHelper->initialize() , "Failed to initialize TTHbbAsgHelper" );
  }


}

bool ElectronDecorator::apply(const top::Event &event) const {
  
  // Check if TTHbb::Event exists
  if (!event.m_info->isAvailable<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables")) {
    std::cout << "ElectronDecorator: TTHbbEventVariables (TTHbb::Event*) "
                 "object not found"
              << std::endl;
    std::cout << "------> aborting :-( " << std::endl;
    abort();
  }

  // Extract TTHbb::Event
  std::shared_ptr<TTHbb::Event> tthevt =
      event.m_info->auxdecor<std::shared_ptr<TTHbb::Event>>("TTHbbEventVariables");

  /* Loop over electrons. Use an index loop to be able to access the TTHbb electron 
   * as well as the xAODElectron */
  for (unsigned int iel = 0; iel < tthevt->m_electrons.size(); iel++) {
    
    //ECID result
    tthevt->m_electrons.at(iel)->floatVariable("ECIDSResult_float") = float(event.m_electrons.at(iel)->auxdata<double>("DFCommonElectronsECIDSResult"));

    //PromptLeptonVeto
    tthevt->m_electrons.at(iel)->floatVariable("PromptLeptonVeto") =event.m_electrons.at(iel)->auxdata<float>("PromptLeptonVeto");

    //deltaPhi between electron track and cluster in EM2
    tthevt->m_electrons.at(iel)->floatVariable("deltaPhi2") =event.m_electrons.at(iel)->auxdata<float>("deltaPhi2");

    //Conversion tagging variables
    const xAOD::VertexContainer* vertices = event.m_primaryVertices;
    const xAOD::Vertex *pv = NULL;
    for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
      if(vertices->at(iVertex)->vertexType() != xAOD::VxType::PriVtx) continue;
      pv = vertices->at(iVertex);
      break;
    }

    const xAOD::TrackParticle *leadTrack = NULL;
    const xAOD::TrackParticle *subleadTrack = NULL;
    float sumTrackPt = 0;
    std::set<const xAOD::TrackParticle*> allElectronTracks = xAOD::EgammaHelpers::getTrackParticles((const xAOD::Egamma*)event.m_electrons.at(iel),true);
    std::set<const xAOD::TrackParticle*> bestElectronTrack = xAOD::EgammaHelpers::getTrackParticles((const xAOD::Egamma*)event.m_electrons.at(iel),true,false);
    for (size_t itrk = 0; itrk < event.m_electrons.at(iel)->nTrackParticles(); itrk++) {
      const xAOD::TrackParticle* gsf_track = event.m_electrons.at(iel)->trackParticle(itrk);
      const xAOD::TrackParticle* id_track = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(gsf_track);
      if (!id_track) continue;
      if (!itrk) {
	leadTrack = id_track;
	continue;
      }
      if (!m_trkseltool.accept(*id_track,pv)) continue; 
      if (id_track->vertex() && id_track->vertex()->index()) continue;
      sumTrackPt += id_track->pt();
      if (leadTrack && leadTrack->charge()*id_track->charge()>0) continue;
      if (!subleadTrack || id_track->pt()>subleadTrack->pt()) subleadTrack = id_track;
    }

    tthevt->m_electrons.at(iel)->floatVariable("leadTrackPt")     = leadTrack                 ? leadTrack->pt()                                : 0;
    tthevt->m_electrons.at(iel)->floatVariable("subleadTrackPt")  = subleadTrack              ? subleadTrack->pt()                             : 0;
    tthevt->m_electrons.at(iel)->floatVariable("leadSubleadDr")   = leadTrack && subleadTrack ? leadTrack->p4().DeltaR(subleadTrack->p4())     : 0;
    tthevt->m_electrons.at(iel)->floatVariable("leadSubleadDeta") = leadTrack && subleadTrack ? leadTrack->eta()-subleadTrack->eta()           : 0;
    tthevt->m_electrons.at(iel)->floatVariable("leadSubleadDphi") = leadTrack && subleadTrack ? (leadTrack->p4().DeltaPhi(subleadTrack->p4())) : 0;
    tthevt->m_electrons.at(iel)->floatVariable("sumTrackPt")      = sumTrackPt;

    // get from ttH Framework
    // Variable used for CO study
    // improve mtrktrk and Radius variable
  float mll_conv;
  float mll_conv_atConvV;
  float radius_conv, radius_convX, radius_convY;
  float closestSiTracknIL, closestSiTrackeIL, closestSiTracknNIL, closestSiTrackeNIL;
  float closestSiTrackPt, closestSiTrackEta, closestSiTrackPhi, closestSiTrackD0, closestSiTrackZ0, closestSiTrackQ;
  float bestmatchSiTrackPt, bestmatchSiTrackEta, bestmatchSiTrackPhi, bestmatchSiTrackD0, bestmatchSiTrackZ0, bestmatchSiTrackQ;
  float separationMinDCT;


  const xAOD::Vertex *pvtx = nullptr;
  const xAOD::VertexContainer *vtxC = 0;

  top::check( m_asgHelper->evtStore()->retrieve(vtxC,"PrimaryVertices"),"Failed to retrieve PrimaryVertices");

  for (auto vertex: *vtxC) {
    if (vertex->vertexType() == xAOD::VxType::VertexType::PriVtx) {
      pvtx     = vertex;
      break;
    }
  }

    mll_conv=-999;
    mll_conv_atConvV=-999;
    radius_conv=-999;
    radius_convX=-999;
    radius_convY=-999;
    closestSiTracknIL = -999;
    closestSiTrackeIL = -999;
    closestSiTracknNIL = -999;
    closestSiTrackeNIL = -999;

    closestSiTrackPt = closestSiTrackEta = closestSiTrackPhi = closestSiTrackD0 = closestSiTrackZ0  = closestSiTrackQ = -999;
    bestmatchSiTrackPt = bestmatchSiTrackEta = bestmatchSiTrackPhi = bestmatchSiTrackD0 = bestmatchSiTrackZ0 = bestmatchSiTrackQ = -999;

    separationMinDCT = -999; // how good the distance between the circles is

    int nTPSi = 0;
    int nTPSiNoIBL = 0;
    double detaMin(999);

    const xAOD::TrackParticleContainer *tpC(nullptr);
    top::check( m_asgHelper->evtStore()->retrieve(tpC,"InDetTrackParticles"),"Failed to retrieve InDetTrackParticles");

    if (tpC)
      {
        const xAOD::TrackParticle *closestSiTrack(0);
        const xAOD::TrackParticle *bestmatchedGSFElTrack= event.m_electrons.at(iel)->trackParticle(0);
        const xAOD::TrackParticle *bestmatchedElTrack = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(bestmatchedGSFElTrack);
        //std::cout << "Validation : electron pT" <<  event.m_electrons.at(iel)->pt() << std::endl;

        for (auto tracks1 : *tpC)  // loop on all tracks
          {
            double dR = bestmatchedElTrack->p4().DeltaR(tracks1->p4());
            double dz0 = fabs(bestmatchedElTrack->z0() - tracks1->z0())*sin(bestmatchedElTrack->theta());
            if (  (dR<0.3) && (dz0<MAXZ0DIFF) ) // loose DeltaR cut and tight delta(z0) cut
              {
               //std::cout << "Looping on all tracks" << std::endl;
                bool hasSi(false);
                uint8_t nBL = 0, nIL = 0, nNIL = 0, nPix = 0, nSCT = 0;
                uint8_t nPshared = 0, nSshared = 0;
                uint8_t eBL = 1, eIL = 1, eNIL = 1;
                tracks1->summaryValue(nPix,xAOD::numberOfPixelHits);
                tracks1->summaryValue(nBL,xAOD::numberOfBLayerHits);
                tracks1->summaryValue(nIL,xAOD::numberOfInnermostPixelLayerHits);
                tracks1->summaryValue(nNIL,xAOD::numberOfNextToInnermostPixelLayerHits);
                tracks1->summaryValue(nSCT,xAOD::numberOfSCTHits);
                tracks1->summaryValue(eBL,xAOD::expectBLayerHit);
                tracks1->summaryValue(eIL,xAOD::expectInnermostPixelLayerHit);
                tracks1->summaryValue(eNIL,xAOD::expectNextToInnermostPixelLayerHit);
                tracks1->summaryValue(nPshared,xAOD::numberOfPixelSharedHits);
                tracks1->summaryValue(nSshared,xAOD::numberOfSCTSharedHits);
                if((nPix+nSCT)>=SIMINHITS){ // requiring min 8 Si hits
                  hasSi=true;
                  nTPSi++;
                  if( (nIL==0 && eIL>0) || (eIL==0 && eNIL>0 && nNIL==0) ){
                    nTPSiNoIBL++;
                    }
                }

                double deta=fabs(tracks1->eta()-bestmatchedElTrack->eta());
                if(deta<detaMin && hasSi && ((bestmatchedElTrack->charge() * tracks1->charge()) < 0) ){ // opposite-sign
                  detaMin=deta;
                  closestSiTrack = tracks1;
                  closestSiTracknIL = nIL;
                  closestSiTrackeIL = eIL;
                  closestSiTracknNIL = nNIL;
                  closestSiTrackeNIL = eNIL;
                }
              }
          }  // end loop on all tracks

         //std::cout << "Validation: nTPSi: " << nTPSi << std::endl;
         //std::cout << "Validation: nTPSiNoIBL: " << nTPSiNoIBL << std::endl;
        bestmatchSiTrackPt    = bestmatchedElTrack->pt();
        bestmatchSiTrackEta   = bestmatchedElTrack->eta();
        bestmatchSiTrackPhi   = bestmatchedElTrack->phi();
        bestmatchSiTrackD0    = bestmatchedElTrack->d0();
        bestmatchSiTrackZ0    = bestmatchedElTrack->z0();
        bestmatchSiTrackQ     = bestmatchedElTrack->charge();

        if (closestSiTrack){
          // Keep pt, eta, phi, d0, z0
         closestSiTrackPt    = closestSiTrack->pt();
          closestSiTrackEta   = closestSiTrack->eta();
          closestSiTrackPhi   = closestSiTrack->phi();
          closestSiTrackD0    = closestSiTrack->d0();
          closestSiTrackZ0    = closestSiTrack->z0();
          closestSiTrackQ     = closestSiTrack->charge();

          TLorentzVector p0,p1;
          p0.SetPtEtaPhiM(bestmatchedElTrack->pt(),bestmatchedElTrack->eta(),bestmatchedElTrack->phi(),0.511);
          p1.SetPtEtaPhiM(closestSiTrack->pt(),closestSiTrack->eta(),closestSiTrack->phi(),0.511);

          mll_conv=(p0+p1).M()*1e-3;

          p0.SetPtEtaPhiM(bestmatchedElTrack->pt(),bestmatchedElTrack->eta(),0,0.511);
          p1.SetPtEtaPhiM(closestSiTrack->pt(),closestSiTrack->eta(),0,0.511);

          mll_conv_atConvV=(p0+p1).M()*1e-3;



          // Conversion methods
          // Helix array:
          // 0 cotan(theta)
          // 1 curvature
          // 2 z
          // 3 d0
          // 4 phi

          double helix1[5];
          double helix2[5];

          ////////////// TrackToHelix Reco Electron Track

          helix1[0] = 1./tan(bestmatchedElTrack->theta());
          helix1[1] = PTTOCURVATURE*bestmatchedElTrack->charge()/bestmatchedElTrack->pt();

          if(bestmatchedElTrack->phi0()>0.)
            helix1[4] = bestmatchedElTrack->phi0();
          else
            helix1[4] = 2*Pi + bestmatchedElTrack->phi0();

          double c1 = cos(bestmatchedElTrack->phi0());
          double s1 = sin(bestmatchedElTrack->phi0());
          helix1[3] = bestmatchedElTrack->d0() + c1*pvtx->y() - s1*pvtx->x();

          c1 = c1*1./tan(bestmatchedElTrack->theta());
          s1 = s1*1./tan(bestmatchedElTrack->theta());
          helix1[2] = bestmatchedElTrack->z0() - c1*pvtx->x() - s1*pvtx->y() + pvtx->z();

          ///////////// TrackToHelix Other Electron Track

          helix2[0] = 1./tan(closestSiTrack->theta());
          helix2[1] = PTTOCURVATURE*closestSiTrack->charge()/closestSiTrack->pt();

          if(closestSiTrack->phi0()>0.)
            helix2[4] = closestSiTrack->phi0();
          else
            helix2[4] = 2*Pi + closestSiTrack->phi0();

          double c2 = cos(closestSiTrack->phi0());
          double s2 = sin(closestSiTrack->phi0());
          helix2[3] = closestSiTrack->d0() + c2*pvtx->y() - s2*pvtx->x();

          c2 = c2*1./tan(closestSiTrack->theta());
          s2 = s2*1./tan(closestSiTrack->theta());
          helix2[2] = closestSiTrack->z0() - c2*pvtx->x() - s2*pvtx->y() + pvtx->z();

          double dct(helix1[0]-helix2[0]);

          //////
            double separation=999;
          double convX=999;
          double convY=999;

          double x1, y1, r1, x2, y2, r2;
          double cpx1, cpx2;

          double beta(0.);
          if(helix1[4] < helix2[4])
            {
              beta = Pi/2-helix1[4];
            }
          else
            {
              beta = Pi/2-helix2[4];
            }

          double phi1(helix1[4] + beta);
          if(phi1>2*Pi) phi1=phi1-2*Pi;
          if(phi1<0.) phi1=phi1+2*Pi;

          double phi2(helix2[4] + beta);
          if(phi2>2*Pi) phi2 = phi2 - 2*Pi;
          if(phi2<0) phi2 = phi2 + 2*Pi;


          /// HelixToCircle Main Track Electron
          r2 = 1/(2.*fabs(helix2[1]));

          double charge2(1.);
          if(helix2[1]<0.) charge2 = -1.;
          double rcenter2(helix2[3]/charge2 + r2);
          double phicenter2(phi2 + Pi/2*charge2);

          x2 = rcenter2*cos(phicenter2);
          y2 = rcenter2*sin(phicenter2);
          //////

          double dx(x1- x2);
          if(dx <  1e-9 && dx > 0.) dx =  1e-9;
          if(dx > -1e-9 && dx < 0.) dx = -1e-9;
          double slope((y1-y2)/dx);
          double b(y1 - slope*x1);
          double alpha(atan(slope));
          double d(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
          //only keeping opposite sign option

          separation = d-r1-r2;
          if(x1>x2)
            {
              cpx1 = x1-r1*cos(alpha);
              cpx2 = x2+r2*cos(alpha);
            }
          else
            {
              cpx1 = x1+r1*cos(alpha);
              cpx2 = x2 - r2*cos(alpha);
            }


          double temp1 = (cpx1+cpx2)/2;
          double temp2 = slope*temp1+b;
          convX = cos(beta)*temp1 + sin(beta)*temp2;
          convY = -sin(beta)*temp1+ cos(beta)*temp2;


          ///////
          if(fabs(separation)<SEPCUT && fabs(dct)<DCTCUT){
            separationMinDCT=separation;
            radius_convX=convX;
            radius_convY=convY;
            radius_conv=sqrt(convX*convX + convY*convY);
            if( convX*cos(bestmatchedElTrack->phi()) + convY*sin(bestmatchedElTrack->phi()) < 0)radius_conv = -radius_conv;

          }
        }

      }
    // std::cout << "Validation w Henri: track0 pT eta: " << bestmatchSiTrackPt*bestmatchSiTrackQ << " " << bestmatchSiTrackEta << std::endl;
    // std::cout << "Validation w Henri: Mass at PV: " << mll_conv << std::endl;
    // std::cout << "Validation w Henri: Mass at conv. vertex : " << mll_conv_atConvV << std::endl;
    // std::cout << "Validation w Henri: Radius: " << radius_conv << std::endl;
    // std::cout << "Validation w Henri: separationMinDCT: " << separationMinDCT << std::endl;
    // std::cout << "Validation w Henri: closestSiTrackPt: " << closestSiTrackPt << std::endl;



     tthevt->m_electrons.at(iel)->floatVariable("Mll_conv_atPV")     = mll_conv;
     tthevt->m_electrons.at(iel)->floatVariable("Mll_conv_atConvV")     = mll_conv_atConvV;
    tthevt->m_electrons.at(iel)->floatVariable("RadiusCO")  = radius_conv;
    tthevt->m_electrons.at(iel)->floatVariable("RadiusCO_convX")  = radius_convX;
    tthevt->m_electrons.at(iel)->floatVariable("RadiusCO_convY")  = radius_convY;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTracknIL") = closestSiTracknIL;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackeIL") = closestSiTrackeIL;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTracknNIL") = closestSiTracknNIL;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackeNIL") = closestSiTrackeNIL;

    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackPt") = closestSiTrackPt;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackEta") = closestSiTrackEta;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackPhi") = closestSiTrackPhi;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackD0") = closestSiTrackD0;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackZ0") = closestSiTrackZ0;
    tthevt->m_electrons.at(iel)->floatVariable("closestSiTrackQ") = closestSiTrackQ;

    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackPt") = bestmatchSiTrackPt;
    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackEta") = bestmatchSiTrackEta;
    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackPhi") = bestmatchSiTrackPhi;
    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackD0") = bestmatchSiTrackD0;
    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackZ0") = bestmatchSiTrackZ0;
    tthevt->m_electrons.at(iel)->floatVariable("bestmatchSiTrackQ") = bestmatchSiTrackQ;

    tthevt->m_electrons.at(iel)->floatVariable("separationMinDCT") = separationMinDCT;

  }  // End electron loop

  return true;
}

std::string ElectronDecorator::name() const { return "DECORATEELECTRONSSSML"; }

}  // namespace SM4topsSSML

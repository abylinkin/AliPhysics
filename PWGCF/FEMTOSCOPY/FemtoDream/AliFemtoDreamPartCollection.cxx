/*
 * AliFemtoPPbpbLamPartCollection.cxx

 *
 *  Created on: Aug 29, 2017
 *      Author: gu74req
 */
#include <iostream>
#include "AliFemtoDreamPartCollection.h"
#include "AliLog.h"
ClassImp(AliFemtoDreamPartCollection)
AliFemtoDreamPartCollection::AliFemtoDreamPartCollection()
:fResults()
,fNSpecies(0)
,fMinimalBooking(false)
,fZVtxMultBuffer()
,fValuesZVtxBins()
,fValuesMultBins()
{

}
AliFemtoDreamPartCollection::AliFemtoDreamPartCollection(
    AliFemtoDreamCollConfig *conf,bool MinimalBooking)
:fResults(new AliFemtoDreamCorrHists(conf,MinimalBooking))
,fNSpecies(conf->GetNParticles())
,fMinimalBooking(false)
,fZVtxMultBuffer(conf->GetNZVtxBins(),
                 std::vector<AliFemtoDreamZVtxMultContainer>(
                     conf->GetNMultBins(),
                     AliFemtoDreamZVtxMultContainer(conf)))
,fValuesZVtxBins(conf->GetZVtxBins())
,fValuesMultBins(conf->GetMultBins())
{
}

AliFemtoDreamPartCollection::~AliFemtoDreamPartCollection() {
}

void AliFemtoDreamPartCollection::SetEvent(
    std::vector<std::vector<AliFemtoDreamBasePart>> &Particles,
    double ZVtx,double Mult)
{
  if (Particles.size()!=fNSpecies) {
    TString fatalOut=
        Form("Too few Species %d for %d",Particles.size(),fNSpecies);
    AliFatal(fatalOut.Data());
  }
  int bins[2] = {0,0};
  FindBin(ZVtx,Mult,bins);
  if (bins[0]==-99||bins[1]==-99) {
    std::cout << "Mult ("<<Mult<<") Bin ("<<bins[1]<<") and ZVtx ("<<ZVtx<<") Bin ("<<bins[0]<<")\n";
    TString fatalOut=
        Form("No Multiplicity bin (%i) for this multiplicity (%i) "
            " or Vtx Bin (%i) for this zVtx (%4.2f) \n",bins[1],Mult,bins[0],ZVtx);
    std::cout << "Mult ("<<Mult<<") Bin ("<<bins[1]<<") and ZVtx ("<<ZVtx<<") Bin ("<<bins[0]<<")\n";
    AliWarning(fatalOut.Data());
  } else {
    auto itZVtx=fZVtxMultBuffer.begin();
    itZVtx+=bins[0];
    auto itMult=itZVtx->begin();
    itMult+=bins[1];
    itMult->PairParticlesSE(Particles,fResults,bins[1]);
    itMult->PairParticlesME(Particles,fResults,bins[1]);
    itMult->SetEvent(Particles);
  }
  return;
}

void AliFemtoDreamPartCollection::PrintEvent(int ZVtx,int Mult) {
  auto itZVtx=fZVtxMultBuffer.begin();
  itZVtx+=ZVtx;
  auto itMult=itZVtx->begin();
  itMult+=Mult;
  return;
}

void AliFemtoDreamPartCollection::FindBin(double ZVtxPos,double Multiplicity,
                                          int *returnBins) {
  returnBins[0]=-99;
  returnBins[1]=-99;
  for (auto itBin=fValuesZVtxBins.begin();itBin!=fValuesZVtxBins.end()-1;
      ++itBin) {
    if (*itBin<ZVtxPos && ZVtxPos<=*(itBin+1)) {
      returnBins[0]=itBin-fValuesZVtxBins.begin();
      break;
    }
  }
  int binCounter = fValuesMultBins.size();
  for (std::vector<int>::reverse_iterator itBin=fValuesMultBins.rbegin();
      itBin!=fValuesMultBins.rend();++itBin) {
    binCounter--;
    if (Multiplicity>=*itBin) {
      returnBins[1]=binCounter;
      break;
    }
  }
  return;
}

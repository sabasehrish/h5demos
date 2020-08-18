#ifndef SERIALIZE_DATAPRODS_H
#define SERIALIZE_DATAPRODS_H
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClass.h"
#include "TBufferFile.h"
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> return_dsetnames(TObjArray* l) {
  std::vector<std::string> names;
  names.reserve(l->GetEntriesFast());
  for( int i=0; i< l->GetEntriesFast(); ++i) {
    auto b = dynamic_cast<TBranch*>((*l)[i]);
    names.push_back(b->GetName());
  }
  return names;
}

std::vector<TClass*> return_classes(TObjArray* l) {
  std::vector<TClass*> classes;
  classes.reserve(l->GetEntriesFast());
  for( int i=0; i< l->GetEntriesFast(); ++i) {
    auto b = dynamic_cast<TBranch*>((*l)[i]);
    //std::cout<<b->GetName()<<std::endl;
    //std::cout<<b->GetClassName()<<std::endl;
    b->SetupAddresses();
    TClass* class_ptr=nullptr;
    EDataType type;
    b->GetExpectedType(class_ptr,type);
    classes.push_back( class_ptr);
  }
  return classes;
}

std::vector<char> return_blob(TBranch* b, TClass* c) {
  TBufferFile bufferFile{TBuffer::kWrite};
  auto address = reinterpret_cast<void**>(b->GetAddress());
  c->WriteBuffer(bufferFile, *address);
  std::vector<char> blob(bufferFile.Buffer(), bufferFile.Buffer()+bufferFile.Length());
  return blob;
}

void cms_read() {
  auto f = TFile::Open("PoolOutputTest.root");
  auto e = f->Get<TTree>("Events");
  auto l = e->GetListOfBranches();
  auto classes = return_classes(l);
  std::cout << "Total events: "<< e->GetEntriesFast() << std::endl;
  for(int ievt=0; ievt< e->GetEntriesFast();++ievt) {
    std::cout <<"event "<<ievt<<std::endl;
    e->GetEntry(ievt);
    for( int ib = 0; ib< l->GetEntriesFast(); ++ib) {
      auto b = dynamic_cast<TBranch*>((*l)[ib]);
      auto blob =  return_blob(b, classes[ib]);
      std::cout<<"   "<<b->GetName()<<" size "<<blob.size() <<std::endl;;
    }
  }
}
#endif

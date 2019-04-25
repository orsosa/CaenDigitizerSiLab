// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME CaenDigitizerSiLabDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "configuration.h"
#include "CaenDigitizerSiLab.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_CaenDigitizerSiLab(void *p = 0);
   static void *newArray_CaenDigitizerSiLab(Long_t size, void *p);
   static void delete_CaenDigitizerSiLab(void *p);
   static void deleteArray_CaenDigitizerSiLab(void *p);
   static void destruct_CaenDigitizerSiLab(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CaenDigitizerSiLab*)
   {
      ::CaenDigitizerSiLab *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CaenDigitizerSiLab >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CaenDigitizerSiLab", ::CaenDigitizerSiLab::Class_Version(), "CaenDigitizerSiLab.h", 28,
                  typeid(::CaenDigitizerSiLab), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CaenDigitizerSiLab::Dictionary, isa_proxy, 4,
                  sizeof(::CaenDigitizerSiLab) );
      instance.SetNew(&new_CaenDigitizerSiLab);
      instance.SetNewArray(&newArray_CaenDigitizerSiLab);
      instance.SetDelete(&delete_CaenDigitizerSiLab);
      instance.SetDeleteArray(&deleteArray_CaenDigitizerSiLab);
      instance.SetDestructor(&destruct_CaenDigitizerSiLab);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CaenDigitizerSiLab*)
   {
      return GenerateInitInstanceLocal((::CaenDigitizerSiLab*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::CaenDigitizerSiLab*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr CaenDigitizerSiLab::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CaenDigitizerSiLab::Class_Name()
{
   return "CaenDigitizerSiLab";
}

//______________________________________________________________________________
const char *CaenDigitizerSiLab::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CaenDigitizerSiLab*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CaenDigitizerSiLab::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CaenDigitizerSiLab*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CaenDigitizerSiLab::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CaenDigitizerSiLab*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CaenDigitizerSiLab::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CaenDigitizerSiLab*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void CaenDigitizerSiLab::Streamer(TBuffer &R__b)
{
   // Stream an object of class CaenDigitizerSiLab.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CaenDigitizerSiLab::Class(),this);
   } else {
      R__b.WriteClassBuffer(CaenDigitizerSiLab::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CaenDigitizerSiLab(void *p) {
      return  p ? new(p) ::CaenDigitizerSiLab : new ::CaenDigitizerSiLab;
   }
   static void *newArray_CaenDigitizerSiLab(Long_t nElements, void *p) {
      return p ? new(p) ::CaenDigitizerSiLab[nElements] : new ::CaenDigitizerSiLab[nElements];
   }
   // Wrapper around operator delete
   static void delete_CaenDigitizerSiLab(void *p) {
      delete ((::CaenDigitizerSiLab*)p);
   }
   static void deleteArray_CaenDigitizerSiLab(void *p) {
      delete [] ((::CaenDigitizerSiLab*)p);
   }
   static void destruct_CaenDigitizerSiLab(void *p) {
      typedef ::CaenDigitizerSiLab current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CaenDigitizerSiLab

namespace {
  void TriggerDictionaryInitialization_CaenDigitizerSiLabDict_Impl() {
    static const char* headers[] = {
"configuration.h",
"CaenDigitizerSiLab.h",
0
    };
    static const char* includePaths[] = {
"/home/pablo/Documentos/ROOT/builddir/include",
"/home/pablo/Documentos/CaenDigitizerSiLab/CaenDigitizerSiLab/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CaenDigitizerSiLabDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$CaenDigitizerSiLab.h")))  __attribute__((annotate("$clingAutoload$configuration.h")))  CaenDigitizerSiLab;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CaenDigitizerSiLabDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
#include "configuration.h"
#include "CaenDigitizerSiLab.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"CaenDigitizerSiLab", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CaenDigitizerSiLabDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CaenDigitizerSiLabDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CaenDigitizerSiLabDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CaenDigitizerSiLabDict() {
  TriggerDictionaryInitialization_CaenDigitizerSiLabDict_Impl();
}

#ifndef _GADGET_H_
#define _GADGET_H_


#include <jni.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


class InstrumentGadgetComposer
{
  public:
    InstrumentGadgetComposer(JNIEnv *env, jobject ref_class_loader,
                             jmethodID id_load_class)
      : env_(env),
        ref_class_loader_(ref_class_loader),
        id_load_class_(id_load_class)
    {}

    void compose();

  private:
    bool LinkWithAnalysisAPK();
    bool RegisterInstrumentGadget();

    JNIEnv* env_;
    jobject ref_class_loader_;
    jmethodID id_load_class_;
};

class MethodBundleNative
{
  public:
    MethodBundleNative(bool is_static, const std::string& name_class,
      const std::string& name_method, const std::string& signature_method,
      const std::vector<char>& type_inputs, char type_output,
      void* quick_code_entry_origin)
     : is_static_(is_static),
       type_output_(type_output),
       quick_code_entry_origin_(quick_code_entry_origin),
       name_class_(name_class),
       name_method_(name_method),
       signature_method_(signature_method),
       type_inputs_(type_output)
    {}

  private:
    bool is_static_;
    char type_output_;
    void* quick_code_entry_origin_;
    std::string name_class_;
    std::string name_method_;
    std::string signature_method_;
    std::vector<char> type_inputs_;
};

// The gadget to extract JNI handle from TLS.
extern "C" void GetJniEnv(JNIEnv**) __asm__("GetJniEnv");

// The gadget to insert an object into the designated indirect reference table.
// Note that the first argument is the pointer to art::IndirectReferenceTable.
extern "C" jobject AddIndirectReference(void*, uint32_t, void*)
                                        __asm__("AddIndirectReference");

// The gadget to remove a reference from the designated indirect reference table.
// Note that the first argument is the pointer to art::IndirectReferenceTable.
extern "C" bool RemoveIndirectReference(void*, uint32_t, jobject)
                                        __asm__("RemoveIndirectReference");

// The gadget to decode the given indirect reference.
// Note that the first argument is the pointer to art::Thread.
extern "C" void* DecodeJObject(void*, jobject) __asm__("DecodeJObject");

// The trampoline to the function to set instrument gadget composer.
extern "C" void* ComposeInstrumentGadgetTrampoline()
                                        __asm__("ComposeInstrumentGadgetTrampoline");

// The function which launches the composer that will set all the instrument
// gadgets towards user designated Java methods for instrumentation.
extern "C" void* ComposeInstrumentGadget(void*, void*, void*, void*, void*);


// The cached symbols delivered from injector.
extern char* g_module_path;
extern char* g_lib_path;
extern char* g_class_name;

// The cached Java VM handle.
extern JavaVM* g_jvm;

// The original entry to IndirectReferenceTable::Add().
extern void* g_indirect_reference_table_add;

// The original entry to IndirectReferneceTable::Remove().
extern void* g_indirect_reference_table_remove;

// The original entry to Thread::DecodeJObject().
extern void* g_thread_decode_jobject;

// The original entry to the loadClass() quick compiled code.
extern void* g_load_class_quick_compiled;

// The cached class and object instance of analysis module.
extern jclass g_class_analysis_main;
extern jobject g_obj_analysis_main;

// The global map to maintain the information about all the instrumented methods
// of the target app.
typedef std::unique_ptr<std::unordered_map<void*, std::unique_ptr<MethodBundleNative>>>
        PtrBundleMap;
extern PtrBundleMap g_map_method_bundle;

#endif
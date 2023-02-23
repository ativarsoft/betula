#include <templatizer.h>
#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Target.h>
#include <llvm-c/BitWriter.h>

LLVMModuleRef mod;
LLVMBuilderRef builder;
LLVMValueRef current_fn;

int pollen_codegen_sanity_check()
{
    LLVMModuleRef mod = LLVMModuleCreateWithName("test_module");

    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef fn_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef sum_fn = LLVMAddFunction(mod, "sum", fn_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum_fn, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    LLVMValueRef x = LLVMGetParam(sum_fn, 0);
    LLVMValueRef y = LLVMGetParam(sum_fn, 1);
    LLVMValueRef sum = LLVMBuildAdd(builder, x, y, "sum");
    LLVMBuildRet(builder, sum);

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    if (LLVMCreateJITCompilerForModule(&engine, mod, 2, &error) != 0) {
        fprintf(stderr, "failed to create JIT compiler: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    int (*sum_ptr)(int, int) = (int (*)(int, int))LLVMGetFunctionAddress(engine, "sum");
    int result = sum_ptr(2, 3);
    printf("Codegen-Sanity-Check: 2 plus 3 equals %d\r\n", result);

    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "test.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);

    return 0;
}

int gen_add_node()
{
    LLVMValueRef x = LLVMGetParam(current_fn, 0);
    LLVMValueRef y = LLVMGetParam(current_fn, 1);
    LLVMValueRef sum = LLVMBuildAdd(builder, x, y, "sum");
    LLVMBuildRet(builder, sum);
    return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
    cb->register_element_start_tag(data, "add", gen_add_node);
    mod = LLVMModuleCreateWithName("test_module");
    return 0;
}

static void quit()
{
    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    if (LLVMCreateJITCompilerForModule(&engine, mod, 2, &error) != 0) {
        fprintf(stderr, "failed to create JIT compiler: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    int (*sum_ptr)(int, int) = (int (*)(int, int))LLVMGetFunctionAddress(engine, "sum");
    int result = sum_ptr(2, 3);
    printf("Codegen-Sanity-Check: 2 plus 3 equals %d\r\n", result);

    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "test.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);
}

struct templatizer_plugin templatizer_plugin_v1 = {
    &init,
    &quit
};
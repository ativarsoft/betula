/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Target.h>
#include "llvm-c/TargetMachine.h"
#include <llvm-c/BitWriter.h>

static thread_local LLVMModuleRef mod;
static thread_local LLVMBuilderRef builder;
static thread_local LLVMValueRef current_fn;
static thread_local LLVMValueRef last_expr;

static thread_local const char *script_name = NULL;

int pollen_codegen_sanity_check()
{
    LLVMModuleRef mod = LLVMModuleCreateWithName("test_module");

    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef fn_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef sum_fn = LLVMAddFunction(mod, "pollen_main", fn_type);

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

    int (*sum_ptr)(int, int) = (int (*)(int, int))LLVMGetFunctionAddress(engine, "pollen_main");
    int result = sum_ptr(2, 3);
    printf("Codegen-Sanity-Check: 2 plus 3 equals %d\r\n", result);

    /* Write out bitcode to file (LLVM IR bitcode) */
    if (LLVMWriteBitcodeToFile(mod, "test.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    /*
     * Write object file (".o" file)
     */
    LLVMTargetRef target = LLVMGetFirstTarget();
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, LLVMGetDefaultTargetTriple(), "x86-64", "", LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

    if (LLVMTargetMachineEmitToFile(target_machine, mod, "sanity.o", LLVMObjectFile, &error) != 0) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        return 1;
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);

    return 0;
}

static int gen_add_node_start(tmpl_ctx_t ctx, const char *el, const char **attr)
{
    const char *x_str = NULL;
    const char *y_str = NULL;
    for (int i = 0; attr[i] != NULL; i += 2) {
        const char *name = attr[i];
        const char *value = attr[i+1];
        if (strcmp(name, "x") == 0) {
            x_str = value;
        } else if (strcmp(name, "y") == 0) {
            y_str = value;
        }
    }

    if (x_str == NULL || y_str == NULL) {
        fprintf(stderr, "error: x or y attribute missing from add element\n");
        return -1;
    }

    int x = atoi(x_str);
    int y = atoi(y_str);
    //LLVMValueRef sum = LLVMBuildAdd(builder, x, y, "sum");
    //LLVMBuildRet(builder, sum);
    return 0;
}

static int gen_add_node_end(tmpl_ctx_t ctx, const char *el)
{
    return 0;
}

static int gen_sub_node_start(tmpl_ctx_t ctx, const char *el, const char **attr)
{
    const char *x_str = NULL;
    const char *y_str = NULL;
    for (int i = 0; attr[i] != NULL; i += 2) {
        const char *name = attr[i];
        const char *value = attr[i+1];
        if (strcmp(name, "x") == 0) {
            x_str = value;
        } else if (strcmp(name, "y") == 0) {
            y_str = value;
        }
    }

    if (x_str == NULL || y_str == NULL) {
        fprintf(stderr, "error: x or y attribute missing from sub element\n");
        return -1;
    }

    int x = atoi(x_str);
    int y = atoi(y_str);
    //LLVMValueRef diff = LLVMBuildSub(builder, x, y, "diff");
    //LLVMBuildRet(builder, diff);
    return 0;
}

static int gen_sub_node_end(tmpl_ctx_t ctx, const char *el)
{
    return 0;
}

static int gen_mul_node_start(tmpl_ctx_t ctx, const char *el, const char **attr)
{
    const char *x_str = NULL;
    const char *y_str = NULL;
    for (int i = 0; attr[i] != NULL; i += 2) {
        const char *name = attr[i];
        const char *value = attr[i+1];
        if (strcmp(name, "x") == 0) {
            x_str = value;
        } else if (strcmp(name, "y") == 0) {
            y_str = value;
        }
    }

    if (x_str == NULL && y_str == NULL) {
        fprintf(stderr, "error: x and y attributes missing from mul element\n");
        return -1;
    } else if (x_str == NULL) {
    	fprintf(stderr, "error: x attribute missing from mul element\n");
        return -1;
    } else if (y_str == NULL) {
        int x = atoi(x_str);
        //LLVMValueRef last = LLVMBuildLoad(builder, last_expr, "x_val");
        //LLVMValueRef c = LLVMConstInt(LLVMTypeOf(last_expr), atoi(x_str), 1);
        //LLVMValueRef prod = LLVMBuildMul(builder, last, c, "prod");
        //last_expr = prod;
    } else if (x_str != NULL && y_str != NULL) {
    	int x = atoi(x_str);
        int y = atoi(y_str);
        //LLVMValueRef prod = LLVMBuildMul(builder, x, y, "prod");
        //last_expr = prod;
    } else {
    	abort();
    }
    return 0;
}

static int gen_mul_node_end(tmpl_ctx_t ctx, const char *el)
{
    return 0;
}

static int gen_div_node_start(tmpl_ctx_t ctx, const char *el, const char **attr)
{
    const char *x_str = NULL;
    const char *y_str = NULL;
    for (int i = 0; attr[i] != NULL; i += 2) {
        const char *name = attr[i];
        const char *value = attr[i+1];
        if (strcmp(name, "x") == 0) {
            x_str = value;
        } else if (strcmp(name, "y") == 0) {
            y_str = value;
        }
    }

    if (x_str == NULL || y_str == NULL) {
        fprintf(stderr, "error: x or y attribute missing from div element\n");
        return -1;
    }

    int x = atoi(x_str);
    int y = atoi(y_str);
    //LLVMValueRef quot = LLVMBuildSDiv(builder, x, y, "quot");
    //LLVMBuildRet(builder, quot);
    return 0;
}

static int gen_div_node_end(tmpl_ctx_t ctx, const char *el)
{
    return 0;
}

static int init(tmpl_ctx_t data, tmpl_cb_t cb)
{
    //script_name = cb->get_script_name(data);
    script_name = "expr.tmpl";
    cb->register_element_start_tag(data, "add", gen_add_node_start);
    cb->register_element_end_tag(data, "add", gen_add_node_end);
    cb->register_element_start_tag(data, "sub", gen_sub_node_start);
    cb->register_element_end_tag(data, "sub", gen_sub_node_end);
    cb->register_element_start_tag(data, "mul", gen_mul_node_start);
    cb->register_element_end_tag(data, "mul", gen_mul_node_end);
    cb->register_element_start_tag(data, "div", gen_div_node_start);
    cb->register_element_end_tag(data, "div", gen_div_node_end);

    LLVMModuleRef mod = LLVMModuleCreateWithName("test_module");

    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef fn_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef main_fn = LLVMAddFunction(mod, "pollen_main", fn_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_fn, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    current_fn = main_fn;
    return 0;
}

static void quit()
{
    LLVMBuildRet(builder, current_fn);

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

#if 1
    int (*test_fn_ptr)(int, int) = (int (*)(int, int))LLVMGetFunctionAddress(engine, "pollen_main");
    int result = test_fn_ptr(2, 3);
    printf("Codegen-Sanity-Check: test function returned %d\r\n", result);
#endif

    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "test.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);
}

const tmpl_plugin_record_t templatizer_plugin_v1 = {
    &init,
    &quit
};

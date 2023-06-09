#include "test/wagic_wrapper.hpp"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <psppower.h>
#include <pspsdk.h>
#include <pspaudiocodec.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspmpeg.h>
#include <malloc.h>
#include <string.h>

// Profiling
// #include <pspprof.h>

#include <pspctrl.h>
#include <unistd.h>
#include <stdio.h>
#include <queue>

#include "JGE.h"
#include "JApp.h"
#include "JGameLauncher.h"
#include "JRenderer.h"
#include "JLogger.h"

#ifndef JGEApp_Title
#define JGEApp_Title "JGE++"
#endif

#ifdef DEVHOOK
PSP_MODULE_INFO(JGEApp_Title, 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
// 256 is not enough for the network to correctly start,
//  let's find an appropriate value the day JGE has working network
PSP_HEAP_SIZE_KB(-256);

#else

PSP_MODULE_INFO(JGEApp_Title, 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#endif


u32 gTickFrequency = 1;

//------------------------------------------------------------------------------------------------
// Exit callback
int exit_callback(int arg1, int arg2, void* common) {
    sceKernelExitGame();
    return 0;
}

//------------------------------------------------------------------------------------------------
// Power Callback
int power_callback(int unknown, int pwrflags, void* common) {
    if ((pwrflags & (PSP_POWER_CB_POWER_SWITCH | PSP_POWER_CB_STANDBY)) > 0) {
        // suspending
    } else if ((pwrflags & PSP_POWER_CB_RESUME_COMPLETE) > 0) {
        sceKernelDelayThread(1500000);
        // resume complete
    }
    return 0;
}

//------------------------------------------------------------------------------------------------
// Callback thread
int CallbackThread(SceSize args, void* argp) {
    int cbid;

#ifdef DEVHOOK
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
#endif
    cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(0, cbid);

    sceKernelSleepThreadCB();

    return 0;
}

// Sets up the callback thread and returns its thread id
int SetupCallbacks(void) {
    int thid = 0;

    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
    return thid;
}

#ifdef DEVHOOK
// code by sakya, crazyc, samuraiX
// http://forums.ps2dev.org/viewtopic.php?t=9591
PspDebugRegBlock exception_regs;

extern int _ftext;

static const char* codeTxt[32] = {"Interrupt",
                                  "TLB modification",
                                  "TLB load/inst fetch",
                                  "TLB store",
                                  "Address load/inst fetch",
                                  "Address store",
                                  "Bus error (instr)",
                                  "Bus error (data)",
                                  "Syscall",
                                  "Breakpoint",
                                  "Reserved instruction",
                                  "Coprocessor unusable",
                                  "Arithmetic overflow",
                                  "Unknown 14",
                                  "Unknown 15",
                                  "Unknown 16",
                                  "Unknown 17",
                                  "Unknown 18",
                                  "Unknown 19",
                                  "Unknown 20",
                                  "Unknown 21",
                                  "Unknown 22",
                                  "Unknown 23",
                                  "Unknown 24",
                                  "Unknown 25",
                                  "Unknown 26",
                                  "Unknown 27",
                                  "Unknown 28",
                                  "Unknown 29",
                                  "Unknown 31"};

static const unsigned char regName[32][5] = {"zr", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2",
                                             "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5",
                                             "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

void ExceptionHandler(PspDebugRegBlock* regs) {
    int i;
    SceCtrlData pad;

    pspDebugScreenInit();
    pspDebugScreenSetBackColor(0x00FF0000);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenClear();
    pspDebugScreenPrintf("Your PSP has just crashed!\n");
    pspDebugScreenPrintf("Exception details:\n\n");

    pspDebugScreenPrintf("Last Log Message: \n%s\n\n", JLogger::lastLog.c_str());
    pspDebugScreenPrintf("Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
    pspDebugScreenPrintf("EPC       - %08X / %s.text + %08X\n", (int)regs->epc, module_info.modname,
                         (unsigned int)(regs->epc - (int)&_ftext));
    pspDebugScreenPrintf("Cause     - %08X\n", (int)regs->cause);
    pspDebugScreenPrintf("Status    - %08X\n", (int)regs->status);
    pspDebugScreenPrintf("BadVAddr  - %08X\n", (int)regs->badvaddr);
    for (i = 0; i < 32; i += 4)
        pspDebugScreenPrintf("%s:%08X %s:%08X %s:%08X %s:%08X\n", regName[i], (int)regs->r[i], regName[i + 1],
                             (int)regs->r[i + 1], regName[i + 2], (int)regs->r[i + 2], regName[i + 3],
                             (int)regs->r[i + 3]);

    sceKernelDelayThread(1000000);
    pspDebugScreenPrintf("\n\nPress X to dump information on file exception.log and quit");
    pspDebugScreenPrintf("\nPress O to quit");

    for (;;) {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_CROSS) {
            FILE* log = fopen("exception.log", "w");
            if (log != NULL) {
                char testo[1024];
                pspDebugScreenPrintf("Last Log Message: \n%s\n\n", JLogger::lastLog.c_str());
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "Exception details:\n\n");
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "EPC       - %08X / %s.text + %08X\n", (int)regs->epc, module_info.modname,
                        (unsigned int)(regs->epc - (int)&_ftext));
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "Cause     - %08X\n", (int)regs->cause);
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "Status    - %08X\n", (int)regs->status);
                fwrite(testo, 1, strlen(testo), log);
                sprintf(testo, "BadVAddr  - %08X\n", (int)regs->badvaddr);
                fwrite(testo, 1, strlen(testo), log);
                for (i = 0; i < 32; i += 4) {
                    sprintf(testo, "%s:%08X %s:%08X %s:%08X %s:%08X\n", regName[i], (int)regs->r[i], regName[i + 1],
                            (int)regs->r[i + 1], regName[i + 2], (int)regs->r[i + 2], regName[i + 3],
                            (int)regs->r[i + 3]);
                    fwrite(testo, 1, strlen(testo), log);
                }
                fclose(log);
            }
            break;
        } else if (pad.Buttons & PSP_CTRL_CIRCLE)
            break;
        sceKernelDelayThread(100000);
    }
    sceKernelExitGame();
}

void initExceptionHandler() {
    SceKernelLMOption option;
    int args[2], fd, modid;

    memset(&option, 0, sizeof(option));
    option.size     = sizeof(option);
    option.mpidtext = PSP_MEMORY_PARTITION_KERNEL;
    option.mpiddata = PSP_MEMORY_PARTITION_KERNEL;
    option.position = 0;
    option.access   = 1;

    if ((modid = sceKernelLoadModule("exception.prx", 0, &option)) >= 0) {
        args[0] = (int)ExceptionHandler;
        args[1] = (int)&exception_regs;
        sceKernelStartModule(modid, 8, args, &fd, NULL);
    }
}
#else
//------------------------------------------------------------------------------------------------
// Custom exception handler
void MyExceptionHandler(PspDebugRegBlock* regs) {
    pspDebugScreenInit();

    pspDebugScreenSetBackColor(0x00FF0000);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenClear();

    pspDebugScreenPrintf("I regret to inform you your psp has just crashed\n");
    pspDebugScreenPrintf("Please contact Sony technical support for further information\n\n");
    pspDebugScreenPrintf("Exception Details:\n");
    pspDebugDumpException(regs);
    pspDebugScreenPrintf("\nBlame the 3rd party software, it cannot possibly be our fault!\n");

    sceKernelExitGame();
}

//------------------------------------------------------------------------------------------------
// Sort of hack to install exception handler under USER THREAD
__attribute__((constructor)) void handlerInit() {
    pspKernelSetKernelPC();

    pspSdkInstallNoDeviceCheckPatch();
    pspSdkInstallNoPlainModuleCheckPatch();
    pspSdkInstallKernelLoadModulePatch();

    pspDebugInstallErrorHandler(MyExceptionHandler);
}

#endif

void JGECreateDefaultBindings() {}

int JGEGetTime() {
    u64 curr;
    sceRtcGetCurrentTick(&curr);
    return (int)((curr * 1000) / gTickFrequency);
}

u8 JGEGetAnalogX() { return 0; }
u8 JGEGetAnalogY() { return 0; }

#include "test/wagic_wrapper.hpp"

#include "GameOptions.h"
#include "MTGDeck.h"
#include "TestSuiteAI.h"

#include <JApp.h>
#include <JGE.h>
#include <JGameLauncher.h>

//------------------------------------------------------------------------------------------------
// The main loop
int main(int argc, char* argv[]) {
    pspDebugScreenInit();

    pspDebugScreenSetBackColor(0x00000000);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenClear();

    pspDebugScreenPrintf("JGE:Loading application...");

    JLOG("SetupCallbacks()");
    SetupCallbacks();
#ifdef DEVHOOK
    JLOG("initExceptionHandler()");
    initExceptionHandler();
#endif

    {
        WagicWrapper* wagic_core = new WagicWrapper();

        MTGCollection()->loadFolder("sets/primitives/");
        MTGCollection()->loadFolder("sets/", "_cards.dat");
        options.reloadProfile();

        TestSuite testSuite("test/_tests.txt");

        const size_t result = testSuite.run();

        const size_t total = testSuite.nbTests + testSuite.nbAITests;
        delete wagic_core;

        char buffer[100];
        snprintf(buffer, 100, "done: failed test: %d out of %d total\n", (int)result, (int)total);

        pspDebugScreenPrintf(buffer);
        JLOG(buffer);
        printf(buffer);
    }

    // Profiling
    // gprof_cleanup();
    sceKernelExitGame();
    return 0;
}

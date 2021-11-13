#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"

#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

// fancy struct for patches for kittyMemory
struct My_Patches {
    // let's assume we have patches for these functions for whatever game
    // like show in miniMap boolean function 
    MemoryPatch GodMode, GodMode2;
    // etc...
} hexPatches;

bool feature1 = false, feature2 = false, goldhack = false;
int sliderValue = 1, level = 0;
void *instanceBtn;

// Hooking examples. Assuming you know how to write hook


int (*old_get_IntGold)(void *instance);
int get_IntGold(void *instance) {
    if (instance != NULL && goldhack) {
        return 10000;
    }
    return old_get_IntGold(instance);
}

int (*old_get_IntLevel)(void *instance);
int get_IntLevel(void *instance) {
    if (instance != NULL && level) {
        return (int)level;
    }
    return old_get_IntLevel(instance);
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it still works
    // New way to patch hex via KittyMemory without need to  specify len. Spaces or without spaces are fine
    // ARM64 assembly exampleh 
    // MOV X0, #0x0 = 00 00 80 D2
    // RET = C0 03 5F D6
	  										                                 
     
    HOOK_LIB("libil2cpp.so", "0x123456", get_IntGold, old_get_IntGold);
    
    HOOK_LIB("libil2cpp.so", "0x123456", get_IntLevel, old_get_IntLevel);
			
                                                            
#else 


    LOGI(OBFUSCATE("Done"));
#endif

    return NULL;
}

//JNI calls
extern "C" {

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

JNIEXPORT jobjectArray
JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    //MakeToast(env, context, OBFUSCATE("Renchon Mod"), Toast::LENGTH_LONG);
    
    const char *features[] = {          
    　      OBFUSCATE("0_InputValue_レベル変更"),//Max value
         　 OBFUSCATE("1_CheckBox_無制限のコイン"),
            
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    pthread_t ptid;
    pthread_create(&ptid, NULL, antiLeech, NULL);

    return (ret);
}

JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 0:
            level = value;           
            break;
        case 1:
            goldhack = boolean;
            break;        	 　
    }
}
}

//No need to use JNI_OnLoad, since we don't use JNIEnv
//We do this to hide OnLoad from disassembler
__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
 

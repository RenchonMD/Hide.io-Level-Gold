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

#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

struct My_Patches {
    MemoryPatch GodMode, GodMode2;
} hexPatches;

bool feature1 = false, feature2 = false, goldhack = false;
int sliderValue = 1, level = 0;
void *instanceBtn;


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


void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) 
	  										                                    
    HOOK_LIB("libil2cpp.so", "0x123456", get_IntGold, old_get_IntGold);
    
    HOOK_LIB("libil2cpp.so", "0x123456", get_IntLevel, old_get_IntLevel);
			
                                                            
#else 


    LOGI(OBFUSCATE("Done"));
#endif

    return NULL;
}

extern "C" {


JNIEXPORT jobjectArray
JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;
    
    const char *features[] = {          
    　      OBFUSCATE("0_InputValue_レベル変更"),//Max value
         　 OBFUSCATE("1_CheckBox_無制限のコイン"),           
    };

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

__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
 

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.27
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#include <jni.h>
#include <stdlib.h>
#include <string.h>


#include "StochsimWrapper.hh"
#include "StochsimWrapperMapper.h"


JNIEXPORT jlong JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_create1(JNIEnv *jenv, jclass jcls, jstring jarg1, jint jarg2) {
    jlong jresult = 0 ;
    char *arg1 = (char *) 0 ;
    int arg2 ;
    StochsimWrapper *result;
    
    (void)jenv;
    (void)jcls;
    {
        arg1 = 0;
        if (jarg1) {
            arg1 = (char *)jenv->GetStringUTFChars(jarg1, 0);
            if (!arg1) return 0;
        }
    }
    arg2 = (int)jarg2; 
    result = (StochsimWrapper *)new StochsimWrapper((char const *)arg1,arg2);
    
    *(StochsimWrapper **)(void *)&jresult = result; 
    {
        if (arg1) jenv->ReleaseStringUTFChars(jarg1, arg1); 
    }
    return jresult;
}

JNIEXPORT jlong JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_create2(JNIEnv *jenv, jclass jcls, jstring jarg1, jstring jarg2, jint jarg3) {
    jlong jresult = 0 ;
    char *arg1 = (char *) 0 ;
    char *arg2 = (char *) 0 ;
    int arg3 ;
    StochsimWrapper *result;
    
    (void)jenv;
    (void)jcls;
    {
        arg1 = 0;
        if (jarg1) {
            arg1 = (char *)jenv->GetStringUTFChars(jarg1, 0);
            if (!arg1) return 0;
        }
    }
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return 0;
        }
    }
    arg3 = (int)jarg3; 
    result = (StochsimWrapper *)new StochsimWrapper((char const *)arg1,(char const *)arg2,arg3);
    
    *(StochsimWrapper **)(void *)&jresult = result; 
    {
        if (arg1) jenv->ReleaseStringUTFChars(jarg1, arg1); 
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2);
    }
    return jresult;
}


JNIEXPORT void JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_destroy(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    delete arg1;
    
}


JNIEXPORT void JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_finalise(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    (arg1)->finalise();
    
}


JNIEXPORT void JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_step(JNIEnv *jenv, jclass jcls, jlong jarg1, jdouble jarg2) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    double arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    arg2 = (double)jarg2; 
    (arg1)->step(arg2);
    
}


JNIEXPORT jdouble JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_getTime(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jdouble jresult = 0 ;
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    double result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    result = (double)(arg1)->getTime();
    
    jresult = (jdouble)result; 
    return jresult;
}


JNIEXPORT jdouble JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_getTimeInc(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jdouble jresult = 0 ;
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    double result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    result = (double)(arg1)->getTimeInc();
    
    jresult = (jdouble)result; 
    return jresult;
}


JNIEXPORT jint JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_getCopynumber(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
    jint jresult = 0 ;
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    char *arg2 = (char *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return 0;
        }
    }
    result = (long)(arg1)->getCopynumber((char const *)arg2);
    
    jresult = (jint)result; 
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
    return jresult;
}


JNIEXPORT jint JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_getOVValue(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
    jint jresult = 0 ;
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    char *arg2 = (char *) 0 ;
    long result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return 0;
        }
    }
    result = (long)(arg1)->getOVValue((char const *)arg2);
    
    jresult = (jint)result; 
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
    return jresult;
}


JNIEXPORT void JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_setDynamicValue(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2, jdouble jarg3) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    char *arg2 = (char *) 0 ;
    double arg3 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return ;
        }
    }
    arg3 = (double)jarg3; 
    (arg1)->setDynamicValue((char const *)arg2,arg3);
    
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
}


JNIEXPORT void JNICALL  Java_agentCell_1re_networks_StochsimWrapperMapper_ErrOut(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2, jint jarg3) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    char *arg2 = (char *) 0 ;
    int arg3 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return ;
        }
    }
    arg3 = (int)jarg3; 
    (arg1)->ErrOut((char const *)arg2,arg3);
    
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
}


JNIEXPORT void JNICALL Java_agentCell_1re_networks_StochsimWrapperMapper_StatusOut(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
    StochsimWrapper *arg1 = (StochsimWrapper *) 0 ;
    char *arg2 = (char *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(StochsimWrapper **)(void *)&jarg1; 
    {
        arg2 = 0;
        if (jarg2) {
            arg2 = (char *)jenv->GetStringUTFChars(jarg2, 0);
            if (!arg2) return ;
        }
    }
    (arg1)->StatusOut((char const *)arg2);
    
    {
        if (arg2) jenv->ReleaseStringUTFChars(jarg2, arg2); 
    }
}

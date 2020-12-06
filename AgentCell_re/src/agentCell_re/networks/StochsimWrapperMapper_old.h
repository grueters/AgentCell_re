/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class edu_uchicago_agentcell_networks_StochsimWrapperMapper */

#ifndef _Included_edu_uchicago_agentcell_networks_StochsimWrapperMapper
#define _Included_edu_uchicago_agentcell_networks_StochsimWrapperMapper
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    create
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_create
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_destroy
  (JNIEnv *, jclass, jlong);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    finalise
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_finalise
  (JNIEnv *, jclass, jlong);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    step
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_step
  (JNIEnv *, jclass, jlong, jdouble);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    getTime
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_getTime
  (JNIEnv *, jclass, jlong);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    getCopynumber
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_getCopynumber
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    setDynamicValue
 * Signature: (JLjava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_setDynamicValue
  (JNIEnv *, jclass, jlong, jstring, jdouble);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    ErrOut
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_ErrOut
  (JNIEnv *, jclass, jlong, jstring, jint);

/*
 * Class:     edu_uchicago_agentcell_networks_StochsimWrapperMapper
 * Method:    StatusOut
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_uchicago_agentcell_networks_StochsimWrapperMapper_StatusOut
  (JNIEnv *, jclass, jlong, jstring);

#ifdef __cplusplus
}
#endif
#endif
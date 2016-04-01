/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class s_android_ffmpeg_FFmpegNative */


#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "JNI_LIB_VIDEO", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "JNI_LIB_VIDEO", __VA_ARGS__)

#ifndef _Included_s_android_ffmpeg_FFmpegNative
#define _Included_s_android_ffmpeg_FFmpegNative
#ifdef __cplusplus
extern "C" {
#endif

#include "include/jni.h"
#include "include/stdio.h"
#include"android/log.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"

int targetHeigth;
int targetWidth;

AVPacket packet;
AVCodec *codec;
static AVCodecContext *g_pCodecCtx = NULL;
//AVFrame描述一个多媒体帧。解码后的数据将被放在其中
static AVFrame *g_pavfFrame = NULL;
struct SwsContext *swsctx = NULL;
AVFrame *picture = NULL;
AVCodecParserContext *avParserContext;

/*
 * Class:     s_android_ffmpeg_FFmpegNative
 * Method:    decoderInit
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_s_android_ffmpeg_FFmpegNative_decoderInit
        (JNIEnv *env, jclass obj, jint iwidth, jint iheight) {
    targetHeigth = iheight;
    targetWidth = iwidth;

    if (g_pCodecCtx != NULL) {
        //如果已经初始化过 那么不再初始化
        LOGD("Codec Context Ready,break Initail.");
        exit(0);
    }

    //解码初始化 使用解码库前必须先执行，挂载所有的解码器
    av_register_all();
    //查找h264解码器
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGE("decoder not found!");
        return -1;
    }
    g_pCodecCtx = avcodec_alloc_context3(codec);
    g_pCodecCtx->width = iwidth;
    g_pCodecCtx->height = iheight;
    //获取解码环境

    //开启解码器
    if (avcodec_open2(g_pCodecCtx, codec, NULL) < 0) {
        //分配视频帧
        LOGE("open decoder failed!");
        return -1;
    }
    g_pavfFrame = avcodec_alloc_frame();

    if (g_pavfFrame == NULL)
        return -1;

    avParserContext = av_parser_init(CODEC_ID_H264);

    av_init_packet(&packet);
    picture = avcodec_alloc_frame();
    LOGD("initial decoder finish!");
    return 0;
};

/*
 * Class:     s_android_ffmpeg_FFmpegNative
 * Method:    decodeData
 * Signature: ([BI[B)I
 */
JNIEXPORT jint JNICALL Java_s_android_ffmpeg_FFmpegNative_decodeData
        (JNIEnv *env, jclass thiz, jbyteArray in, jint nalLen, jbyteArray out) {
    int frameFinished; //解码完成标识
    int consumed_bytes;

    jbyte *Buf = (jbyte *) (*env)->GetByteArrayElements(env, in, 0);
    jbyte *Pixel = (jbyte *) (*env)->GetByteArrayElements(env, out, 0);

    packet.data = Buf;
    packet.size = nalLen;

    //解码
    consumed_bytes = avcodec_decode_video2(g_pCodecCtx, g_pavfFrame, &frameFinished, &packet);

    if (frameFinished > 0) {
        //初始化
        swsctx = sws_getContext(g_pCodecCtx->width, g_pCodecCtx->height, g_pCodecCtx->pix_fmt,
                                targetWidth, targetHeigth, PIX_FMT_RGB565, SWS_BICUBIC, NULL, NULL,
                                NULL);
        if (swsctx != NULL) {
            //将Pixel填充至picture作为输出的缓冲区
            avpicture_fill((AVPicture *) picture, (uint8_t *) Pixel, PIX_FMT_RGB565, targetWidth,
                           targetHeigth);
            sws_scale(swsctx, (const uint8_t *const *) g_pavfFrame->data, g_pavfFrame->linesize, 0,
                      g_pCodecCtx->height, picture->data, picture->linesize);
        } else {
            LOGD("GET SWSCTX FAILED");
        }
    } else {
        //缓冲不足以解码一帧
    }

    (*env)->ReleaseByteArrayElements(env, in, Buf, 0);
    (*env)->ReleaseByteArrayElements(env, out, Pixel, 0);
    return consumed_bytes;
};

/*
 * Class:     s_android_ffmpeg_FFmpegNative
 * Method:    releaseDecoder
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_s_android_ffmpeg_FFmpegNative_releaseDecoder
        (JNIEnv *env, jclass thiz) {
    //	delete[]yuv_buff;
    //	delete[]rgb_buff;
    sws_freeContext(swsctx);
    if (g_pavfFrame != NULL) {
        av_free(g_pavfFrame);
        av_free(picture);
        g_pavfFrame = NULL;
    }

    if (g_pCodecCtx != NULL) {
        avcodec_close(g_pCodecCtx);
        av_free(g_pCodecCtx);
        g_pCodecCtx = NULL;
    }
    LOGD("release decoder success!");
};

#ifdef __cplusplus
}
#endif
#endif

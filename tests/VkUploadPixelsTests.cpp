/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// This is a GPU-backend specific test. It relies on static intializers to work

#include "SkTypes.h"

#if defined(SK_VULKAN)

#include "GrContextFactory.h"
#include "GrContextPriv.h"
#include "GrSurfaceProxy.h"
#include "GrTest.h"
#include "ProxyUtils.h"
#include "SkGr.h"
#include "Test.h"
#include "TestUtils.h"
#include "vk/GrVkGpu.h"

using sk_gpu_test::GrContextFactory;

void basic_texture_test(skiatest::Reporter* reporter, GrContext* context, SkColorType ct,
                        bool renderTarget) {
    const int kWidth = 16;
    const int kHeight = 16;
    SkAutoTMalloc<GrColor> srcBuffer(kWidth*kHeight);
    SkAutoTMalloc<GrColor> dstBuffer(kWidth*kHeight);

    fill_pixel_data(kWidth, kHeight, srcBuffer.get());

    auto proxy = sk_gpu_test::MakeTextureProxyFromData(context, renderTarget, kWidth, kHeight, ct,
                                                       kTopLeft_GrSurfaceOrigin, srcBuffer, 0);
    REPORTER_ASSERT(reporter, proxy);
    if (proxy) {
        sk_sp<GrSurfaceContext> sContext = context->contextPriv().makeWrappedSurfaceContext(proxy);

        SkImageInfo dstInfo = SkImageInfo::Make(kWidth, kHeight, ct, kPremul_SkAlphaType);

        bool result = sContext->readPixels(dstInfo, dstBuffer, 0, 0, 0);
        REPORTER_ASSERT(reporter, result);
        REPORTER_ASSERT(reporter, does_full_buffer_contain_correct_color(srcBuffer,
                                                                         dstBuffer,
                                                                         kWidth,
                                                                         kHeight));

        dstInfo = SkImageInfo::Make(10, 2, ct, kPremul_SkAlphaType);
        result = sContext->writePixels(dstInfo, srcBuffer, 0, 2, 10);
        REPORTER_ASSERT(reporter, result);

        memset(dstBuffer, 0, kWidth*kHeight*sizeof(GrColor));

        result = sContext->readPixels(dstInfo, dstBuffer, 0, 2, 10);
        REPORTER_ASSERT(reporter, result);

        REPORTER_ASSERT(reporter, does_full_buffer_contain_correct_color(srcBuffer,
                                                                         dstBuffer,
                                                                         10,
                                                                         2));
    }

    proxy = sk_gpu_test::MakeTextureProxyFromData(context, renderTarget, kWidth, kHeight, ct,
                                                  kBottomLeft_GrSurfaceOrigin, srcBuffer, 0);
    REPORTER_ASSERT(reporter, proxy);
    if (proxy) {
        sk_sp<GrSurfaceContext> sContext = context->contextPriv().makeWrappedSurfaceContext(proxy);

        SkImageInfo dstInfo = SkImageInfo::Make(kWidth, kHeight, ct, kPremul_SkAlphaType);

        bool result = sContext->readPixels(dstInfo, dstBuffer, 0, 0, 0);
        REPORTER_ASSERT(reporter, result);
        REPORTER_ASSERT(reporter, does_full_buffer_contain_correct_color(srcBuffer,
                                                                         dstBuffer,
                                                                         kWidth,
                                                                         kHeight));

        dstInfo = SkImageInfo::Make(4, 5, ct, kPremul_SkAlphaType);
        result = sContext->writePixels(dstInfo, srcBuffer, 0, 5, 4);
        REPORTER_ASSERT(reporter, result);

        memset(dstBuffer, 0, kWidth*kHeight*sizeof(GrColor));

        result = sContext->readPixels(dstInfo, dstBuffer, 0, 5, 4);
        REPORTER_ASSERT(reporter, result);

        REPORTER_ASSERT(reporter, does_full_buffer_contain_correct_color(srcBuffer,
                                                                         dstBuffer,
                                                                         4,
                                                                         5));

    }
}

DEF_GPUTEST_FOR_VULKAN_CONTEXT(VkUploadPixelsTests, reporter, ctxInfo) {
    // RGBA
    basic_texture_test(reporter, ctxInfo.grContext(), kRGBA_8888_SkColorType, false);
    basic_texture_test(reporter, ctxInfo.grContext(), kRGBA_8888_SkColorType, true);

    // BGRA
    basic_texture_test(reporter, ctxInfo.grContext(), kBGRA_8888_SkColorType, false);
    basic_texture_test(reporter, ctxInfo.grContext(), kBGRA_8888_SkColorType, true);
}

#endif

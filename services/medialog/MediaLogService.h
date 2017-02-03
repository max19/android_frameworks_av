/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_MEDIA_LOG_SERVICE_H
#define ANDROID_MEDIA_LOG_SERVICE_H

#include <binder/BinderService.h>
#include <media/IMediaLogService.h>
#include <media/nbaio/NBLog.h>

namespace android {

class MediaLogService : public BinderService<MediaLogService>, public BnMediaLogService
{
    friend class BinderService<MediaLogService>;    // for MediaLogService()
public:
    MediaLogService() :
        BnMediaLogService(),
        mMergerShared((NBLog::Shared*) malloc(NBLog::Timeline::sharedSize(kMergeBufferSize))),
        mMerger(mMergerShared, kMergeBufferSize),
        mMergeReader(mMergerShared, kMergeBufferSize, mMerger)
    {ALOGI("Nico creating MergeReader");}
    virtual ~MediaLogService() { free(mMergerShared); }
    virtual void onFirstRef() { }

    static const char*  getServiceName() { return "media.log"; }

    static const size_t kMinSize = 0x100;
    static const size_t kMaxSize = 0x10000;
    virtual void        registerWriter(const sp<IMemory>& shared, size_t size, const char *name);
    virtual void        unregisterWriter(const sp<IMemory>& shared);

    virtual status_t    dump(int fd, const Vector<String16>& args);
    virtual status_t    onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                                uint32_t flags);

private:

    // Internal dump
    static const int kDumpLockRetries = 50;
    static const int kDumpLockSleepUs = 20000;
    static const size_t kMergeBufferSize = 16 * 1024; // TODO determine good value for this
    static bool dumpTryLock(Mutex& mutex);

    Mutex               mLock;

    Vector<NBLog::NamedReader> mNamedReaders;

    NBLog::Shared *mMergerShared;
    NBLog::Merger mMerger;
    NBLog::MergeReader mMergeReader;
};

}   // namespace android

#endif  // ANDROID_MEDIA_LOG_SERVICE_H

#ifndef POSIX_SEGMENT_H_
#define POSIX_SEGMENT_H_

#include <string>
#include "ME_OBJECT.h"

class PosixSegment {
public:
    explicit PosixSegment(uint64_t channel_id);
    ~PosixSegment();
    bool AcquireBlockToWrite();
    bool AcquireBlockToRead();
    void* getShmObject();
private:
    bool OpenOnly();
    bool OpenOrCreate();
    std::string shm_name_;
    std::size_t size_;
    void* managed_shm_;
};

#endif
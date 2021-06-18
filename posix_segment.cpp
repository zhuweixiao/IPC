#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>

#include "posix_segment.h"

PosixSegment::PosixSegment(uint64_t channel_id) {
    shm_name_ = std::to_string(channel_id);
    size_ = 1024;  // TODO: consider changing size
    managed_shm_ = nullptr;
}

PosixSegment::~PosixSegment() {
    managed_shm_ = nullptr;
}

void* PosixSegment::getShmObject() {
    return managed_shm_;
}

bool PosixSegment::AcquireBlockToWrite() {
    if (!OpenOrCreate()) {
        std::cout << "create shm failed, can't write now.\n";
        return false;
    }

    return true;
}

bool PosixSegment::AcquireBlockToRead() {
  if (!OpenOnly()) {
    std::cout << "failed to open shared memory, can't read now.\n";
    return false;
  }

  return true;
}

bool PosixSegment::OpenOrCreate() {
    // shm_open: return a file descriptor for the shared memory object
    int fd = shm_open(shm_name_.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        if (EEXIST == errno) {
            std::cout << "shm already exist, open only.\n";
            return OpenOnly();
        } else {
            std::cout << "create shm failed, error: \n";
            return false;
        }
    }
    // set the size of the memory segment.
    if (ftruncate(fd, size_) < 0) {
        std::cout << "ftruncate failed: \n";
        close(fd);
        return false;
    }
    // attach managed_shm_
    managed_shm_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    if (managed_shm_ == MAP_FAILED) {
        std::cout << "attach shm failed: " << std::endl;
        close(fd);
        shm_unlink(shm_name_.c_str());
        return false;
    }

    close(fd);

    return true;
}

bool PosixSegment::OpenOnly() {
    int fd = shm_open(shm_name_.c_str(), O_RDWR, 0644);
    if (fd == -1) {
        std::cout << "get shm failed: " << std::endl;
        return false;
    }

    struct stat file_attr;
    if (fstat(fd, &file_attr) < 0) {
        std::cout << "fstat failed: " << std::endl;
        close(fd);
        return false;
    }

    // attach managed_shm_
    managed_shm_ = mmap(nullptr, file_attr.st_size, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    if (managed_shm_ == MAP_FAILED) {
        std::cout << "attach shm failed: " << std::endl;
        close(fd);
        return false;
    }

    close(fd);

    return true;
}
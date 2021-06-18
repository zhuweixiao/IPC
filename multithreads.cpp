#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <string.h>
#include "posix_segment.h"

std::mutex mtx;
auto segment_ = std::make_shared<PosixSegment>(123456);

void send_30ms(ME_OBJECT* meObjSend) {
    // pass a pointer to this function
    if (!segment_->AcquireBlockToWrite()) {
        std::cout << "acquire block failed.\n";
    }
    //std::cout << "--aaa" << segment_->getShmObject() << "=+=" << std::endl;
    int8_t* buf = (int8_t *)segment_->getShmObject();
    if (buf == nullptr) {
        std::cout << "=*= Fail to get shared memory!\n";
    }

    while(1) {
        std::unique_lock<std::mutex> lk(mtx);
        meObjSend->OBJ_ID++; //for test
        memcpy(buf, meObjSend, sizeof(ME_OBJECT));
        std::cout << "send Object ID: " << std::to_string(*buf) << std::endl;
        lk.unlock();
        // sleep will not release lock
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void read_30ms(ME_OBJECT* meObjRead) {
    if (!segment_->AcquireBlockToRead()) {
        std::cout << "fail to acquire block\n";    
    }

    int8_t* buf = (int8_t *)segment_->getShmObject();
    if (buf == nullptr) {
        std::cout << "=*= Fail to get shared memory!\n";
    }

    while(1) {
        std::unique_lock<std::mutex> lk(mtx);
        memcpy(meObjRead, buf, 12);
        std::cout << "read Object ID: " << std::to_string(meObjRead->OBJ_ID) 
                  << "; H: " << meObjRead->OBJ_Height 
                  << ", W: " << meObjRead->OBJ_Width << std::endl;
        lk.unlock();
        // loop period: 30ms
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

int main() {
    // init
    ME_OBJECT meSend;
    ME_OBJECT meRead;
    meSend.OBJ_ID = 11;
    meSend.OBJ_Height = 2.3;
    meSend.OBJ_Width = 4.5;

    std::thread thread1(send_30ms, &meSend);
    std::thread thread2(read_30ms, &meRead);
    //std::thread thread3(send_30ms, &meSend);

    // wait
    thread1.join();
    thread2.join();
    //thread3.join();

    return 0;
}
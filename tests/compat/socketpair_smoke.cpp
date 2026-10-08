#include <dlfcn.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <initializer_list>
using Socketpair = int (*)(int,int,int,int*);
using Send = std::int64_t (*)(int,const void*,std::uint64_t,int);
using Recv = std::int64_t (*)(int,void*,std::uint64_t,int);
using Read = std::int64_t (*)(int,void*,std::uint64_t);
using Write = std::int64_t (*)(int,const void*,std::uint64_t);
using Close = int (*)(int);
template <typename T> T resolve(void* lib, const char* symbol) {
    auto f = reinterpret_cast<T>(dlsym(lib,symbol));
    if (!f) std::fprintf(stderr,"Missing NID: %s\n",symbol);
    return f;
}
int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "./build/core/libs/libs/libkernel.prx";
    void* lib = dlopen(path,RTLD_NOW|RTLD_LOCAL);
    if (!lib) { std::fprintf(stderr,"dlopen: %s\n",dlerror()); return 1; }
    auto pairFn=resolve<Socketpair>(lib,"MZb0GKT3mo8");
    auto sendFn=resolve<Send>(lib,"fZOeZIOEmLw");
    auto recvFn=resolve<Recv>(lib,"Ez8xjo9UF4E");
    auto readFn=resolve<Read>(lib,"AqBioC2vF3I");
    auto writeFn=resolve<Write>(lib,"FN4gaPmuFV8");
    auto closeFn=resolve<Close>(lib,"bY-PO6JhzhQ");
    if (!pairFn||!sendFn||!recvFn||!readFn||!writeFn||!closeFn) return 2;
    for (int type : {1,2}) {
        int fd[2]={-1,-1};
        if (pairFn(1,type,0,fd)!=0) return 3;
        const char message[]="AnyPS5 ping";
        char buffer[sizeof(message)]{};
        const auto bytes=sizeof(message);
        if (sendFn(fd[0],message,bytes,0)!=static_cast<std::int64_t>(bytes)) return 4;
        if (recvFn(fd[1],buffer,bytes,0)!=static_cast<std::int64_t>(bytes) || std::memcmp(message,buffer,bytes)) return 5;
        std::memset(buffer,0,bytes);
        if (writeFn(fd[1],message,bytes)!=static_cast<std::int64_t>(bytes)) return 6;
        if (readFn(fd[0],buffer,bytes)!=static_cast<std::int64_t>(bytes) || std::memcmp(message,buffer,bytes)) return 7;
        if (closeFn(fd[0])!=0 || closeFn(fd[1])!=0) return 8;
        if (sendFn(fd[0],message,bytes,0)!=-1) return 9;
    }
    if (pairFn(2,1,0,nullptr)!=-1) return 10;
    if (pairFn(1,1,0,nullptr)!=-1) return 11;
    dlclose(lib);
    std::puts("PASS: stream/datagram socketpair, both directions, close, invalid args");
    return 0;
}

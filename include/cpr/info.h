#ifndef CPR_INFO_H
#define CPR_INFO_H

#include <cstdint>

namespace cpr {

    class Info {
    public:
        Info(
            const double connect, const double pretransfer, const double start, const double elapsed,
            const uint64_t upload_size, const uint64_t upload_speed,
            const uint64_t download_size, const uint64_t download_speed 
            ) : connect(connect), pretransfer(pretransfer), start(start), elapsed(elapsed),
            upload_size(upload_size), upload_speed(upload_speed),
            download_size(download_size), download_speed(download_speed) {}

        double connect;
        double pretransfer;
        double start;
        double elapsed;

        uint64_t upload_size;
        uint64_t upload_speed;
        uint64_t download_size;
        uint64_t download_speed;
    };

} // namespace cpr

#endif
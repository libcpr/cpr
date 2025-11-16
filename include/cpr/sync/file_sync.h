#ifndef CPR_SYNC_FILE_SYNC_H
#define CPR_SYNC_FILE_SYNC_H

#include <cpr/cpr.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace cpr {
namespace sync {

// File sync status enum
enum class SyncStatus {
    PENDING,
    RUNNING,
    SUCCESS,
    FAILURE,
    PAUSED,
    CANCELLED,
    RESUMED
};

// File chunk information
struct FileChunk {
    size_t chunk_index;
    size_t chunk_size;
    std::string hash; // SHA-256 hash of the chunk
    off_t offset; // Offset in the original file
    bool is_transferred; // Whether the chunk has been transferred
    std::chrono::time_point<std::chrono::steady_clock> transfer_time;
};

// File metadata
struct FileMetadata {
    std::string file_id;
    std::string file_path;
    std::string file_name;
    size_t file_size;
    size_t chunk_size;
    int total_chunks;
    std::string file_hash; // SHA-256 hash of the entire file
    std::vector<FileChunk> chunks;
    std::chrono::time_point<std::chrono::steady_clock> last_modified;
    std::chrono::time_point<std::chrono::steady_clock> last_synced;
    std::string version;
};

// Sync progress information
struct SyncProgress {
    std::string sync_id;
    SyncStatus status;
    double progress; // Progress percentage (0-100)
    size_t transferred_chunks;
    size_t total_chunks;
    size_t transferred_bytes;
    size_t total_bytes;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
    std::string error_message;
    std::unordered_map<std::string, std::string> metadata;
};

// Sync configuration
struct SyncConfig {
    size_t chunk_size; // Default chunk size (default: 10MB)
    int max_parallel_chunks; // Maximum number of parallel chunk transfers
    bool enable_checksum; // Enable checksum verification
    bool enable_resume; // Enable resume support
    bool enable_incremental; // Enable incremental sync
    int max_retries; // Maximum number of retries per chunk
    std::chrono::milliseconds retry_delay; // Delay between retries
    std::chrono::milliseconds timeout; // Transfer timeout
    std::string temp_dir; // Temporary directory for chunk storage
};

// Conflict resolution strategy
enum class ConflictResolutionStrategy {
    KEEP_SOURCE, // Keep source file
    KEEP_TARGET, // Keep target file
    MERGE, // Merge files if possible
    RENAME_BOTH, // Rename both files
    MANUAL, // Require manual intervention
    LATEST, // Keep the latest modified file
    SIZE // Keep the largest file
};

// File conflict information
struct FileConflict {
    std::string conflict_id;
    std::string source_file_path;
    std::string target_file_path;
    FileMetadata source_metadata;
    FileMetadata target_metadata;
    ConflictResolutionStrategy suggested_strategy;
    std::chrono::time_point<std::chrono::steady_clock> detected_time;
};

// File sync interface
class FileSync {
public:
    virtual ~FileSync() = default;
    virtual std::string UploadFile(const std::string& local_path, const std::string& remote_path, const SyncConfig& config = {}) = 0;
    virtual std::string DownloadFile(const std::string& remote_path, const std::string& local_path, const SyncConfig& config = {}) = 0;
    virtual std::string SyncDirectory(const std::string& local_dir, const std::string& remote_dir, const SyncConfig& config = {}) = 0;
    virtual bool PauseSync(const std::string& sync_id) = 0;
    virtual bool ResumeSync(const std::string& sync_id) = 0;
    virtual bool CancelSync(const std::string& sync_id) = 0;
    virtual SyncProgress GetSyncProgress(const std::string& sync_id) = 0;
    virtual bool VerifyFile(const std::string& file_path, const FileMetadata& metadata) = 0;
    virtual FileMetadata GenerateFileMetadata(const std::string& file_path, size_t chunk_size = 10 * 1024 * 1024) = 0;
    virtual bool ResolveConflict(const FileConflict& conflict, ConflictResolutionStrategy strategy) = 0;
    virtual std::vector<FileConflict> GetConflicts() = 0;
    virtual void SetSyncCallback(std::function<void(const SyncProgress&)> callback) = 0;
};

// Distributed storage interface
class DistributedStorage {
public:
    virtual ~DistributedStorage() = default;
    virtual std::string StoreChunk(const FileChunk& chunk, const std::vector<char>& data) = 0;
    virtual std::vector<char> RetrieveChunk(const std::string& chunk_id) = 0;
    virtual bool DeleteChunk(const std::string& chunk_id) = 0;
    virtual bool ExistsChunk(const std::string& chunk_id) = 0;
    virtual std::vector<std::string> GetChunkNodes(const std::string& chunk_id) = 0;
    virtual std::string GetChunkId(const FileChunk& chunk) = 0;
    virtual void SetReplicationFactor(int factor) = 0;
    virtual int GetReplicationFactor() const = 0;
};

} // namespace sync
} // namespace cpr

#endif // CPR_SYNC_FILE_SYNC_H
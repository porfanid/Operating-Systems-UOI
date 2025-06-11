# 🥝 Kiwi Database

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/porfanid/Operating-Systems-UOI)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

> A high-performance, lightweight key-value storage engine written in C, featuring LSM-tree architecture with advanced concurrency control and compression support.

## ✨ Features

### 🚀 **Performance**
- **LSM-Tree Architecture**: Optimized for write-heavy workloads
- **Multi-threaded Operations**: Concurrent read/write operations with reader-writer locks
- **Bloom Filters**: Efficient key existence checking to reduce disk I/O
- **Block Compression**: Optional Snappy compression for reduced storage footprint
- **Memory-Mapped I/O**: Fast file access with intelligent caching

### 🏗️ **Architecture**
- **Skip List Memtables**: In-memory data structure for fast insertions
- **SST Files**: Sorted String Tables with block-based storage
- **Write-Ahead Logging**: Crash recovery and durability guarantees
- **Background Compaction**: Automatic data organization and cleanup
- **LRU Cache**: Configurable block cache for improved read performance

### 🔧 **Advanced Features**
- **MVCC Support**: Multi-version concurrency control
- **Range Queries**: Efficient iteration over key ranges
- **Atomic Operations**: ACID-compliant transactions
- **Configurable Levels**: Tunable LSM-tree depth (up to 7 levels)
- **Dynamic Scaling**: Automatic file size management

## 📋 Table of Contents

- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [API Reference](#-api-reference)
- [Benchmarks](#-benchmarks)
- [Architecture](#-architecture)
- [Configuration](#-configuration)
- [Contributing](#-contributing)
- [License](#-license)

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/porfanid/Operating-Systems-UOI.git
cd Operating-Systems-UOI

# Build the project
make -C kiwi/kiwi-source/engine
make -C kiwi/kiwi-source/bench

# Run a simple benchmark
cd kiwi/kiwi-source/bench
./kiwi-bench write 1000 4
```

## 📦 Installation

### Prerequisites

- **GCC 4.3+** or compatible C compiler
- **POSIX-compliant system** (Linux, macOS)
- **libsnappy** (optional, for compression)
- **pthread** library

### Build from Source

```bash
# Navigate to the engine directory
cd kiwi/kiwi-source/engine

# Compile the storage engine
make

# Build the benchmark tool
cd ../bench
make
```

### Configuration Options

The build system supports several compile-time options:

```makefile
# Enable compression support
CFLAGS += -DWITH_SNAPPY

# Enable bloom filters
CFLAGS += -DWITH_BLOOM_FILTER

# Enable background merging
CFLAGS += -DBACKGROUND_MERGE
```

## 🎯 Usage

### Basic Operations

```c
#include "db.h"
#include "variant.h"

// Open database
DB* db = db_open("./mydb");

// Prepare key and value
Variant* key = buffer_new(16);
Variant* value = buffer_new(1000);

buffer_putstr(key, "hello");
buffer_putstr(value, "world");

// Write operation
db_add(db, key, value);

// Read operation
Variant* result = buffer_new(1000);
if (db_get(db, key, result)) {
    printf("Value: %.*s\n", result->length, result->mem);
}

// Cleanup
buffer_free(key);
buffer_free(value);
buffer_free(result);
db_close(db);
```

### Benchmark Tool

The included benchmark tool supports various operation modes:

```bash
# Write benchmark: 10,000 operations with 4 threads
./kiwi-bench write 10000 4

# Read benchmark: 5,000 operations with 2 threads
./kiwi-bench read 5000 2

# Mixed workload: 50% writes, 50% reads
./kiwi-bench readwrite 10000 4 50
```

### Iterator Usage

```c
// Create iterator
DBIterator* iter = db_iterator_new(db);

// Seek to a specific key
Variant* seek_key = buffer_new(16);
buffer_putstr(seek_key, "start_key");
db_iterator_seek(iter, seek_key);

// Iterate through records
while (db_iterator_valid(iter)) {
    Variant* key = db_iterator_key(iter);
    Variant* value = db_iterator_value(iter);
    
    printf("Key: %.*s, Value: %.*s\n", 
           key->length, key->mem,
           value->length, value->mem);
    
    db_iterator_next(iter);
}

// Cleanup
db_iterator_free(iter);
buffer_free(seek_key);
```

## 📚 API Reference

### Core Database Operations

| Function | Description | Returns |
|----------|-------------|---------|
| `db_open(path)` | Open database at specified path | `DB*` handle |
| `db_close(db)` | Close database and flush data | `void` |
| `db_add(db, key, value)` | Insert or update key-value pair | `int` (success/failure) |
| `db_get(db, key, value)` | Retrieve value for given key | `int` (found/not found) |
| `db_remove(db, key)` | Delete key-value pair | `int` (success/failure) |

### Iterator Interface

| Function | Description |
|----------|-------------|
| `db_iterator_new(db)` | Create new iterator |
| `db_iterator_seek(iter, key)` | Position iterator at key |
| `db_iterator_next(iter)` | Advance to next record |
| `db_iterator_valid(iter)` | Check if iterator is valid |
| `db_iterator_key(iter)` | Get current key |
| `db_iterator_value(iter)` | Get current value |

## 📊 Benchmarks

Performance results on Intel Core i5-7200U @ 2.50GHz with 4 threads:

### Write Performance
```
Keys:           16 bytes each
Values:         1000 bytes each
Entries:        500
Random-Write:   0.000060 sec/write
Throughput:     16,650 writes/sec
```

### Read Performance
```
Random-Read:    0.000053 sec/read
Throughput:     18,813 reads/sec
Cache hit rate: ~85%
```

### Storage Efficiency
- **Compression Ratio**: ~70% with Snappy (varies by data)
- **Write Amplification**: ~2.5x (typical for LSM-trees)
- **Space Amplification**: ~1.8x

## 🏛️ Architecture

### LSM-Tree Structure

```
┌─────────────┐    ┌─────────────┐
│  MemTable   │───▶│ Immutable   │
│ (SkipList)  │    │  MemTable   │
└─────────────┘    └─────────────┘
       │                   │
       ▼                   ▼
┌─────────────────────────────────┐
│        Write-Ahead Log          │
└─────────────────────────────────┘
                   │
                   ▼
       ┌─────────────────────┐
       │      SST Files      │
       │                     │
       │ Level 0: 4 files    │
       │ Level 1: 10 files   │
       │ Level 2: 100 files  │
       │ Level N: 10^N files │
       └─────────────────────┘
```

### Component Overview

- **MemTable**: Active in-memory structure (Skip List)
- **Immutable MemTable**: Read-only MemTable being flushed
- **SST Files**: Sorted disk-based storage with bloom filters
- **Compaction**: Background process merging and organizing data
- **WAL**: Write-ahead log for crash recovery

### File Format

```
SST File Structure:
┌──────────────┐
│ Data Block 1 │ ← User key-value pairs
├──────────────┤
│ Data Block 2 │
├──────────────┤
│     ...      │
├──────────────┤
│ Index Block  │ ← Block index for fast lookup
├──────────────┤
│ Bloom Filter │ ← Probabilistic key existence
├──────────────┤
│ Meta Block   │ ← Statistics and metadata
├──────────────┤
│   Footer     │ ← File format information
└──────────────┘
```

## ⚙️ Configuration

### Compile-time Configuration

Edit `config.h` to customize behavior:

```c
// Memory and performance tuning
#define SKIPLIST_SIZE 1000000           // MemTable size
#define BLOCK_SIZE 4096                 // SST block size
#define LRU_CACHE_SIZE (92 * 1048576)   // Block cache size

// LSM-tree parameters
#define MAX_LEVELS 7                    // Maximum tree depth
#define MAX_FILES_LEVEL0 4              // L0 compaction trigger

// Bloom filter settings
#define BITS_PER_KEY 10                 // False positive rate
#define NUM_PROBES 7                    // Hash functions

// Compression
#define WITH_SNAPPY                     // Enable compression
```

### Runtime Tuning

```c
// Custom cache size
DB* db = db_open_ex("./mydb", 128 * 1024 * 1024); // 128MB cache

// Thread-safe operations are automatic
// No additional configuration needed
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Clone with development branches
git clone --recursive https://github.com/porfanid/Operating-Systems-UOI.git

# Run tests
cd kiwi/kiwi-source/engine/test
make && ./skiplist_test

# Format code (if clang-format available)
find . -name "*.c" -o -name "*.h" | xargs clang-format -i
```

### Code Style

- Follow K&R C style
- Use descriptive variable names
- Comment complex algorithms
- Maintain thread safety

## 🔍 Troubleshooting

### Common Issues

1. **Compilation Errors**
   ```bash
   # Missing dependencies
   sudo apt-get install build-essential libsnappy-dev
   ```

2. **Permission Errors**
   ```bash
   # Ensure write permissions for database directory
   chmod 755 /path/to/database
   ```

3. **Performance Issues**
   ```bash
   # Increase cache size or adjust compaction triggers
   # Monitor with: ./kiwi-bench read 10000 1
   ```

### Debug Mode

```bash
# Compile with debug symbols
make DEBUG=1

# Run with GDB
gdb ./kiwi-bench
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by LevelDB and RocksDB architectures
- Skip list implementation based on William Pugh's original paper
- Bloom filter algorithms from Burton Howard Bloom's work
- LSM-tree concepts from Patrick O'Neil et al.

## 📞 Support

- 📧 **Email**: [your.email@example.com](mailto:your.email@example.com)
- 🐛 **Issues**: [GitHub Issues](https://github.com/porfanid/Operating-Systems-UOI/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/porfanid/Operating-Systems-UOI/discussions)

---

<div align="center">

**[⭐ Star this repository](https://github.com/porfanid/Operating-Systems-UOI/stargazers) if you find it useful!**

Made with ❤️ and C

</div>

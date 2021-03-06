/*
 *  Copyright (C) 2009-2010 Sourcefire, Inc.
 *  All rights reserved.
 *  Authors: Török Edvin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/** @file */
#define force_inline inline __attribute__((always_inline))
#define overloadable_func __attribute__((overloadable))

/* DOXYGEN defined() must come first */
#if defined(DOXYGEN) || __has_feature(attribute_overloadable)
/* Yes, clang supports overloading functions in C! */
/** Prints \p str to clamscan's --debug output.
  \param str null terminated string */
static force_inline void overloadable_func debug(const char * str)
{
    debug_print_str((const uint8_t*)str, 0);
}

/** Prints \p str to clamscan's --debug output.
  \param str null terminated string */
static force_inline void overloadable_func debug(const uint8_t* str)
{
    debug_print_str((const uint8_t*)str, 0);
}

/** Prints \p a integer to clamscan's --debug output.
  \param a integer */
static force_inline void overloadable_func debug(uint32_t a)
{
    debug_print_uint(a);
}

/** debug is an overloaded function (yes clang supports that in C!), but it only
  works on strings, and integers. Give an error on any other type */
void debug(...) __attribute__((overloadable, unavailable));
#endif


/* Virusname definition handling */
/**
 * Declares the virusname prefix.
 \group_config
 * @param name the prefix common to all viruses reported by this bytecode
 * */
#define VIRUSNAME_PREFIX(name) const char __clambc_virusname_prefix[] = name;
/** Declares all the virusnames that this bytecode can report.
 \group_config
 * @param ... a comma-separated list of strings interpreted as virusnames  */
#define VIRUSNAMES(...) const char *const __clambc_virusnames[] = {__VA_ARGS__};

/* Logical signature handling */

typedef struct signature {
    uint64_t id;
} __Signature;

/** Like \p PE_HOOK_DECLARE, but it is not run for packed files that pe.c can
 * unpack (only on the unpacked file).
 \group_config */
#define PE_UNPACKER_DECLARE const uint16_t __clambc_kind = BC_PE_UNPACKER;

/** Make the current bytecode a PDF hook. Having a logical signature doesn't
 * make sense here, since logical signature is evaluated AFTER these hooks
 * run.
 \group_config
 * This hook is called several times, use pdf_get_phase() to find out in which
 * phase you got called. */
#define PDF_HOOK_DECLARE const uint16_t __clambc_kind = BC_PDF;

/** entrypoint() return code that tells hook invoker that it should skip
 * executing, probably because it'd trigger a bug in it */
#define BYTECODE_ABORT_HOOK 0xcea5e

/** Make the current bytecode a PE hook, i.e. it will be called once
  the logical signature trigger matches (or always if there is none), and you
  have access to all the PE information. By default you only have access to
  execs.h information, and not to PE field information (even for PE files).
  \group_config */
#define PE_HOOK_DECLARE const uint16_t __clambc_kind = BC_PE_ALL;

/** Marks the beginning of the subsignature name declaration section.
 \group_config */
#define SIGNATURES_DECL_BEGIN \
    struct __Signatures {
/** Declares a name for a subsignature.
 \group_config */
#define DECLARE_SIGNATURE(name) \
    const char *name##_sig;\
    __Signature name;
/** Marks the end of the subsignature name declaration section.
 \group_config */
#define SIGNATURES_DECL_END };

/** Defines the ClamAV file target.
 \group_config
 * @param tgt ClamAV signature type (0 - raw, 1 - PE, etc.) */
#define TARGET(tgt) const unsigned short __Target = (tgt);

/** Defines an alternative copyright for this bytecode.
 \group_config
  This will also prevent the sourcecode from being embedded into the bytecode */
#define COPYRIGHT(c) const char *const __Copyright = (c);

/** Define IconGroup1 for logical signature. 
  
  See logical signature documentation
 * for what it is
 \group_config
 */
#define ICONGROUP1(group) const char *const __IconGroup1 = (group);

/** Define IconGroup2 for logical signature. See logical signature documentation
 * for what it is.
 \group_config
 */
#define ICONGROUP2(group) const char *const __IconGroup2 = (group);

/** Define the minimum engine functionality level required for this 
 * bytecode/logical signature.
 * Engines older than this will skip loading the bytecode.
 * You can use the 'enum FunctionalityLevels' constants here.
 \group_config
 */
#define FUNCTIONALITY_LEVEL_MIN(m) const unsigned short __FuncMin = (m);

/** Define the maximum engine functionality level required for this 
 * bytecode/logical signature.
 * Engines newer than this will skip loading the bytecode.
 * You can use the 'enum FunctionalityLevels' constants here.
 \group_config */
#define FUNCTIONALITY_LEVEL_MAX(m) const unsigned short __FuncMax = (m);

#define LDB_ADDATTRIBUTES(x) const char * __ldb_rawattrs = (x);

#define CONTAINER(x) const char * __ldb_container = (x);

/** Marks the beginning of subsignature pattern definitions. 
 \group_config
 * \sa SIGNATURES_DECL_BEGIN */
/* some other macro may use __COUNTER__, so we need to subtract its current\
 * value to obtain zero-based indices */
#define SIGNATURES_DEF_BEGIN \
    static const unsigned __signature_bias = __COUNTER__+1;\
const struct __Signatures Signatures = {\
/** Defines the pattern for a previously declared subsignature.
 * \sa DECLARE_SIGNATURE
 \group_config
 * @param name the name of a previously declared subsignature
 * @param hex the pattern for this subsignature
 * */
#define DEFINE_SIGNATURE(name, hex) \
    .name##_sig = (hex),\
    .name = {__COUNTER__ - __signature_bias},
/** Marks the end of the subsignature pattern definitions.
 \group_config */
#define SIGNATURES_END };\

/** Returns how many times the specified signature matched.
 * @param sig name of subsignature queried
 * @return number of times this subsignature matched in the entire file
 \group_engine
 *
 * This is a constant-time operation, the counts for all subsignatures are
 * already computed.*/
static force_inline uint32_t count_match(__Signature sig)\
{ return __clambc_match_counts[sig.id]; }\

/** Returns whether the specified subsignature has matched at least once.
 \group_engine
 * @param sig name of subsignature queried
 * @return 1 if subsignature one or more times, 0 otherwise */
static force_inline uint32_t matches(__Signature sig)\
{ return __clambc_match_counts[sig.id]  != 0; }\


/** Returns the offset of the match.
 \group_engine
  * @param sig - Signature
  * @param goback - max length of signature
  * @return offset of match
  */
static force_inline uint32_t match_location(__Signature sig, uint32_t goback)
{
  int32_t pos = __clambc_match_offsets[sig.id];
  if (engine_functionality_level() <= FUNC_LEVEL_096_1) {
    /* bug, it returns offset of last subsig, not offset of first */
    pos -= goback;
    if (pos <= 0) pos = 0;
  }
  return pos;
}

/** Like match_location(), but also checks that the match starts with
  the specified hex string.
 \group_engine
  It is recommended to use this for safety and compatibility with 0.96.1
  @param sig - signature
  @param goback - maximum length of signature (till start of last subsig)
  @param static_start - static string that sig must begin with
  @param static_len - static string that sig must begin with - length
  @return >=0 - offset of match
           -1 - no match
  */
static force_inline int32_t match_location_check(__Signature sig,
                                                  uint32_t goback,
                                                  const char *static_start,
                                                  uint32_t static_len)
{
  int32_t pos = match_location(sig, goback);
  if (seek(pos, SEEK_SET) != pos)
    return -1;
  int32_t cpos = file_find_limit(static_start, static_len, pos + goback);
  if (cpos == -1) {
    debug("Engine reported match, but we couldn't find it! Engine reported (after fixup):");
    debug(pos);
    return -1;
  }
  if (seek(cpos, SEEK_SET) != cpos)
    return -1;
  if (cpos != pos && engine_functionality_level() >= FUNC_LEVEL_096_1_dev) {
    debug("wrong match pos reported by engine, real match pos:");
    debug(cpos);
    debug("reported by engine:");
    debug(pos);
    debug("but goback fixed it up!");
  }
  return cpos;
}

/** Sets the specified virusname as the virus detected by this bytecode.
  \group_scan
 * @param virusname the name of the virus, excluding the prefix, must be one of
 * the virusnames declared in \p VIRUSNAMES.
 * \sa VIRUSNAMES */
static force_inline overloadable_func void foundVirus(const char *virusname)
{
    setvirusname((const uint8_t*)virusname, 0);
}

#if defined(DOXYGEN) || __has_feature(attribute_overloadable)
/** Like foundVirus() but just use the prefix as virusname */
static force_inline void overloadable_func foundVirus(void)
{
  foundVirus("");
}
#endif

/** Returns the currently scanned file's size.
  \group_file
  * @return file size as 32-bit unsigned integer */
static force_inline uint32_t getFilesize(void)
{
  return __clambc_filesize[0];
}

union unaligned_32 {
	uint32_t una_u32;
	int32_t una_s32;
} __attribute__((packed));

union unaligned_16 {
	uint16_t una_u16;
	int16_t una_s16;
} __attribute__((packed));

/**
 * Returns true if the bytecode is executing on a big-endian CPU.
 * @return true if executing on bigendian CPU, false otherwise
 \group_env
 *
 * This will be optimized away in libclamav, but it must be used when dealing
 * with endianess for portability reasons.
 * For example whenever you read a 32-bit integer from a file, it can be written
 * in little-endian convention (x86 CPU for example), or big-endian convention
 * (PowerPC CPU for example).
 * If the file always contains little-endian integers, then conversion might be
 * needed.
 * ClamAV bytecodes by their nature must only handle known-endian integers, if
 * endianness can change, then both situations must be taken into account (based
 * on a 1-byte field for example).
 */
bool __is_bigendian(void) __attribute__((const)) __attribute__((nothrow));

/** Converts the specified value if needed, knowing it is in little endian
 * order.
 \group_adt
 * @param[in] v 32-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint32_t force_inline le32_to_host(uint32_t v)
{
  /* calculate bswap always, so compiler can use a select,
     and doesn't need to create a branch.
     This will get optimized away at bytecode load time anyway */
  uint32_t swapped = __builtin_bswap32(v);
  return __is_bigendian() ? swapped : v;
}

/** Converts the specified value if needed, knowing it is in big endian
 * order.
 \group_adt
 * @param[in] v 32-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint32_t force_inline be32_to_host(uint32_t v)
{
  /* calculate bswap always, so compiler can use a select,
     and doesn't need to create a branch.
     This will get optimized away at bytecode load time anyway */
  uint32_t swapped = __builtin_bswap32(v);
  return __is_bigendian() ? v : swapped;
}

/** Converts the specified value if needed, knowing it is in little endian
 * order.
 \group_adt
 * @param[in] v 64-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint64_t force_inline le64_to_host(uint64_t v)
{
  uint64_t swapped = __builtin_bswap64(v);
  return __is_bigendian() ? swapped : v;
}

/** Converts the specified value if needed, knowing it is in big endian
 * order.
 \group_adt
 * @param[in] v 64-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint64_t force_inline be64_to_host(uint64_t v)
{
  uint64_t swapped = __builtin_bswap64(v);
  return __is_bigendian() ? v : swapped;
}

/** Converts the specified value if needed, knowing it is in little endian
 * order.
 \group_adt
 * @param[in] v 16-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint16_t force_inline le16_to_host(uint16_t v)
{
  uint16_t swapped = ((v & 0xff) << 8) | ((v >> 8) & 0xff);
  return __is_bigendian() ? swapped : v;
}

/** Converts the specified value if needed, knowing it is in big endian
 * order.
 \group_adt
 * @param[in] v 16-bit integer as read from a file
 * @return integer converted to host's endianess */
static uint16_t force_inline be16_to_host(uint16_t v)
{
  uint16_t swapped = ((v & 0xff) << 8) | ((v >> 8) & 0xff);
  return __is_bigendian() ? v : swapped;
}

/** Reads from the specified buffer a 32-bit of little-endian integer.
 \group_adt
 * @param[in] buff pointer to buffer
 * @return 32-bit little-endian integer converted to host endianness */
static uint32_t force_inline cli_readint32(const void* buff)
{
    uint32_t v = ((const union unaligned_32 *)buff)->una_s32;
    return le32_to_host(v);
}

/** Reads from the specified buffer a 16-bit of little-endian integer.
 \group_adt
 * @param[in] buff pointer to buffer
 * @return 16-bit little-endian integer converted to host endianness */
static uint16_t force_inline cli_readint16(const void* buff)
{
    uint16_t v = ((const union unaligned_16 *)buff)->una_s16;
    return le16_to_host(v);
}

/** Writes the specified value into the specified buffer in little-endian order
 \group_adt
 * @param[out] offset pointer to buffer to write to
 * @param[in] v value to write*/
static void force_inline cli_writeint32(void* offset, uint32_t v)
{
    ((union unaligned_32 *)offset)->una_u32 = le32_to_host(v);
}

/* --------------------- PE helper functions ------------------------ */
/** Returns whether the current file has executable information.
  \group_pe
 * @return true if the file has exe info, false otherwise */
static force_inline bool hasExeInfo(void)
{
    return __clambc_pedata.offset != -1;
}

/** Returns whether PE information is available
  \group_pe
  * @return true if PE information is available (in PE hooks) */
static force_inline bool hasPEInfo(void)
{
  return (__clambc_kind == BC_PE_ALL ||
          __clambc_kind == BC_PE_UNPACKER);
}

#define NEED_PE_INFO {  /* only available in PE hooks */\
  if (!hasPEInfo())\
    __fail_missing_PE_HOOK_DECLARE__or__PE_UNPACKER_DECLARE();\
}

/** Returns whether this is a PE32+ executable.
  \group_pe
  @return true if this is a PE32+ executable
*/
static force_inline bool isPE64(void)
{
  NEED_PE_INFO;
  return le16_to_host(__clambc_pedata.opt64.Magic) == 0x020b;
}

static force_inline
/** Returns MajorLinkerVersion for this PE file.
  \group_pe
  * @return PE MajorLinkerVersion or 0 if not in PE hook */
static force_inline uint8_t getPEMajorLinkerVersion(void)
{
  return isPE64() ?
    __clambc_pedata.opt64.MajorLinkerVersion :
    __clambc_pedata.opt32.MajorLinkerVersion;
}

/** Returns MinorLinkerVersion for this PE file.
  \group_pe
  * @return PE MinorLinkerVersion or 0 if not in PE hook */
static force_inline uint8_t getPEMinorLinkerVersion(void)
{
  return isPE64() ?
    __clambc_pedata.opt64.MinorLinkerVersion :
    __clambc_pedata.opt32.MinorLinkerVersion;
}

/** Return the PE SizeOfCode.
  \group_pe
  * @return PE SizeOfCode or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfCode(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfCode :
                      __clambc_pedata.opt32.SizeOfCode);
}

/** Return the PE SizeofInitializedData.
  \group_pe
  * @return PE SizeOfInitializeData or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfInitializedData(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfInitializedData :
                      __clambc_pedata.opt32.SizeOfInitializedData);
}

/** Return the PE SizeofUninitializedData.
  \group_pe
  * @return PE SizeofUninitializedData or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfUninitializedData(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfUninitializedData :
                      __clambc_pedata.opt32.SizeOfUninitializedData);
}

/** Return the PE BaseOfCode.
  \group_pe
 * @return PE BaseOfCode, or 0 if not in PE hook.
 */
static force_inline uint32_t getPEBaseOfCode(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.BaseOfCode :
                      __clambc_pedata.opt32.BaseOfCode);
}

/** Return the PE BaseOfData.
  \group_pe
  @return PE BaseOfData, or 0 if not in PE hook.
  */
static force_inline uint32_t getPEBaseOfData(void)
{
  return le32_to_host(isPE64() ?
                      0 :
                      __clambc_pedata.opt32.BaseOfData);
}

/** Return the PE ImageBase as 64-bit integer.
  \group_pe
  * @return PE ImageBase as 64-bit int, or 0 if not in PE hook */
static force_inline uint64_t getPEImageBase(void)
{
  return le64_to_host(isPE64() ?
                      __clambc_pedata.opt64.ImageBase :
                      __clambc_pedata.opt32.ImageBase);
}

/** Return the PE SectionAlignment.
  \group_pe
  * @return PE SectionAlignment, or 0 if not in PE hook */
static force_inline uint32_t getPESectionAlignment(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SectionAlignment :
                      __clambc_pedata.opt32.SectionAlignment);
}

/** Return the PE FileAlignment.
  \group_pe
  * @return PE FileAlignment, or 0 if not in PE hook
  */
static force_inline uint32_t getPEFileAlignment(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.FileAlignment :
                      __clambc_pedata.opt32.FileAlignment);
}

/** Return the PE MajorOperatingSystemVersion.
  \group_pe
  * @return PE MajorOperatingSystemVersion, or 0 if not in PE hook */
static force_inline uint16_t getPEMajorOperatingSystemVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MajorOperatingSystemVersion :
                      __clambc_pedata.opt32.MajorOperatingSystemVersion);
}

/** Return the PE MinorOperatingSystemVersion.
  \group_pe
  * @return PE MinorOperatingSystemVersion, or 0 if not in PE hook */
static force_inline uint16_t getPEMinorOperatingSystemVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MinorOperatingSystemVersion :
                      __clambc_pedata.opt32.MinorOperatingSystemVersion);
}

/** Return the PE MajorImageVersion.
  \group_pe
  * @return PE MajorImageVersion, or 0 if not in PE hook */
static force_inline uint16_t getPEMajorImageVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MajorImageVersion :
                      __clambc_pedata.opt32.MajorImageVersion);
}

/** Return the PE MinorImageVersion.
  \group_pe
  * @return PE MinorrImageVersion, or 0 if not in PE hook */
static force_inline uint16_t getPEMinorImageVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MinorImageVersion :
                      __clambc_pedata.opt32.MinorImageVersion);
}

/** Return the PE MajorSubsystemVersion.
  \group_pe
  * @return PE MajorSubsystemVersion or 0 if not in PE hook */
static force_inline uint16_t getPEMajorSubsystemVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MajorSubsystemVersion :
                      __clambc_pedata.opt32.MajorSubsystemVersion);
}

/** Return the PE MinorSubsystemVersion.
  \group_pe
  * @return PE MinorSubsystemVersion, or 0 if not in PE hook */
static force_inline uint16_t getPEMinorSubsystemVersion(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.MinorSubsystemVersion :
                      __clambc_pedata.opt32.MinorSubsystemVersion);
}

/** Return the PE Win32VersionValue.
  \group_pe
  * @return PE Win32VersionValue, or 0 if not in PE hook */
static force_inline uint32_t getPEWin32VersionValue(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.Win32VersionValue :
                      __clambc_pedata.opt32.Win32VersionValue);
}

/** Return the PE SizeOfImage.
  \group_pe
  * @return PE SizeOfImage, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfImage(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfImage :
                      __clambc_pedata.opt32.SizeOfImage);
}

/** Return the PE SizeOfHeaders.
  \group_pe
  * @return PE SizeOfHeaders, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfHeaders(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfHeaders :
                      __clambc_pedata.opt32.SizeOfHeaders);
}

/** Return the PE CheckSum.
  \group_pe
  * @return PE CheckSum, or 0 if not in PE hook
  */
static force_inline uint32_t getPECheckSum(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.CheckSum :
                      __clambc_pedata.opt32.CheckSum);
}

/** Return the PE Subsystem.
  \group_pe
  * @return PE subsystem, or 0 if not in PE hook */
static force_inline uint16_t getPESubsystem(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.Subsystem :
                      __clambc_pedata.opt32.Subsystem);
}

/** @brief Return the PE DllCharacteristics.

  \group_pe
  * @return PE DllCharacteristics, or 0 if not in PE hook */
static force_inline uint16_t getPEDllCharacteristics(void)
{
  return le16_to_host(isPE64() ?
                      __clambc_pedata.opt64.DllCharacteristics :
                      __clambc_pedata.opt32.DllCharacteristics);
}

/** Return the PE SizeOfStackReserve.
  \group_pe
  * @return PE SizeOfStackReserver, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfStackReserve(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfStackReserve :
                      __clambc_pedata.opt32.SizeOfStackReserve);
}

/** Return the PE SizeOfStackCommit.
  \group_pe
  * @return PE SizeOfStackCommit, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfStackCommit(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfStackCommit :
                      __clambc_pedata.opt32.SizeOfStackCommit);
}

/** Return the PE SizeOfHeapReserve.
  \group_pe
  * @return PE SizeOfHeapReserve, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfHeapReserve(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfHeapReserve :
                      __clambc_pedata.opt32.SizeOfHeapReserve);
}

/** Return the PE SizeOfHeapCommit.
  \group_pe
  * @return PE SizeOfHeapCommit, or 0 if not in PE hook */
static force_inline uint32_t getPESizeOfHeapCommit(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.SizeOfHeapCommit :
                      __clambc_pedata.opt32.SizeOfHeapCommit);
}

/** Return the PE LoaderFlags.
  \group_pe
  * @return PE LoaderFlags or 0 if not in PE hook
  */
static force_inline uint32_t getPELoaderFlags(void)
{
  return le32_to_host(isPE64() ?
                      __clambc_pedata.opt64.LoaderFlags :
                      __clambc_pedata.opt32.LoaderFlags);
}

/** Returns the CPU this executable runs on, see libclamav/pe.c for possible
 * values.
  \group_pe
  * @return PE Machine or 0 if not in PE hook */
static force_inline uint16_t getPEMachine()
{
  NEED_PE_INFO;
  return le16_to_host(__clambc_pedata.file_hdr.Machine);
}

/** Returns the PE TimeDateStamp from headers
  \group_pe
  * @return PE TimeDateStamp or 0 if not in PE hook */
static force_inline uint32_t getPETimeDateStamp()
{
  NEED_PE_INFO;
  return le32_to_host(__clambc_pedata.file_hdr.TimeDateStamp);
}

/** Returns pointer to the PE debug symbol table
  \group_pe
  * @return PE PointerToSymbolTable or 0 if not in PE hook */
static force_inline uint32_t getPEPointerToSymbolTable()
{
  NEED_PE_INFO;
  return le32_to_host(__clambc_pedata.file_hdr.PointerToSymbolTable);
}

/** Returns the PE number of debug symbols
  \group_pe
  * @return PE NumberOfSymbols or 0 if not in PE hook */
static force_inline uint32_t getPENumberOfSymbols()
{
  NEED_PE_INFO;
  return le32_to_host(__clambc_pedata.file_hdr.NumberOfSymbols);
}

/** Returns the size of PE optional header.
  \group_pe
  * @return size of PE optional header, or 0 if not in PE hook
  */
static force_inline uint16_t getPESizeOfOptionalHeader()
{
  NEED_PE_INFO;
  return le16_to_host(__clambc_pedata.file_hdr.SizeOfOptionalHeader);
}

/** Returns PE characteristics.
  * For example you can use this to check whether it is a DLL (0x2000).
  \group_pe
  * @return characteristic of PE file, or 0 if not in PE hook*/
static force_inline uint16_t getPECharacteristics()
{
  NEED_PE_INFO;
  return le16_to_host(__clambc_pedata.file_hdr.Characteristics);
}

/** Returns whether this is a DLL.
  * Use this only in a PE hook!
  \group_pe
  * @return true - the file is a DLL
            false - file is not a DLL
*/
static force_inline bool getPEisDLL()
{
  return getPECharacteristics() & 0x2000;
}

/** Gets the virtual address of specified image data directory.
  \group_pe
  @param n image directory requested
  @return Virtual Address of requested image directory
*/
static force_inline uint32_t getPEDataDirRVA(unsigned n)
{
  NEED_PE_INFO;
  struct pe_image_data_dir *p = &__clambc_pedata.opt64.DataDirectory[n];
  struct pe_image_data_dir *p32 = &__clambc_pedata.opt32.DataDirectory[n];
  return n < 16 ? le32_to_host(isPE64() ?
                               p->VirtualAddress :
                               p32->VirtualAddress)
    : 0;
}

/** Gets the size of the specified image data directory.
  \group_pe
  @param n image directory requested
  @return Size of requested image directory
*/
static force_inline uint32_t getPEDataDirSize(unsigned n)
{
  NEED_PE_INFO;
  return n < 16 ? le32_to_host(isPE64() ?
                               __clambc_pedata.opt64.DataDirectory[n].Size :
                               __clambc_pedata.opt32.DataDirectory[n].Size)
    : 0;
}

/** Returns the number of sections in this executable file.
  \group_pe
 * @return number of sections as 16-bit unsigned integer */
static force_inline uint16_t getNumberOfSections(void)
{
  /* available in non-PE hooks too */
    return __clambc_pedata.nsections;
}

/** Gets the offset to the PE header.
  \group_pe
  @return offset to the PE header, or 0 if not in PE hook */
static uint32_t getPELFANew(void)
{
  NEED_PE_INFO;
    return le32_to_host(__clambc_pedata.e_lfanew);
}

/** Read name of requested PE section.
  \group_pe
  @param[out] name name of PE section
  @param[in] n PE section requested
  @return 0 if successful, 
        <0 otherwise
*/
static force_inline int readPESectionName(unsigned char name[8], unsigned n)
{
  NEED_PE_INFO;
  if (n >= getNumberOfSections())
    return -1;
  uint32_t at = getPELFANew() + sizeof(struct pe_image_file_hdr) + sizeof(struct pe_image_optional_hdr32);
  if (!isPE64()) {
    /* Seek to the end of the long header */
    at += getPESizeOfOptionalHeader() - sizeof(struct pe_image_optional_hdr32);
  } else {
    at += sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32);
  }
  at += n * sizeof(struct pe_image_section_hdr);
  int32_t pos = seek(at, SEEK_SET);
  if (pos == -1)
    return -2;
  if (read(name, 8) != 8)
    return -3;
  seek(pos, SEEK_SET);
  return 0;
}

/** Returns the offset of the EntryPoint in the executable file.
  \group_pe
 * @return offset of EP as 32-bit unsigned integer */
static force_inline uint32_t getEntryPoint(void)
{
  /* available in non-PE hooks too */
    return __clambc_pedata.ep;
}

/** Returns the offset of the executable in the file.
  \group_pe
 * @return offset of embedded executable inside file. */
static force_inline uint32_t getExeOffset(void)
{
  /* available in non-PE hooks too */
    return __clambc_pedata.offset;
}

/** Returns the ImageBase with the correct endian conversion.
  * Only works if the bytecode is a PE hook (i.e. you invoked
  * PE_UNPACKER_DECLARE)
  \group_pe
  * @return ImageBase of PE file, 0 - for non-PE hook
  */
static force_inline uint32_t getImageBase(void)
{
  NEED_PE_INFO;
  return le32_to_host(__clambc_pedata.opt32.ImageBase);
}

/** The address of the EntryPoint. Use this for matching EP against sections.
  \group_pe
  * @return virtual address of EntryPoint, or 0 if not in PE hook */
static uint32_t getVirtualEntryPoint(void)
{
  NEED_PE_INFO;
  return le32_to_host(isPE64() ?
                        __clambc_pedata.opt64.AddressOfEntryPoint:
                        __clambc_pedata.opt32.AddressOfEntryPoint);
}

/** Return the RVA of the specified section 
  \group_pe.
  @param i section index (from 0)
  @return RVA of section, or -1 if invalid */
static uint32_t getSectionRVA(unsigned i)
{
  struct cli_exe_section section;
  if (get_pe_section(&section, i) == -1)
    return -1;
  return section.rva;
}

/** Return the virtual size of the specified section.
  \group_pe.
  @param i section index (from 0)
  @return VSZ of section, or -1 if invalid */
static uint32_t getSectionVirtualSize(unsigned i)
{
  struct cli_exe_section section;
  if (get_pe_section(&section, i) == -1)
    return -1;
  return section.vsz;
}

/** read the specified amount of bytes from the PE file, starting at the
  address specified by RVA.
  \group_pe
  @param rva the Relative Virtual Address you want to read from (will be
  converted to file offset)
  @param[out] buf destination buffer
  @param bufsize size of buffer
  @return true on success (full read),
          false on any failure */
static force_inline bool readRVA(uint32_t rva, void *buf, size_t bufsize)
{
  uint32_t off = pe_rawaddr(rva);
  if (off == PE_INVALID_RVA)
    return false;
  int32_t oldpos = seek(off, SEEK_SET);
  if (oldpos == -1)
    return false;
  if (read(buf, bufsize) != bufsize) {
    return false;
  }
  seek(oldpos, SEEK_SET);
  return true;
}

#ifdef __cplusplus
#define restrict
#endif

/** Scan the first \p n bytes of the buffer \p s, for the character \p c.
  \group_string
  @param[in] s buffer to scan
  @param c character to look for
  @param n size of buffer
  @return a pointer to the first byte to match, or NULL if not found.
 */
static void* memchr(const void* s, int c, size_t n)
{
  unsigned char cc = c;
  const char *end, *p = s;

  for (end=p+n; p < end; p++)
    if (*p == cc)
      return p;
  return (void*)0;
}

/* Provided by LLVM intrinsics */
/** Fills the specified buffer to the specified value.
  \group_string
 * @param[out] src pointer to buffer
 * @param[in] c character to fill buffer with
 * @param[in] n length of buffer
 * @return \p src*/
void* memset(void *src, int c, uintptr_t n) __attribute__((nothrow)) __attribute__((__nonnull__((1))));

/** Copies data between two possibly overlapping buffers.
  \group_string
 * @param[out] dst destination buffer
 * @param[in] src source buffer
 * @param[in] n amount of bytes to copy
 * @return dst */
void *memmove (void *dst, const void *src, uintptr_t n)
    __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
/** Copies data between two non-overlapping buffers.
  \group_string
 * @param[out] dst destination buffer
 * @param[in] src source buffer
 * @param[in] n amount of bytes to copy
 * @return dst */
void *memcpy (void *restrict dst, const void *restrict src, uintptr_t n)
    __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

/** Compares two memory buffers.
  \group_string
 * @param[in] s1 buffer one
 * @param[in] s2 buffer two
 * @param[in] n amount of bytes to copy
 * @return an integer less than, equal to, or greater than zero if the first 
 * \p n bytes of \p s1 are found, respectively, to be less than, to match, 
 * or be greater than the first \p n bytes of \p s2.*/
int memcmp (const void *s1, const void *s2, uint32_t n)
    __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

/** disassembled memory operand: scale_reg*scale + add_reg + displacement
  \group_disasm */
struct DIS_mem_arg {
    enum DIS_SIZE access_size;/**< size of access */
    enum X86REGS scale_reg;/**< register used as scale */
    enum X86REGS add_reg;/**< register used as displacemenet */
    uint8_t scale;/**< scale as immediate number */
    int32_t displacement;/**< displacement as immediate number */
};

/** disassembled operand
  \group_disasm */
struct DIS_arg {
    enum DIS_ACCESS access_type;/**< type of access */
    enum DIS_SIZE access_size;/**< size of access */
    union {
	struct DIS_mem_arg mem;/**< memory operand */
	enum X86REGS reg;/**< register operand */
	uint64_t other;/**< other operand */
    } u;
};

/** disassembled instruction.
  \group_disasm */
struct DIS_fixed {
    enum X86OPS x86_opcode;/**< opcode of X86 instruction */
    enum DIS_SIZE operation_size;/**< size of operation */
    enum DIS_SIZE address_size;/**< size of address */
    uint8_t segment;/**< segment */
    struct DIS_arg arg[3];/** arguments */
};

/** Disassembles one X86 instruction starting at the specified offset.
  \group_disasm
 * @param[out] result disassembly result
 * @param[in] offset start disassembling from this offset, in the current file
 * @param[in] len max amount of bytes to disassemble
 * @return offset where disassembly ended*/
static force_inline uint32_t
DisassembleAt(struct DIS_fixed* result, uint32_t offset, uint32_t len)
{
    struct DISASM_RESULT res;
    unsigned i;
    seek(offset, SEEK_SET);
    offset = disasm_x86(&res, len);
    result->x86_opcode = (enum X86OPS) cli_readint16(&res.real_op);
    result->operation_size = (enum DIS_SIZE) res.opsize;
    result->address_size = (enum DIS_SIZE) res.adsize;
    result->segment = res.segment;
    for (i=0;i<3;i++) {
	struct DIS_arg *arg = &result->arg[i];
	arg->access_type = (enum DIS_ACCESS) res.arg[i][0];
	switch (result->arg[i].access_type) {
	    case ACCESS_MEM:
		arg->u.mem.access_size = (enum DIS_SIZE) res.arg[i][1];
		arg->u.mem.scale_reg = (enum X86REGS) res.arg[i][2];
		arg->u.mem.add_reg = (enum X86REGS) res.arg[i][3];
		arg->u.mem.scale = res.arg[i][4];
		arg->u.mem.displacement = cli_readint32((const uint32_t*)&res.arg[i][6]);
		break;
	    case ACCESS_REG:
		arg->u.reg = (enum X86REGS) res.arg[i][1];
		break;
	    default: {
		uint64_t x = cli_readint32((const uint32_t*)&res.arg[i][6]);
		arg->u.other = (x << 32) | cli_readint32((const uint32_t*)&res.arg[i][2]);
		break;
	    }
	}
    }
    return offset;
}

// re2c macros
#define RE2C_BSIZE 1024
typedef struct {
  unsigned char *cur, *lim, *mrk, *ctx, *eof, *tok;
  int res;
  int32_t tokstart;
  unsigned char buffer[RE2C_BSIZE];
} regex_scanner_t;

#define YYCTYPE unsigned char
#define YYCURSOR re2c_scur
#define YYLIMIT re2c_slim
#define YYMARKER re2c_smrk
#define YYCONTEXT re2c_sctx
#define YYFILL(n) { \
  RE2C_FILLBUFFER(n);\
  if (re2c_sres <= 0) break;\
}

#define REGEX_SCANNER unsigned char *re2c_scur, *re2c_stok, *re2c_smrk, *re2c_sctx, *re2c_slim;\
  int re2c_sres; int32_t re2c_stokstart;\
  unsigned char re2c_sbuffer[RE2C_BSIZE];\
  re2c_scur = re2c_slim = re2c_smrk = re2c_sctx = &re2c_sbuffer[0];\
  re2c_sres = 0;\
  RE2C_FILLBUFFER(0);

#define REGEX_POS (-(re2c_slim - re2c_scur) + seek(0, SEEK_CUR))
#define REGEX_LOOP_BEGIN do { re2c_stok = re2c_scur; re2c_stokstart = REGEX_POS;} while (0);
#define REGEX_RESULT (re2c_sres)

#define RE2C_DEBUG_PRINT \
do {\
  char buf[81];\
  uint32_t here = seek(0, SEEK_CUR);\
  uint32_t d = re2c_slim - re2c_scur;\
  uint32_t end = here - d;\
  unsigned len = end - re2c_stokstart;\
  if (len > 80) {\
    unsigned skipped = len - 74;\
    seek(re2c_stokstart, SEEK_SET);\
    if (read(buf, 37) == 37)\
      break;\
    memcpy(buf+37, "[...]", 5);\
    seek(end-37, SEEK_SET);\
    if (read(buf, 37) != 37)\
      break;\
    buf[80] = '\0';\
  } else {\
    seek(re2c_stokstart, SEEK_SET);\
    if (read(buf, len) != len)\
      break;\
    buf[len] = '\0';\
  }\
  buf[80] = '\0';\
  debug_print_str(buf, 0);\
  seek(here, SEEK_SET);\
} while (0)

#define DEBUG_PRINT_REGEX_MATCH RE2C_DEBUG_PRINT

#define BUFFER_FILL(buf, cursor, need, limit) do {\
  (limit) = fill_buffer((buf), sizeof((buf)), (limit), (cursor), (need));\
} while (0);

#define BUFFER_ENSURE(buf, cursor, need, limit) do {\
  if ((cursor) + (need) >= (limit)) {\
    BUFFER_FILL(buf, cursor, need, limit)\
    (cursor) = 0;\
  }\
} while (0);

/* Move stok to offset 0, and fill rest of buffer, at least with 'len' bytes.
   Adjust the other pointers, which must be after the stok pointer!
*/
#define RE2C_FILLBUFFER(need) do {\
  uint32_t cursor = re2c_stok - &re2c_sbuffer[0];\
  int32_t limit = re2c_slim - &re2c_sbuffer[0];\
  limit = fill_buffer(re2c_sbuffer, sizeof(re2c_sbuffer), limit, (cursor), (need));\
  if (!limit) {\
    re2c_sres = 0;\
  } else if (limit <= (need)) {\
     re2c_sres = -1;\
  } else {\
    uint32_t curoff = re2c_scur - re2c_stok;\
    uint32_t mrkoff = re2c_smrk - re2c_stok;\
    uint32_t ctxoff = re2c_sctx - re2c_stok;\
    re2c_slim = &re2c_sbuffer[0] + limit;\
    re2c_stok = &re2c_sbuffer[0];\
    re2c_scur = &re2c_sbuffer[0] + curoff;\
    re2c_smrk = &re2c_sbuffer[0] + mrkoff;\
    re2c_sctx = &re2c_sbuffer[0] + ctxoff;\
    re2c_sres = limit;\
  }\
} while (0);

/** ilog2_compat for 0.96 compatibility, you should use ilog2() 0.96.1 API
 * instead of this one! */
static inline int32_t ilog2_compat(uint32_t a, uint32_t b)
{
    uint32_t c = a > b ? a : b;
    if (c < 2048) {
	// scale up a,b to [0, 4096]
	uint32_t scale = 2048/c;
	a *= scale;
	b *= scale;
    } else {
	// scale down a,b to [0, 4096]
	uint32_t scale = (c+2047) / 2048;
	a /= scale;
	b /= scale;
    }
    // log(a/b) = log(a*scale/(b*scale)) = log(a*scale) - log(b*scale)
    return ilog_table[a] - ilog_table[b];
}

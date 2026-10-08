/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "DatasetFingerprint.h"

#include <algorithm>
#include <filesystem>
#include <system_error>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>

#include <array>
#include <fstream>
#endif

namespace
{
	// Regular files directly inside p_folder, sorted by path. None when the folder does not exist.
	std::vector<std::filesystem::path> filesOf(const std::filesystem::path& p_folder)
	{
		std::vector<std::filesystem::path> files;
		std::error_code error;
		if (!std::filesystem::is_directory(p_folder, error))
		{
			return files;
		}
		for (const std::filesystem::directory_entry& ENTRY : std::filesystem::directory_iterator(p_folder, error))
		{
			if (ENTRY.is_regular_file(error))
			{
				files.push_back(ENTRY.path());
			}
		}
		std::sort(files.begin(), files.end());
		return files;
	}

#ifdef _WIN32
	// A SHA-256 computation of the Windows cryptography API, released when it ends.
	class Sha256
	{
	public:
		Sha256()
		{
			if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&m_algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0)))
			{
				m_valid = BCRYPT_SUCCESS(BCryptCreateHash(m_algorithm, &m_hash, nullptr, 0, nullptr, 0, 0));
			}
		}

		~Sha256()
		{
			if (m_hash != nullptr)
			{
				BCryptDestroyHash(m_hash);
			}
			if (m_algorithm != nullptr)
			{
				BCryptCloseAlgorithmProvider(m_algorithm, 0);
			}
		}

		Sha256(const Sha256&) = delete;
		Sha256& operator=(const Sha256&) = delete;

		bool add(const char* p_bytes, std::size_t p_count)
		{
			m_valid = m_valid && BCRYPT_SUCCESS(BCryptHashData(m_hash,
				reinterpret_cast<PUCHAR>(const_cast<char*>(p_bytes)), static_cast<ULONG>(p_count), 0));
			return m_valid;
		}

		// The digest in lowercase hexadecimal, or an empty text when a step failed.
		std::string finish()
		{
			std::array<UCHAR, 32> digest{};
			if (!m_valid || !BCRYPT_SUCCESS(BCryptFinishHash(m_hash, digest.data(), static_cast<ULONG>(digest.size()), 0)))
			{
				return std::string();
			}
			const char* const DIGITS = "0123456789abcdef";
			std::string text;
			for (const UCHAR BYTE : digest)
			{
				text += DIGITS[BYTE >> 4];
				text += DIGITS[BYTE & 0x0F];
			}
			return text;
		}

	private:
		BCRYPT_ALG_HANDLE m_algorithm = nullptr;
		BCRYPT_HASH_HANDLE m_hash = nullptr;
		bool m_valid = false;
	};

	// Adds the relative path of p_file, a separator, then its content.
	bool addFile(Sha256& p_sha, const std::filesystem::path& p_file, const std::filesystem::path& p_root)
	{
		std::error_code error;
		const std::string NAME = std::filesystem::relative(p_file, p_root, error).generic_string();
		if (error || !p_sha.add(NAME.c_str(), NAME.size() + 1))
		{
			return false;
		}
		std::ifstream stream(p_file, std::ios::binary);
		if (!stream)
		{
			return false;
		}
		std::array<char, 65536> buffer{};
		while (stream)
		{
			stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
			const std::streamsize READ = stream.gcount();
			if (READ > 0 && !p_sha.add(buffer.data(), static_cast<std::size_t>(READ)))
			{
				return false;
			}
		}
		return stream.eof();
	}
#endif
}

namespace Performance
{
	std::string DatasetFingerprint::compute(const std::string& p_primaryFile, const std::vector<std::string>& p_scenarios)
	{
#ifdef _WIN32
		const std::filesystem::path ROOT = std::filesystem::path(p_primaryFile).parent_path();
		std::vector<std::filesystem::path> files = filesOf(ROOT);
		for (const std::string& SCENARIO : p_scenarios)
		{
			const std::vector<std::filesystem::path> SCENARIO_FILES = filesOf(ROOT / "Scenarios" / SCENARIO);
			files.insert(files.end(), SCENARIO_FILES.begin(), SCENARIO_FILES.end());
		}
		if (files.empty())
		{
			return std::string();
		}
		Sha256 sha;
		for (const std::filesystem::path& MODEL_FILE : files)
		{
			if (!addFile(sha, MODEL_FILE, ROOT))
			{
				return std::string();
			}
		}
		return sha.finish();
#else
		static_cast<void>(p_primaryFile);
		static_cast<void>(p_scenarios);
		static_cast<void>(&filesOf);
		return std::string();
#endif
	}
}

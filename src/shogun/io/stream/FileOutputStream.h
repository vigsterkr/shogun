/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Vitktor Gal
 */
#ifndef __FILE_OUTPUT_STREAM_H__
#define __FILE_OUTPUT_STREAM_H__

#include <shogun/io/fs/FileSystem.h>
#include <shogun/io/stream/OutputStream.h>

#include <string_view.hpp>

namespace shogun
{
	namespace io
	{
#define IGNORE_IN_CLASSLIST

		IGNORE_IN_CLASSLIST class CFileOutputStream : public COutputStream
		{
		public:
			CFileOutputStream(WritableFile* dest, bool free = false):
				COutputStream(), m_dst(dest) {}
			~CFileOutputStream() override
			{
				if (m_free)
					delete m_dst;
			}

			std::error_condition close() override
			{
				return m_dst->close();
			}

			std::error_condition flush() override
			{
				return m_dst->flush();
			}

			std::error_condition write(const void* buffer, int64_t size) override
			{
				return m_dst->append(nonstd::string_view(static_cast<const char*>(buffer), size));
			}

			const char* get_name() const override { return "FileOutputStream"; }

		private:
			WritableFile* m_dst;
			bool m_free;

			SG_DELETE_COPY_AND_ASSIGN(CFileOutputStream);
		};
	}
}

#endif

#include <shogun/io/stream/FileInputStream.h>

using namespace std;
using namespace shogun::io;

CFileInputStream::CFileInputStream(RandomAccessFile* src, bool free):
	CInputStream(), m_src(src), m_free(free), m_pos(0) {}
CFileInputStream::~CFileInputStream()
{
	if (m_free)
		delete m_src;
}

error_condition CFileInputStream::read(string* buffer, int64_t size)
{
	if (size < 0)
		return make_error_condition(errc::invalid_argument);

	buffer->clear();
	buffer->resize(size);
	char* result_buffer = &(*buffer)[0];
	nonstd::string_view data;
	auto r = m_src->read(m_pos, size, &data, result_buffer);

	if (data.data() != result_buffer)
		memmove(result_buffer, data.data(), data.size());

		buffer->resize(data.size());
		if (!r || io::is_out_of_range(r))
		m_pos += data.size();
	return r;
}

static constexpr int64_t kMaxSkipSize = 8 * 1024 * 1024;

error_condition CFileInputStream::skip(int64_t bytes)
{
	if (bytes < 0)
		return make_error_condition(errc::invalid_argument);

	unique_ptr<char[]> scratch(new char[kMaxSkipSize]);
	if (bytes > 0)
	{
		// try quickly jumping on the position
		nonstd::string_view data;
		auto r = m_src->read(m_pos + bytes - 1, 1, &data, scratch.get());
		if ((!r || io::is_out_of_range(r)) && data.size() == 1)
		{
			m_pos += bytes;
			return {};
		}
	}

	while (bytes > 0)
	{
		// skip until we can and then
		auto bytes_to_read = min<int64_t>(kMaxSkipSize, bytes);
		nonstd::string_view data;
		auto r = m_src->read(m_pos, bytes, &data, scratch.get());
		if (!r || io::is_out_of_range(r))
			m_pos += data.size();
		else
		 	return r;

		if (data.size() < bytes_to_read)
		  return make_error_condition(ShogunErrc::OutOfRange);
		bytes -= bytes_to_read;
	}
	return {};
}

int64_t CFileInputStream::tell() const
{
	return m_pos;
}

void CFileInputStream::reset()
{
	m_pos = 0;
}

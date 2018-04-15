#ifndef SHOGUN_PATH_H
#define SHOGUN_PATH_H

#include <string>
#include <string_view.hpp>

namespace shogun
{
	namespace io
	{
		namespace detail
		{
			std::string join_path(std::initializer_list<nonstd::string_view> paths);
		}

#ifndef SWIG
		template <typename... T>
		std::string join_path(const T&... args)
		{
		  return detail::join_path({args...});
		}
#endif
		bool is_absolute_path(nonstd::string_view path);

		void parse_uri(
			nonstd::string_view remaining, nonstd::string_view* scheme,
			nonstd::string_view* host,nonstd::string_view* path);
	}
}

#endif

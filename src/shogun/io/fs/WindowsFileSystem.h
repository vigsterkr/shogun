#ifndef __WINDOWS_FILE_SYSTEM_H__
#define __WINDOWS_FILE_SYSTEM_H__

#include <shogun/io/fs/FileSystem.h>

namespace shogun
{
	namespace io
	{

		class WindowsFileSystem : public FileSystem
		{
		public:
			WindowsFileSystem() {}

			~WindowsFileSystem() {}

			std::error_condition new_random_access_file(
				const std::string& filename, std::unique_ptr<RandomAccessFile>* file) const override;

			std::error_condition new_writable_file(
				const std::string& fname, std::unique_ptr<WritableFile>* file) const override;

			std::error_condition new_appendable_file(
				const std::string& fname, std::unique_ptr<WritableFile>* file) const override;

			std::error_condition file_exists(const std::string& fname) const override;

			std::error_condition delete_file(const std::string& fname) const override;

			std::error_condition create_dir(const std::string& name) const override;

			std::error_condition delete_dir(const std::string& name) const override;

			std::error_condition rename_file(const std::string& src, const std::string& target) const override;

			uint64_t get_file_size(const std::string& fname) const override;

			std::error_condition is_directory(const std::string& fname) const override;

			std::error_condition get_children(const std::string& dir,
				std::vector<std::string>* result) const override;

			std::error_condition get_paths(const std::string& pattern,
				std::vector<std::string>* results) const override;
		};

		class LocalWindowsFileSystem : public WindowsFileSystem
		{
		public:
			std::string translate_name(const std::string& name) const override
			{
				nonstd::string_view scheme, host, path;
				parse_uri(name, &scheme, &host, &path);
				return path.to_string();
			}
		};
	}

}  // namespace shogun

#endif  // __WINDOWS_FILE_SYSTEM_H__

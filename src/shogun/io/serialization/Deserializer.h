/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn
 */
#ifndef __DESERIALIZER_H__
#define __DESERIALIZER_H__

#include <shogun/base/SGObject.h>
#include <shogun/io/stream/InputStream.h>

namespace shogun
{
	namespace io
	{
		class CDeserializer : public CSGObject
		{
		public:
			CDeserializer();
			virtual ~CDeserializer();
			virtual void attach(std::shared_ptr<CInputStream> stream);
			virtual std::shared_ptr<CSGObject> read_object() = 0;
			virtual void read(std::shared_ptr<CSGObject> _this) = 0;
			std::shared_ptr<CInputStream> stream() const;

		private:
			std::shared_ptr<CInputStream> m_stream;
		};

		std::shared_ptr<CSGObject> deserialize(const std::string& _path, std::shared_ptr<CDeserializer> _deser);
		void pre_deserialize(std::shared_ptr<CSGObject> obj) noexcept(false);
		void post_deserialize(std::shared_ptr<CSGObject> obj) noexcept(false);

	}
}

#endif

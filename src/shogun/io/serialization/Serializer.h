/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn
 */
#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <shogun/base/SGObject.h>
#include <shogun/io/stream/OutputStream.h>

namespace shogun
{
	namespace io
	{
		class CSerializer : public CSGObject
		{
		public:
			CSerializer();
			virtual ~CSerializer();
			virtual void attach(std::shared_ptr<COutputStream> stream);
			virtual void write(std::shared_ptr<CSGObject> object) noexcept(false) = 0;
			std::shared_ptr<COutputStream> stream() const;

		private:
			std::shared_ptr<COutputStream> m_stream;
		};

		void serialize(const std::string& _path, std::shared_ptr<CSGObject> _obj, std::shared_ptr<CSerializer> _serializer);
		void pre_serialize(std::shared_ptr<CSGObject> obj) noexcept(false);
		void post_serialize(std::shared_ptr<CSGObject> obj) noexcept(false);
	}
}

#endif

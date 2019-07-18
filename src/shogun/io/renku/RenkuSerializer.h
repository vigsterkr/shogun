/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal
 */
#ifndef __RENKU_SERIALIZER_H__
#define __RENKU_SERIALIZER_H__

#include <shogun/io/serialization/Serializer.h>

namespace shogun
{
	namespace renku
	{
		class RenkuSerializer : public io::Serializer
		{
		public:
			RenkuSerializer();
			virtual ~RenkuSerializer();
			virtual void write(const std::shared_ptr<SGObject>& object) noexcept(false);

			virtual const char* get_name() const
			{
				return "RenkuSerializer";
			}
		};
	}
}

#endif

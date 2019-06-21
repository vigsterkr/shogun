/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal
 */
#ifndef __BITSERY_DESERIALIZER_H__
#define __BITSERY_DESERIALIZER_H__

#include <shogun/io/serialization/Deserializer.h>

namespace shogun
{
	namespace io
	{
		class CBitseryDeserializer : public CDeserializer
		{
		public:
			CBitseryDeserializer();
			~CBitseryDeserializer() override;
			std::shared_ptr<CSGObject> read_object() override;
			void read(std::shared_ptr<CSGObject> _this) override;

			const char* get_name() const override
			{
				return "BitseryDeserializer";
			}
		};
	}
}

#endif

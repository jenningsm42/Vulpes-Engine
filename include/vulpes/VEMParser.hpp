#ifndef _VUL_VEMPARSER_HPP
#define _VUL_VEMPARSER_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "Mesh.hpp"
#include "MeshData.hpp"

namespace vul
{
	class VEAPI VEMParser
	{
	public:
		VEMParser();
		~VEMParser();

		bool parse(MeshData*, const uint8_t* buffer, uint32_t size);
	};
}

#endif // _VUL_VEMPARSER_HPP

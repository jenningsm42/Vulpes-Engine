#ifndef _VUL_VEMPARSER_H
#define _VUL_VEMPARSER_H
#include "Export.h"
#include "Mesh.h"
#include "MeshData.h"
#include "Handle.h"

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

#endif // _VUL_VEMPARSER_H
#ifndef _VUL_MATERIALUNIFORMVALUES_H
#define _VUL_MATERIALUNIFORMVALUES_H
#include "Export.h"
#include <cstdint>

namespace vul
{
	enum struct MaterialUniformType
	{
		Float1,
		Float2,
		Float3,
		Float4,
		Float2x2,
		Float3x3,
		Float4x4,
		Int1,
		Int2,
		Int3,
		Int4,
	};

	union MaterialUniformValue
	{
		float float1;
		float float2[2];
		float float3[3];
		float float4[4];
		float float2x2[2][2];
		float float3x3[3][3];
		float float4x4[4][4];
		int32_t int1;
		int32_t int2[2];
		int32_t int3[3];
		int32_t int4[4];
	};

	struct MaterialUniform
	{
		char name[32];
		MaterialUniformType type;
		MaterialUniformValue value;
	};

	class VEAPI MaterialUniforms
	{
	public:
	};
}

#endif // _VUL_MATERIALUNIFORMVALUES_H
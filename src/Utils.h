#pragma once

#include "Offsets.h"

namespace Utils
{
	[[nodiscard]] inline RE::NiPoint3 HkVectorToNiPoint(const RE::hkVector4& vec, bool bConvertScale = false) 
	{ 
		RE::NiPoint3 ret = { vec.quad.m128_f32[0], vec.quad.m128_f32[1], vec.quad.m128_f32[2] };
		if (bConvertScale) {
			ret *= *g_worldScaleInverse;
		}
		return ret;
	}
	
	[[nodiscard]] inline RE::hkVector4 NiPointToHkVector(const RE::NiPoint3& pt, bool bConvertScale = false) 
	{
		RE::hkVector4 ret = { pt.x, pt.y, pt.z, 0 };
		if (bConvertScale) {
			ret = ret * *g_worldScale;
		}
		return ret;
	}

	inline RE::NiPoint2 Vec2Rotate(const RE::NiPoint2& vec, float angle)
	{
		RE::NiPoint2 ret;
		ret.x = vec.x * cos(angle) - vec.y * sin(angle);
		ret.y = vec.x * sin(angle) + vec.y * cos(angle);
		return ret;
	}

	inline float Vec2Length(const RE::NiPoint2& vec)
	{
		return std::sqrtf(vec.x * vec.x + vec.y * vec.y);
	}

	inline RE::NiPoint2 Vec2Normalize(RE::NiPoint2& vec)
	{
		RE::NiPoint2 ret(0.f, 0.f);
		float vecLength = Vec2Length(vec);
		if (vecLength == 0) {
			return ret;
		}
		float invlen = 1.0f / vecLength;
		ret.x = vec.x * invlen;
		ret.y = vec.y * invlen;
		return ret;
	}

	[[nodiscard]] inline RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis)
	{
		float S = sin(angle);
		float C = cos(angle);

		const float XX = axis.x * axis.x;
		const float YY = axis.y * axis.y;
		const float ZZ = axis.z * axis.z;

		const float XY = axis.x * axis.y;
		const float YZ = axis.y * axis.z;
		const float ZX = axis.z * axis.x;

		const float XS = axis.x * S;
		const float YS = axis.y * S;
		const float ZS = axis.z * S;

		const float OMC = 1.f - C;

		return RE::NiPoint3(
			(OMC * XX + C) * vec.x + (OMC * XY - ZS) * vec.y + (OMC * ZX + YS) * vec.z,
			(OMC * XY + ZS) * vec.x + (OMC * YY + C) * vec.y + (OMC * YZ - XS) * vec.z,
			(OMC * ZX - YS) * vec.x + (OMC * YZ + XS) * vec.y + (OMC * ZZ + C) * vec.z);
	}

	inline float DotProduct(RE::NiPoint2& a, RE::NiPoint2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float CrossProduct(RE::NiPoint2& a, RE::NiPoint2& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	inline float GetAngle(RE::NiPoint2& a, RE::NiPoint2& b)
	{
		return atan2(CrossProduct(a, b), DotProduct(a, b));
	}

	inline RE::NiMatrix3 MakeRotationMatrixFromX(const RE::NiPoint3& a_xAxis)
	{
		// try to use up if possible
		RE::NiPoint3 upVector = (fabs(a_xAxis.z) < (1.f - 1.e-4f)) ? RE::NiPoint3{ 0.f, 0.f, 1.f } : RE::NiPoint3{ 1.f, 0.f, 0.f };

		RE::NiPoint3 newY = upVector.UnitCross(a_xAxis);
		RE::NiPoint3 newZ = a_xAxis.Cross(newY);

		return RE::NiMatrix3(a_xAxis, newY, newZ);
	}

	inline RE::NiMatrix3 MakeRotationMatrixFromY(const RE::NiPoint3& a_yAxis)
	{
		// try to use up if possible
		RE::NiPoint3 upVector = (fabs(a_yAxis.z) < (1.f - 1.e-4f)) ? RE::NiPoint3{ 0.f, 0.f, 1.f } : RE::NiPoint3{ 1.f, 0.f, 0.f };

		RE::NiPoint3 newZ = upVector.UnitCross(a_yAxis);
		RE::NiPoint3 newX = a_yAxis.Cross(newZ);

		return RE::NiMatrix3(newX, a_yAxis, newZ);
	}

	inline RE::NiMatrix3 MakeRotationMatrixFromZ(const RE::NiPoint3& a_zAxis)
	{
		// try to use up if possible
		RE::NiPoint3 upVector = (fabs(a_zAxis.z) < (1.f - 1.e-4f)) ? RE::NiPoint3{ 0.f, 0.f, 1.f } : RE::NiPoint3{ 1.f, 0.f, 0.f };

		RE::NiPoint3 newX = upVector.UnitCross(a_zAxis);
		RE::NiPoint3 newY = a_zAxis.Cross(newX);

		return RE::NiMatrix3(newX, newY, a_zAxis);
	}

	inline RE::NiMatrix3 MakeRotationMatrixFromXY(const RE::NiPoint3& a_xAxis, const RE::NiPoint3& a_yAxis)
	{
		RE::NiPoint3 newZ = a_xAxis.UnitCross(a_yAxis);
		RE::NiPoint3 newY = newZ.Cross(a_xAxis);

		return RE::NiMatrix3(a_xAxis, newY, newZ);
	}

	inline RE::NiMatrix3 MakeRotationMatrixFromXZ(const RE::NiPoint3& a_xAxis, const RE::NiPoint3& a_zAxis)
	{
		RE::NiPoint3 newY = a_zAxis.UnitCross(a_xAxis);
		RE::NiPoint3 newZ = a_xAxis.Cross(newY);

		return RE::NiMatrix3(a_xAxis, newY, newZ);
	}

	[[nodiscard]] inline RE::NiQuaternion MatrixToQuaternion(const RE::NiMatrix3& m)
	{
		RE::NiQuaternion q;
		NiMatrixToNiQuaternion(q, m);
		return q;
	}

	void FillCloningProcess(RE::NiCloningProcess& a_cloningProcess, const RE::NiPoint3& a_scale);

	template <class T>
	T* Clone(T* a_object, const RE::NiPoint3& a_scale)
	{
		if (!a_object) {
			return nullptr;
		}

		RE::NiCloningProcess cloningProcess{};
		FillCloningProcess(cloningProcess, a_scale);

		auto clone = reinterpret_cast<T*>(NiObject_Clone(a_object, cloningProcess));

		/*CleanupProcessMap(cloningProcess.processMap);
		CleanupCloneMap(cloningProcess.cloneMap);*/

		return clone;
	}

	void ToggleCharacterBumper(RE::Actor* a_actor, bool a_bEnable);

}

#pragma once
#include "FileID.h"

namespace Animation
{
	struct RawOzzFaceAnimation
	{
		std::array<ozz::animation::offline::RawFloatTrack, RE::BSFaceGenAnimationData::morphSize> tracks;
		float duration;
	};

	struct RawOzzAnimation
	{
		ozz::unique_ptr<ozz::animation::offline::RawAnimation> data = nullptr;
		std::unique_ptr<RawOzzFaceAnimation> faceData = nullptr;
	};

	struct OzzFaceAnimation
	{
		std::array<ozz::animation::FloatTrack, RE::BSFaceGenAnimationData::morphSize> tracks;
		float duration;
	};

	struct OzzAnimation
	{
		struct ExtraData
		{
			float loadTime = -1.0f;
			AnimID id;
		};

		ozz::unique_ptr<ozz::animation::Animation> data = nullptr;
		std::unique_ptr<OzzFaceAnimation> faceData = nullptr;
		ExtraData extra;

		size_t GetSize();

		~OzzAnimation() noexcept;
	};

	struct OzzSkeleton
	{
		ozz::unique_ptr<ozz::animation::Skeleton> data = nullptr;
		size_t lEyeIdx = UINT64_MAX;
		size_t rEyeIdx = UINT64_MAX;
		std::string name;

		std::unique_ptr<std::vector<ozz::math::Transform>> GetRestPose() const;
	};
}
#pragma once
#include "PNode.h"

namespace Animation::Procedural
{
	struct SpringPhysicsJob
	{
		struct Context
		{
			ozz::math::SimdFloat4 restOffset;
			ozz::math::SimdFloat4 physicsPositionWS;
			ozz::math::SimdFloat4 velocity = ozz::math::simd_float4::zero();
			float deltaTime = 1.0f;
			bool initialized = false;
		};

		float stiffness;
		float damping;
		float mass;
		ozz::math::SimdFloat4 gravity;
		const ozz::math::Float4x4* boneTransform;    // Model-space transform.
		const ozz::math::Float4x4* parentTransform;  // Model-space transform.
		const ozz::math::Float4x4* rootTransform;    // World-space transform.
		Context* context;

		ozz::math::SimdFloat4* positionOutput;  // Local-space transform.

		bool Run();
	};

	class PSpringBoneNode : public PNodeT<PSpringBoneNode>
	{
	public:
		struct InstanceData : public PNodeInstanceData
		{
			SpringPhysicsJob::Context context;
		};

		uint16_t boneIdx;
		uint16_t parentIdx;

		virtual std::unique_ptr<PNodeInstanceData> CreateInstanceData() override;
		virtual PEvaluationResult Evaluate(PNodeInstanceData* a_instanceData, PoseCache& a_poseCache, PEvaluationContext& a_evalContext) override;
		virtual void AdvanceTime(PNodeInstanceData* a_instanceData, float a_deltaTime) override;
		virtual bool SetCustomValues(const std::span<PEvaluationResult>& a_values, const std::string_view a_skeleton) override;

		inline static Registration _reg{
			"spring_bone",
			{
				{ "pose", PEvaluationType<PoseCache::Handle> },
				{ "stiffness", PEvaluationType<float> },
				{ "damping", PEvaluationType<float> },
				{ "mass", PEvaluationType<float> },
				{ "gravity", PEvaluationType<ozz::math::Float4> },
			},
			{
				{ "bone", PEvaluationType<RE::BSFixedString> }
			},
			PEvaluationType<PoseCache::Handle>,
			CreateNodeOfType<PSpringBoneNode>
		};
	};
}
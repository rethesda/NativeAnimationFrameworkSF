#pragma once
#include "Animation/PoseCache.h"
#include "Animation/Ozz.h"
#include "Util/General.h"

namespace Animation::Procedural
{
	class PNode;
	class PGraph;
	using PEvaluationResult = std::variant<float, PoseCache::Handle, uint64_t, std::string>;

	template <typename T>
	inline constexpr std::size_t PEvaluationType = variant_index<T, PEvaluationResult>::value;

	struct PNodeInstanceData
	{
		virtual ~PNodeInstanceData() = default;
	};

	struct PEvaluationContext
	{
		struct SyncData
		{
			PNode* node;
			PNodeInstanceData* selfInstData;
			PNodeInstanceData* ownerInstData;
		};

		std::vector<std::unique_ptr<PNodeInstanceData>> nodeInstances;
		std::vector<PEvaluationResult> results;
		PEvaluationContext* lastSyncOwner = nullptr;
		std::vector<SyncData> syncMap;
	};

	class PNode
	{
	public:
		struct InputConnection
		{
			InputConnection(const char* a_name, size_t a_evalType, bool a_optional = false);

			const char* name;
			size_t evalType;
			bool optional;
		};

		struct Registration
		{
			typedef std::unique_ptr<PNode> (*CreationFunctor)();

			Registration(const char* a_typeName,
				const std::vector<InputConnection>& a_inputs,
				const std::vector<std::pair<const char*, size_t>>& a_customValues,
				size_t a_output,
				CreationFunctor a_createFunctor);

			const char* typeName;
			std::vector<InputConnection> inputs;
			std::vector<std::pair<const char*, size_t>> customValues;
			size_t output;
			CreationFunctor createFunctor;
		};

		uint64_t syncId = UINT64_MAX;
		std::vector<uint64_t> inputs;

		virtual std::unique_ptr<PNodeInstanceData> CreateInstanceData();
		virtual PEvaluationResult Evaluate(PNodeInstanceData* a_instanceData, PoseCache& a_poseCache, PEvaluationContext& a_evalContext) = 0;
		virtual void AdvanceTime(PNodeInstanceData* a_instanceData, float a_deltaTime);
		virtual void Synchronize(PNodeInstanceData* a_instanceData, PNodeInstanceData* a_ownerInstance, float a_correctionDelta);
		virtual bool SetCustomValues(const std::span<PEvaluationResult>& a_values, const std::string_view a_skeleton);
		virtual Registration* GetTypeInfo();
		virtual ~PNode() = default;

		template <typename T>
		static std::unique_ptr<PNode> CreateNodeOfType()
		{
			return std::make_unique<T>();
		}
	};

	std::unordered_map<std::string_view, PNode::Registration*>& GetRegisteredNodeTypes();

	template <typename T>
	class PNodeT : public PNode
	{
	public:
		virtual Registration* GetTypeInfo() override
		{
			return &T::_reg;
		}
	};

	template <typename T>
	bool IsNodeOfType(PNode* a_node)
	{
		return a_node->GetTypeInfo() == &T::_reg;
	}
}
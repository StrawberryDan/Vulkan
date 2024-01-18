#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Shader.hpp"
#include "Buffer.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSet.hpp"
#include "PipelineLayout.hpp"
// Vulkan
#include <vulkan/vulkan.h>
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <map>
#include <vector>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Vulkan
{
	class Device;
	class Sampler;
	class ImageView;
	class RenderPass;


	class Pipeline
	{
		friend class Builder;
		friend class CommandBuffer;
		friend class Framebuffer;


	public:
		class Builder;


	public:
		Pipeline(const Pipeline& rhs) = delete;
		Pipeline& operator=(const Pipeline& rhs) = delete;
		Pipeline(Pipeline&& rhs) noexcept;
		Pipeline& operator=(Pipeline&& rhs) noexcept;
		~Pipeline();


		template <std::movable T, typename... Args>
		T Create(const Args&... args) const { return T(*this, std::forward<const Args&>(args)...); }


	private:
		Pipeline();


	private:
		// Our RenderPass
		Core::ReflexivePointer<RenderPass> mRenderPass;
		// Our Layout
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;

		// Handle to pipeline
		VkPipeline mPipeline = nullptr;
		// The size of the viewport to render to
		VkViewport mViewport;
	};


	class VertexInputDescription
	{
	private:
		class Binding;


		class Attribute;


	public:
		Binding& AddBinding(uint32_t stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);


		const Binding& operator[](const uint32_t index) const;


		inline uint32_t Size() const;


		std::vector<VkVertexInputBindingDescription> GetBindingDescriptions() const;
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() const;


	private:
		std::vector<Binding> mBindings;
	};


	class VertexInputDescription::Binding
	{
		friend class VertexInputDescription;


	public:
		Binding(uint32_t stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

		Binding& WithAttribute(uint32_t location, VkFormat format, uint32_t offset);

	private:
		uint32_t mStride;
		VkVertexInputRate mInputRate;
		std::vector<Attribute> mAttributes;
	};


	class VertexInputDescription::Attribute
	{
		friend class VertexInputDescription;


	public:
		Attribute(uint32_t location, VkFormat format, uint32_t offset);


	private:
		uint32_t mLocation;
		VkFormat mFormat;
		uint32_t mOffset;
	};


	class Pipeline::Builder
	{
	public:
		explicit Builder(const PipelineLayout& layout, const RenderPass& renderPass, uint32_t subpass = 0);


		Builder& WithShaderStage(VkShaderStageFlagBits stage, Shader shader);


		Builder& WithVertexInput(VertexInputDescription description);


		Builder& WithPrimitiveTopology(VkPrimitiveTopology topology);


		Pipeline::Builder& WithViewport(Core::Math::Vec2f offset, Core::Math::Vec2f size);


		Pipeline::Builder& WithDepthClamping();


		Pipeline::Builder& WithDepthTesting();


		Pipeline::Builder& WithColorBlending();


		Builder& WithCullMode(VkCullModeFlags cullModeFlags);


		[[nodiscard]] Pipeline Build() const;


	private:
		Core::ReflexivePointer<PipelineLayout> mPipelineLayout;

		Core::ReflexivePointer<RenderPass> mRenderPass;

		uint32_t mSubpass;

		std::map<VkShaderStageFlagBits, Shader> mStages;

		VertexInputDescription mVertexInputDescription;

		Core::Optional<VkPrimitiveTopology> mPrimitiveTopology;

		Core::Optional<VkViewport> mViewport;

		VkBool32 mDepthClampEnabled = VK_FALSE;

		bool mDepthTestingEnabled = true;

		bool mColorBlendingEnabled = true;

		VkPolygonMode mPolygonMode = VK_POLYGON_MODE_FILL;

		VkCullModeFlags mCullingMode = VK_CULL_MODE_NONE;

		VkFrontFace mFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		float mLineWidth = 1.0;
	};
}

#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Shader.hpp"
#include "Strawberry/Graphics/Vulkan/Buffer.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSet.hpp"
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
namespace Strawberry::Graphics::Vulkan
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


		void SetUniformBuffer(const Vulkan::Buffer& buffer, uint32_t set, uint32_t binding, uint32_t arrayElement = 0);


		void SetUniformTexture(const Sampler& sampler, const ImageView& image, VkImageLayout layout, uint32_t set, uint32_t binding, uint32_t arrayElement = 0);


	private:
		Pipeline(DescriptorPool descriptorPool);


	private:
		// Our RenderPass
		Core::ReflexivePointer<RenderPass> mRenderPass;

		// Handle to pipeline
		VkPipeline mPipeline = nullptr;
		// Handle to pipeline layout
		VkPipelineLayout mPipelineLayout = nullptr;
		// The size of the viewport to render to
		VkViewport mViewport;
		// Our descriptor sets
		std::vector<DescriptorSet> mDescriptorSets;
		// Our descriptor set layouts
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		// The pool from which we allocate descriptor sets
		DescriptorPool mDescriptorPool;
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


	class DescriptorSetLayout
	{
		friend class Pipeline;


	public:
		DescriptorSetLayout& WithBinding(VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);


	private:
		std::vector<VkDescriptorSetLayoutBinding> mBindings;
	};


	class Pipeline::Builder
	{
	public:
		explicit Builder(const RenderPass& renderPass);


		Builder& WithShaderStage(VkShaderStageFlagBits stage, Shader shader);


		Builder& WithVertexInput(VertexInputDescription description);


		Builder& WithPrimitiveTopology(VkPrimitiveTopology topology);


		Pipeline::Builder& WithViewport(Core::Math::Vec2f offset, Core::Math::Vec2f size);


		Builder& WithCullMode(VkCullModeFlags cullModeFlags);


		Builder& WithPushConstantRange(VkShaderStageFlags stage, uint32_t size, uint32_t offset);


		Builder& WithDescriptorSetLayout(const DescriptorSetLayout& descriptorSetLayout);


		[[nodiscard]] Pipeline Build() const;


	private:
		Core::ReflexivePointer<RenderPass> mRenderPass;

		std::map<VkShaderStageFlagBits, Shader> mStages;

		VertexInputDescription mVertexInputDescription;

		Core::Optional<VkPrimitiveTopology> mPrimitiveTopology;

		Core::Optional<VkViewport> mViewport;

		VkBool32 mDepthClampEnabled = VK_FALSE;

		VkPolygonMode mPolygonMode = VK_POLYGON_MODE_FILL;

		VkCullModeFlags mCullingMode = VK_CULL_MODE_NONE;

		VkFrontFace mFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		float mLineWidth = 1.0;

		uint32_t mSubpass = 0;

		std::vector<VkPushConstantRange> mPushConstantRanges;

		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;

		std::vector<VkDescriptorPoolSize> mDescriptorPoolSizes;
	};
}

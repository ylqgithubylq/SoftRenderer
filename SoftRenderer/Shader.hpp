#pragma once
#include "Common.hpp"

namespace X
{

	class VertexShader
		: Noncopyable
	{
	public:
		void operator ()(void const* attributeInput, void const* constantInput, void* attributeOutput)
		{
			Execute(attributeInput, constantInput, attributeOutput);
		}
	private:
		virtual void Execute(void const* attributeInput, void const* constantInput, void* attributeOutput) = 0;
	};

	class FragmentShader
		: Noncopyable
	{
	public:
		void operator ()(void const* fragmentInput, void const* constantInput, void* fragmentOutput)
		{
			Execute(fragmentInput, constantInput, fragmentOutput);
		}
	private:
		virtual void Execute(void const* fragmentInput, void const* constantInput, void* fragmentOutput) = 0;
	};

	/*
	*	Fake compute shader without shared memory and sync support.
	*
	*/
	class ComputeShader
		: Noncopyable
	{
	public:
		struct WorkSize
		{
			Size<u32, 3> groupCount;
			Size<u32, 3> groupSize;
			WorkSize(Size<u32, 3> groupCount, Size<u32, 3> groupSize)
				: groupCount(groupCount), groupSize(groupSize)
			{
			}
		};

	public:
		void operator ()(WorkSize const& size, Point<u32, 3> const& groupIndex, Point<u32, 3> const& threadIndex, void const* constantInput, void* resource)
		{
			Execute(size, groupIndex, threadIndex, constantInput, resource);
		}
	private:
		virtual void Execute(WorkSize const& size, Point<u32, 3> const& groupIndex, Point<u32, 3> const& threadIndex, void const* constantInput, void* resource) = 0;
	};

	class ComputeLauncher
		: Noncopyable
	{
	public:
		ComputeLauncher(std::shared_ptr<ComputeShader> shader, void const* constantInput, void* resources)
			: shader_(std::move(shader)), constantInput_(constantInput), resources_(resources)
		{
		}
		void Launch(ComputeShader::WorkSize const& size, u32 threadSupport);
	private:
		std::shared_ptr<ComputeShader> shader_;
		void const* constantInput_;
		void* resources_;
	};


}


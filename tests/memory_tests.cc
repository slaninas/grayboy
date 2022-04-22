#include "catch2/catch.hpp"
#include "memory.h"
#include "test_utils.h"

TEST_CASE("Initialization", "[memory]")
{
	auto mem = Memory{};
	CHECK(mem.dump() == Memory::ArrayType{});

	mem = {{0xea, 0x01, 0x02, 0xfe}};
	const auto correct_mem = Memory::ArrayType{0xea, 0x01, 0x02, 0xfe};
	CHECK(mem.dump() == correct_mem);
}

TEST_CASE("MakeMemory", "[memory]")
{
	SECTION("No init values")
	{
		const auto mem = MakeMemory{}.get();
		CHECK(mem.dump() == Memory::ArrayType{});
	}
	SECTION("Some init values")
	{
		auto mem = MakeMemory{{{0x02, 0xfe}, {0x03, 0xab}, {0x04, 0x12}}}.get();
		const auto correct_mem = Memory::ArrayType{0x00, 0x00, 0xfe, 0xab, 0x12};
		CHECK(mem.dump() == correct_mem);
	}
}

TEST_CASE("MemoryChanger", "[memory]")
{
	const auto mem = Memory{{0xfe, 0xab, 0xe1, 0x49}};

	SECTION("Nothing changed")
	{
		const auto changed_memory = MemoryChanger{}.get(mem);
		CHECK(changed_memory.dump() == mem.dump());
	}
	SECTION("Some changes")
	{
		auto changed_memory = MemoryChanger{{{0x00, 0x00}, {0x01, 0x12}, {0x02, 0x34}}}.get(mem);
		const auto correct_mem = Memory::ArrayType{0x00, 0x12, 0x34, 0x49};
		CHECK(changed_memory.dump() == correct_mem);
	}
}

TEST_CASE("Read/Write", "[memory]")
{
	const auto orig_mem = getRandomMemory();
	auto mem = orig_mem;

	mem.write(0xc000, 0xea);
	CHECK(mem.read(0xc000) == 0xea);

	mem.write(0xc001, 0xbc);
	CHECK(mem.read(0xc001) == 0xbc);

	mem.write(0xc002, 0x43);
	CHECK(mem.read(0xc002) == 0x43);

	mem.write(0xc003, 0xfe);
	CHECK(mem.read(0xc003) == 0xfe);

	const auto correct_mem = MemoryChanger{{{0xc000, 0xea}, {0xc001, 0xbc}, {0xc002, 0x43}, {0xc003, 0xfe}}}.get(orig_mem);
	CHECK(mem.dump() == correct_mem.dump());
}

TEST_CASE("Memory snapshot", "[memory]")
{
	const auto orig_memory = MemoryChanger{{{0xc034, 0x43}, {0xc0ba, 0x12}, {0xc001, 0x89}}}.get(getRandomMemory());
	auto snapshots = MemorySnapshots{orig_memory};
	auto memory = orig_memory;

	memory.write(0xc0ba, 0x33);
	snapshots.add(memory);

	memory.write(0xc034, 0x97);
	snapshots.add(memory);

	memory.write(0xc001, 0xba);
	snapshots.add(memory);

	SECTION("Zero back")
	{
		memory = snapshots.getMemory(0);
		const auto correct_memory = MemoryChanger{{{0xc0ba, 0x33}, {0xc034, 0x97}, {0xc001, 0xba}}}.get(orig_memory);
		CHECK(memory.dump() == correct_memory.dump());
	}

	SECTION("One back")
	{
		memory = snapshots.getMemory(1);
		const auto correct_memory = MemoryChanger{{{0xc0ba, 0x33}, {0xc034, 0x97}}}.get(orig_memory);
		CHECK(memory.dump() == correct_memory.dump());
	}

	SECTION("Two back")
	{
		const auto correct_memory = MemoryChanger{{{0xc0ba, 0x33}}}.get(orig_memory);
		memory = snapshots.getMemory(2);
		CHECK(memory.dump() == correct_memory.dump());
	}

	SECTION("Three back")
	{
		memory = snapshots.getMemory(3);
		CHECK(memory.dump() == orig_memory.dump());
	}

	SECTION("More writes at once")
	{
		memory.write(0xc034, 0xcd);
		memory.write(0xc0ba, 0x02);
		memory.write(0xc001, 0x1b);
		snapshots.add(memory);

		const auto correct_memory = MemoryChanger{{{0xc0ba, 0x02}, {0xc034, 0xcd}, {0xc001, 0x1b}}}.get(orig_memory);
		CHECK(snapshots.getMemory(0).dump() == correct_memory.dump());

		// Should be same as "Zero back"
		const auto correct_memory2 = MemoryChanger{{{0xc0ba, 0x33}, {0xc034, 0x97}, {0xc001, 0xba}}}.get(orig_memory);
		CHECK(snapshots.getMemory(1).dump() == correct_memory2.dump());
	}
}

TEST_CASE("Last diffs", "[memory]")
{
	const auto orig_memory = MemoryChanger{{{0xc034, 0x43}, {0xc0ba, 0x12}, {0xc001, 0x89}}}.get(getRandomMemory());
	auto snapshots = MemorySnapshots{orig_memory};
	auto memory = orig_memory;

	memory.write(0xc0ba, 0x33);
	snapshots.add(memory);

	memory.write(0xc034, 0x97);
	snapshots.add(memory);
	snapshots.add(memory);

	memory.write(0xc001, 0xba);
	memory.write(0xc0ba, 0xcd);
	snapshots.add(memory);
	snapshots.add(memory);
	memory.write(0xc0ba, 0x12);
	snapshots.add(memory);

	const auto last_diffs = snapshots.getLastNonEmptyDiffs(4);
	const auto correct_diffs = std::vector<MemoryDiff>{
		{0xc034, 0x43, 0x97},
		{0xc001, 0x89, 0xba},
		{0xc0ba, 0x33, 0xcd},
		{0xc0ba, 0xcd, 0x12},
	};
	CHECK(last_diffs == correct_diffs);
}

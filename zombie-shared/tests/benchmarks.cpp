#include "benchmarks.hpp"

#include <optional>
#include <chrono>
#include <iostream>
#include <robin_hood.h>

#include "entity/EntityManager.hpp"
#include "entity/ComponentContainer.hpp"


namespace {
	double Random(double i) {
		auto rand = 56783.0 * std::abs(std::sin(100.0 * i));
		return rand - std::uint64_t(rand);
	}

	std::size_t RandomIndex(double i, std::size_t size) {
		return static_cast<std::size_t>(Random(i) * 0.99 * size);
	}

	constexpr u64 c_num_benchmark_millis = 500;
}

class BenchmarkContext {
public:
	BenchmarkContext(std::string name) : m_name(name) {}
	bool ShouldRun() {
		if (!m_start) {
			m_start = std::chrono::steady_clock::now();
			m_real_elapsed = m_start.value();
		}
		++m_iterations;
		if (m_iterations % 100 == 0) {
			auto real_elapsed = std::chrono::steady_clock::now() - m_real_elapsed;
			if (real_elapsed > std::chrono::milliseconds(c_num_benchmark_millis)) {
				auto elapsed = m_accumulated_time + std::chrono::steady_clock::now() - m_start.value();
				std::cout << m_name << ": " << (u64)((double)elapsed.count() / (double)m_iterations) << " ns\n";
				return false;
			}
		}
		return true;
	}
	void PauseTimer() {
		m_accumulated_time += std::chrono::steady_clock::now() - m_start.value();
	}
	void ResumeTimer() {
		m_start = std::chrono::steady_clock::now();
	}
private:
	std::string m_name;
	std::optional<std::chrono::steady_clock::time_point> m_start;
	std::chrono::steady_clock::time_point m_real_elapsed;
	std::chrono::nanoseconds m_accumulated_time = std::chrono::nanoseconds(0);
	std::size_t m_iterations = 0;
};

template <class T>
inline auto DoNotOptimize(T const& datum) {
	return reinterpret_cast<char const volatile&>(datum);
}

#define BENCHMARK(name) \
void name(BenchmarkContext context); \
void name() { \
	name(BenchmarkContext(#name)); \
} \
void name(BenchmarkContext context) \

BENCHMARK(CreateEntity) {
	EntityManager entity_manager;

	while (context.ShouldRun()) {
		entity_manager.Create();
	}
}

BENCHMARK(HasEntity) {
	EntityManager entity_manager;

	for (int i = 0; i < 1000; ++i) {
		entity_manager.Create();
	}
	auto entity = entity_manager.Create();

	while (context.ShouldRun()) {
		DoNotOptimize(entity_manager.Has(entity));
	}
}

BENCHMARK(HasComponent) {
	struct Component {
		float a, b, c, d, e, f;
	};

	EntityManager entity_manager;
	ComponentContainer<Component> components(entity_manager);

	std::vector<Entity> entities;

	for (double seed = 0; seed < 5000.0; ++seed) {
		entities.push_back(entity_manager.Create());

		if (Random(seed) > 0.5) {
			components.Create(entities.back());
		}
	}

	while (context.ShouldRun()) {
		for (auto entity : entities) {
			auto value = components.Has(entity);
			DoNotOptimize(value);
		}
	}
}

BENCHMARK(HasComponent2) {
	struct Component {
		float a, b, c, d, e, f;
	};

	EntityManager entity_manager;
	
	std::vector<ComponentContainer<Component>> component_containers;

	for (int i = 0; i < 64; ++i) {
		component_containers.emplace_back(entity_manager);
	}

	std::vector<Entity> entities;
	for (int i = 0; i < 1000; ++i) {
		entities.push_back(entity_manager.Create());
	}

	for (int i = 0; i < 64; ++i) {
		auto& components = component_containers[i];
		for (double seed = i * 500.0; seed < 500.0; ++seed) {
			if (Random(seed) > 0.5) {
				components.Create(entities.back());
			}
		}
	}

	while (context.ShouldRun()) {
		for (auto entity : entities) {
			for (auto& components : component_containers) {
				auto value = components.Has(entity);
				DoNotOptimize(value);
			}
		}
	}
}

BENCHMARK(DestroyEntityComponents) {
	struct Component {
		float a, b, c, d, e, f;
	};

	EntityManager entity_manager;
	std::vector<ComponentContainer<Component>> component_containers;

	constexpr u64 c_num_component_types = 64;
	component_containers.reserve(c_num_component_types);
	for (int i = 0; i < c_num_component_types; ++i) {
		component_containers.emplace_back(entity_manager);
	}

	constexpr u64 c_count = 1000;

	std::vector<Entity> entities;
	entities.reserve(c_count);

	while (context.ShouldRun()) {
		context.PauseTimer();
		double seed = 0.f;
		for (int i = 0; i < c_count; ++i) {
			entities.push_back(entity_manager.Create());

			for (int j = 0; j < c_num_component_types; ++j) {
				if (Random(seed) > 0.95) {
					component_containers[j].Create(entities.back());
				}

				seed += 0.01;
			}
		}

		context.ResumeTimer();

		for (int i = 0; i < c_count; ++i) {
			entity_manager.Destroy(entities[i]);
		}
		entities.clear();
	}
}

constexpr u64 c_iterate_count = 10000;

BENCHMARK(IterateEntitiesEmpty) {
	EntityManager entity_manager;

	std::vector<Entity> entities;
	double seed = 0.f;
	for (int i = 0; i < c_iterate_count; ++i) {
		entities.push_back(entity_manager.Create());
	}
	for (int i = 0; i < c_iterate_count; ++i) {
		entity_manager.Destroy(entities[i]);
	}

	while (context.ShouldRun()) {
		for (auto entity : entity_manager) {
			DoNotOptimize(entity);
		}
	}
}

BENCHMARK(IterateEntitiesQuarterFull) {
	std::vector<Entity> entities;
	double seed = 0.f;
	EntityManager entity_manager;
	for (int i = 0; i < c_iterate_count; ++i) {
		entities.push_back(entity_manager.Create());
	}
	for (auto entity : entities) {
		if (Random(seed) > 0.25) {
			entity_manager.Destroy(entity);
		}
		seed += 0.001f;
	}

	while (context.ShouldRun()) {
		for (auto entity : entity_manager) {
			DoNotOptimize(entity);
		}
	}
}

BENCHMARK(IterateEntitiesHalfFull) {
	std::vector<Entity> entities;
	double seed = 0.f;
	EntityManager entity_manager;
	for (int i = 0; i < c_iterate_count; ++i) {
		entities.push_back(entity_manager.Create());
	}
	for (auto entity : entities) {
		if (Random(seed) > 0.5) {
			entity_manager.Destroy(entity);
		}
		seed += 0.001f;
	}

	while (context.ShouldRun()) {
		for (auto entity : entity_manager) {
			DoNotOptimize(entity);
		}
	}
}

BENCHMARK(IterateEntitiesThreeQuarterFull) {
	std::vector<Entity> entities;
	double seed = 0.f;
	EntityManager entity_manager;
	for (int i = 0; i < c_iterate_count; ++i) {
		entities.push_back(entity_manager.Create());
	}
	for (auto entity : entities) {
		if (Random(seed) > 0.75) {
			entity_manager.Destroy(entity);
		}
		seed += 0.001f;
	}

	while (context.ShouldRun()) {
		for (auto entity : entity_manager) {
			DoNotOptimize(entity);
		}
	}
}

BENCHMARK(IterateEntitiesFull) {
	EntityManager entity_manager;
	for (int i = 0; i < c_iterate_count; ++i) {
		entity_manager.Create();
	}
	while (context.ShouldRun()) {
		for (auto entity : entity_manager) {
			DoNotOptimize(entity);
		}
	}
}

constexpr std::uint64_t c_num_map_elements = 10000;
constexpr std::uint64_t c_map_data_bytes = 1;
struct MapStruct {
	std::uint8_t data[c_map_data_bytes];
};

#define ITERATE_MAP_BENCH(MAP_TYPE) \
	MAP_TYPE<std::uint64_t, MapStruct> map; \
	double seed = 0.0; \
	for (std::uint64_t i = 0; i < c_num_map_elements; ++i) { map[RandomIndex(seed, c_num_map_elements)]; seed += 0.1; } \
	while (context.ShouldRun()) { \
		for (const auto& element : map) { DoNotOptimize(element); } \
	} \


BENCHMARK(IterateStlUnorderedMap) {
	ITERATE_MAP_BENCH(std::unordered_map)
}
BENCHMARK(IterateRobinHoodUnorderedMap) {
	ITERATE_MAP_BENCH(robin_hood::unordered_node_map)
}
BENCHMARK(IterateRobinHoodUnorderedFlatMap) {
	ITERATE_MAP_BENCH(robin_hood::unordered_flat_map)
}
//BENCHMARK(IterateDumbMap) {
//	ITERATE_MAP_BENCH(DumbMap)
//}
//BENCHMARK(IterateSmartMap) {
//	ITERATE_MAP_BENCH(SmartMap)
//}

#define RANDOM_ACCESS_MAP_BENCH(MAP_TYPE) \
	MAP_TYPE<std::size_t, MapStruct> map; \
	for (std::size_t i = 0; i < c_num_map_elements; ++i) { map[i]; } \
	double seed = 0.f; \
	while (context.ShouldRun()) { \
		for (int i = 0; i < 100; ++i) { \
			seed += 0.1; \
			auto index = RandomIndex(seed, c_num_map_elements); \
			auto iter = map.find(index); \
			DoNotOptimize(iter); \
		} \
	} \

BENCHMARK(RandomAccessStlUnorderedMap) {
	RANDOM_ACCESS_MAP_BENCH(std::unordered_map)
}
BENCHMARK(RandomAccessRobinHoodUnorderedMap) {
	RANDOM_ACCESS_MAP_BENCH(robin_hood::unordered_node_map)
}
BENCHMARK(RandomAccessRobinHoodUnorderedFlatMap) {
	RANDOM_ACCESS_MAP_BENCH(robin_hood::unordered_flat_map)
}
//BENCHMARK(RandomAccessDumbMap) {
//	RANDOM_ACCESS_MAP_BENCH(DumbMap)
//}
//BENCHMARK(RandomAccessSmartMap) {
//	RANDOM_ACCESS_MAP_BENCH(SmartMap)
//}

void RunBenchmarks() {
	std::cout << "Running benchmarks\n";
	//CreateEntity();
	//HasEntity();
	//HasComponent();
	//HasComponent2();
	//DestroyEntityComponents();
	IterateEntitiesEmpty();
	IterateEntitiesQuarterFull();
	IterateEntitiesHalfFull();
	IterateEntitiesThreeQuarterFull();
	IterateEntitiesFull();

	//IterateStlUnorderedMap();
	//IterateRobinHoodUnorderedMap();
	//IterateRobinHoodUnorderedFlatMap();
	//IterateDumbMap();
	//IterateSmartMap();

	//RandomAccessStlUnorderedMap();
	//RandomAccessRobinHoodUnorderedMap();
	//RandomAccessRobinHoodUnorderedFlatMap();
	//RandomAccessDumbMap();
	//RandomAccessSmartMap();
}

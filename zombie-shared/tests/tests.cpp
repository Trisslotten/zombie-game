#include "tests.hpp"

#include <iostream>

#include "entity/EntityManager.hpp"
#include "entity/ComponentContainer.hpp"

#define EXPECT_EQ(a, b) if (a != b) { std::cout << "ERROR " << #a << " was not equal to " << #b << "(" << a << " != " << b << ")\n"; __debugbreak(); }
#define EXPECT_NE(a, b) if (a == b) { std::cout << "ERROR " << #a << " was equal to " << #b << "(" << a << " == " << b << ")\n"; __debugbreak(); }
#define EXPECT_TRUE(a) if (!a) { std::cout << "ERROR " << #a << " was not true\n"; __debugbreak(); }
#define EXPECT_FALSE(a) if (a) { std::cout << "ERROR " << #a << " was true\n"; __debugbreak(); }


void EntityIterationCountTest(u64 c_count = 1) {
	EntityManager entity_manager;
	for (int i = 0; i < c_count; ++i) {
		entity_manager.Create();
	}
	u64 count = 0;
	for (auto entity : entity_manager) {
		++count;
	}
	EXPECT_EQ(c_count, count);
}

void EntityTest() {
	{
		EntityManager entity_manager;

		auto entity1 = entity_manager.Create();
		auto entity2 = entity_manager.Create();
		EXPECT_TRUE(entity_manager.Has(entity1));
		EXPECT_TRUE(entity_manager.Has(entity2));

		EXPECT_NE(entity1.GetInternalValue(), entity2.GetInternalValue());

		entity_manager.Destroy(entity1);
		EXPECT_FALSE(entity_manager.Has(entity1));
		EXPECT_TRUE(entity_manager.Has(entity2));

		auto entity3 = entity_manager.Create();

		entity_manager.Destroy(entity2);
		EXPECT_FALSE(entity_manager.Has(entity1));
		EXPECT_FALSE(entity_manager.Has(entity2));

		EXPECT_NE(entity1.GetInternalValue(), entity3.GetInternalValue());
	}

	{
		EntityManager entity_manager;

		auto entity = entity_manager.Create();
		EXPECT_TRUE(entity_manager.Has(entity));

		entity_manager.Destroy(entity);
		EXPECT_FALSE(entity_manager.Has(entity));
	}

	{
		struct Component {
			float a, b, c, d, e, f;
		};

		EntityManager entity_manager;
		ComponentContainer<Component> components(entity_manager);
		
		auto entity = entity_manager.Create();
		EXPECT_FALSE(components.Has(entity));

		components.Create(entity);
		EXPECT_TRUE(components.Has(entity));

		components.Destroy(entity);
		EXPECT_FALSE(components.Has(entity));


		auto entity2 = entity_manager.Create();
		EXPECT_FALSE(components.Has(entity2));


		components.Create(entity);
		EXPECT_TRUE(components.Has(entity));
		components.Create(entity2);
		EXPECT_TRUE(components.Has(entity2));

		components.Destroy(entity);
		EXPECT_FALSE(components.Has(entity));
		components.Destroy(entity2);
		EXPECT_FALSE(components.Has(entity2));
	}

	EntityIterationCountTest(0);
	EntityIterationCountTest(1);
	EntityIterationCountTest(2);
	EntityIterationCountTest(63);
	EntityIterationCountTest(64);
	EntityIterationCountTest(65);
	EntityIterationCountTest(20000);
}

void RunTests() {
	std::cout << "Running tests...\n";

	EntityTest();

	std::cout << "Finished tests\n";
}

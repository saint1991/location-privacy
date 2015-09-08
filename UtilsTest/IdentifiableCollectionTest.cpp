#include "stdafx.h"
#include "CppUnitTest.h"
#include "IdentifiableCollection.h"
#include "Node.h"
#include "BasicPath.h"
#include "LatLng.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilsTest
{

	//Identifiableの派生クラスでもうまくいくか試すため
	class DerivedFromIdentifiable : public Identifiable<long> {
	public :
		DerivedFromIdentifiable(const long& id) : Identifiable<long>(id) {}
	};


	TEST_CLASS(IdentifiableCollectionTest)
	{
	public:
		const double ACCURACY = 1.0E-10;

		TEST_METHOD(IdentifiableCollection_Constructor)
		{
			Collection::IdentifiableCollection<long, Identifiable<long>> collection; 
			Collection::IdentifiableCollection<long, DerivedFromIdentifiable> collection2;
		}

		//とりあえず各メソッド実行してみて，コンパイルが通っているかチェック
		TEST_METHOD(IdentifiableCollection_CompileCheck) 
		{
			Collection::IdentifiableCollection<long, Identifiable<long>> collection;
			collection.add(Identifiable<long>(1L));
			collection.add(std::make_shared<Identifiable<long>>(2L));
			collection.read_by_id(1L);
			collection.get_by_id(1L);
			collection.remove_by_id(1L);
			collection.contains(2L);
			collection.contains(Identifiable<long>(2L));
			collection.contains(std::make_shared<Identifiable<long>>(2L));
			collection.foreach([](std::shared_ptr<Identifiable<long>> element) {

			});
			collection.foreach([](std::shared_ptr<Identifiable<long> const> element) {

			});
		}
		
		TEST_METHOD(IdentifiableCollection_read_by_id)
		{
			Collection::IdentifiableCollection<long, Identifiable<long>> collection;
			collection.add(Identifiable<long>(1L));
			std::shared_ptr<Identifiable<long> const> id = collection.read_by_id(1L);
			Assert::AreEqual(1L, id->get_id());
		}
		
		TEST_METHOD(IdentifiableCollection_get_by_id)
		{

			Collection::IdentifiableCollection<std::string, Identifiable<std::string>> collection;
			collection.add(Identifiable<std::string>("C"));
			collection.add(Identifiable<std::string>("A"));
			collection.add(Identifiable<std::string>("B"));
			std::shared_ptr<Identifiable<std::string>> id = collection.get_by_id("A");
			Assert::AreEqual("A", id->get_id().c_str());
			
			auto iter = collection.begin();
			Assert::AreEqual("A", (*iter)->get_id().c_str());
			iter++;
			Assert::AreEqual("B", (*iter)->get_id().c_str() );
			iter++;
			Assert::AreEqual("C", (*iter)->get_id().c_str());	
		}

		TEST_METHOD(IdentifiableCollection_read_by_id2)
		{
			Collection::IdentifiableCollection<Graph::node_id, Graph::Node<Geography::LatLng, Graph::BasicPath>> collection;
			collection.add(Graph::Node < Geography::LatLng, Graph::BasicPath>(1L, std::make_shared<Geography::LatLng>(10.0, 20.0)));
			std::shared_ptr<Graph::Node<Geography::LatLng, Graph::BasicPath> const> node = collection.read_by_id(1L);
			Assert::AreEqual(1L, node->get_id());
			Assert::AreEqual(10.0, node->data->lat(), ACCURACY);
			Assert::AreEqual(20.0, node->data->lng(), ACCURACY);
		}
		
		TEST_METHOD(IdentifiableCollection_remove_by_id)
		{
			Collection::IdentifiableCollection<Graph::node_id, Graph::Node<Geography::LatLng, Graph::BasicPath>> collection;
			collection.add(Graph::Node<Geography::LatLng,Graph::BasicPath>(1L,std::make_shared<Geography::LatLng>(10.0, 20.0)));
			Assert::IsFalse(collection.remove_by_id(2L));
			Assert::IsTrue(collection.remove_by_id(1L));

			std::shared_ptr<Graph::Node<Geography::LatLng, Graph::BasicPath> const> node = collection.read_by_id(1L);
			Assert::IsTrue(nullptr == node);
		}
		/*
		TEST_METHOD(IdentifiableCollection_get_id_list)
		{
			Collection::IdentifiableCollection<Graph::node_id, Graph::Node<Geography::LatLng, Graph::BasicPath>> collection;
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(1L, std::make_shared<Geography::LatLng>(10.0, 20.0)));
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(2L, std::make_shared<Geography::LatLng>(100.0, 20.0)));
			std::unique_ptr<std::vector<Graph::node_id> const> list = std::make_unique<std::vector<Graph::node_id> const>(collection.get_id_list());
			auto iter = list->begin();

			Assert::AreEqual(1L, *iter);
			iter++;
			Assert::AreEqual(2L, *iter);
		}*/

		TEST_METHOD(IdentifiableCollection_contains)
		{
			Collection::IdentifiableCollection<Graph::node_id, Graph::Node<Geography::LatLng, Graph::BasicPath>> collection;
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(1L, std::make_shared<Geography::LatLng>(10.0, 20.0)));
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(2L, std::make_shared<Geography::LatLng>(100.0, 20.0)));
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(1L, std::make_shared<Geography::LatLng>(15.0, 20.0)));
			std::shared_ptr<Graph::Node<Geography::LatLng, Graph::BasicPath>> node = collection.begin(); 

			Assert::IsTrue(collection.contains(1L));
			Assert::IsFalse(collection.contains(3L));
			Assert::IsTrue(collection.contains(std::make_shared<Graph::Node<Geography::LatLng, Graph::BasicPath>>(1L, std::make_shared<Geography::LatLng>(10.0, 20.0)));
			Assert::IsFalse(collection.contains(3L));

		}

		TEST_METHOD(IdentifiableCollection_add)
		{

		}

		TEST_METHOD(IdentifiableCollection_foreach)
		{
			Collection::IdentifiableCollection<Graph::node_id, Graph::Node<Geography::LatLng, Graph::BasicPath>> collection;
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(1L, std::make_shared<Geography::LatLng>(10.0, 20.0)));
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(2L, std::make_shared<Geography::LatLng>(100.0, 20.0)));
			collection.add(Graph::Node<Geography::LatLng, Graph::BasicPath>(3L, std::make_shared<Geography::LatLng>(15.0, 20.0)));
			std::shared_ptr<Graph::Node<Geography::LatLng, Graph::BasicPath> const> node = collection.begin();

			collection.foreach([]
				(){Assert::AreEqual(collection.read_by_id());

		}

	
	};
}

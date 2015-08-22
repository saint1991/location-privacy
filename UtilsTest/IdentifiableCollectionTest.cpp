#include "stdafx.h"
#include "CppUnitTest.h"
#include "IdentifiableCollection.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{

	//Identifiableの派生クラスでもうまくいくか試すため
	class DerivedFromIdentifiable : public Identifiable {
	public :
		DerivedFromIdentifiable(const long& id) : Identifiable(id) {}
	};


	TEST_CLASS(IdentifiableCollectionTest)
	{
	public:

		TEST_METHOD(IdentifiableCollection_Constructor)
		{
			Collection::IdentifiableCollection<Identifiable> collection;
			Collection::IdentifiableCollection<DerivedFromIdentifiable> collection2;
		}
	};
}

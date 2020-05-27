#include <iostream>
#include <mysqlx/xdevapi.h>

using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;


int32_t main(int32_t argc, const char* argv[])
try {

	const char* url = (argc > 1 ? argv[1] : "mysqlx://zx:Luck25.zx@localhost");

	cout << "Creating session on " << url
		<< " ..." << endl;

	Session sess(url);

	cout << "Session accepted, creating collection..." << endl;

	SqlResult result = sess.sql(R"(show databases;)").execute();
	if (result.hasData()) {
		std::cout << "column count: " << result.getColumnCount() << std::endl;
		std::cout << "row count: " << result.count() << std::endl;
		std::cout << "affected count: " << result.getAffectedItemsCount() << std::endl;
		std::cout << "warnning count: " << result.getWarningsCount() << std::endl;
	}
/*
	Collection coll = sch.createCollection("person", true);

	cout << "Inserting documents..." << endl;

	coll.remove("true").execute();

	{
		DbDoc doc(R"({ "name": "foo", "age1": 2})");

		Result add = coll.add(doc)
			.add(R"({ "name": "bar", "toys": [ "car", "ball" ] })")
			.add(R"({ "name": "bar", "age": 2, "toys": [ "car", "ball" ] })")
			.add(R"({ "name": "baz", "age": 3, "date": { "day": 20, "month": "Apr" }})")
			.add(R"({ "_id": "myuuid-1", "name": "foo", "age": 7 })")
			.execute();

		std::list<string> ids = add.getGeneratedIds();
		for (string id : ids)
			cout << "- added doc with id: " << id << endl;
	}

	cout << "Fetching documents..." << endl;

	DocResult docs = coll.find("age > 1 and name like 'ba%'").execute();

	int32_t i = 0;
	for (DbDoc doc : docs)
	{
		cout << "doc#" << i++ << ": " << doc << endl;

		for (Field fld : doc)
		{
			cout << " field `" << fld << "`: " << doc[fld] << endl;
		}

		string name = doc["name"];
		cout << " name: " << name << endl;

		if (doc.hasField("date") && Value::DOCUMENT == doc.fieldType("date"))
		{
			cout << "- date field" << endl;
			DbDoc date = doc["date"];
			for (Field fld : date)
			{
				cout << "  date `" << fld << "`: " << date[fld] << endl;
			}
			string month = doc["date"]["month"];
			int32_t day = date["day"];
			cout << "  month: " << month << endl;
			cout << "  day: " << day << endl;
		}

		if (doc.hasField("toys") && Value::ARRAY == doc.fieldType("toys"))
		{
			cout << "- toys:" << endl;
			for (auto toy : doc["toys"])
			{
				cout << "  " << toy << endl;
			}
		}

		cout << endl;
	}*/
	cout << "Done!" << endl;
}
catch (const mysqlx::Error& err)
{
	cout << "ERROR: " << err << endl;
	return 1;
}
catch (std::exception& ex)
{
	cout << "STD EXCEPTION: " << ex.what() << endl;
	return 1;
}
catch (const char* ex)
{
	cout << "EXCEPTION: " << ex << endl;
	return 1;
}
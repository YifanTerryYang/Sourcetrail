#include "cxxtest/TestSuite.h"

#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "settings/ApplicationSettings.h"

#include "helper/TestFileManager.h"

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

	void test_cxx_parser_finds_global_class_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:1 <1:7 1:7> 3:1>");
	}

	void test_cxx_parser_finds_global_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A;\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:7 1:7>");
	}

	void test_cxx_parser_finds_global_struct_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A <1:1 <1:8 1:8> 3:1>");
	}

	void test_cxx_parser_finds_global_struct_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A;\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A <1:8 1:8>");
	}

	void test_cxx_parser_finds_variable_definitions_in_global_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
		);

		TS_ASSERT_EQUALS(client->globalVariables.size(), 1);
		TS_ASSERT_EQUALS(client->globalVariables[0], "x <1:5 1:5>");
	}

	void test_cxx_parser_finds_fields_in_class_with_access_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	int a;\n"
			"public:\n"
			"	A() : d(0) {};\n"
			"	int b;\n"
			"protected:\n"
			"	static int c;\n"
			"private:\n"
			"	const int d;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 4);
		TS_ASSERT_EQUALS(client->fields[0], "private A::a <3:6 3:6>");
		TS_ASSERT_EQUALS(client->fields[1], "public A::b <6:6 6:6>");
		TS_ASSERT_EQUALS(client->fields[2], "protected A::c <8:13 8:13>");
		TS_ASSERT_EQUALS(client->fields[3], "private A::d <10:12 10:12>");
	}

	void test_cxx_parser_finds_function_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int ceil(float a)\n"
			"{\n"
			"	return 1;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int ceil(float) <1:1 <1:5 1:8> 4:1>");
	}

	void test_cxx_parser_finds_static_function_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"static int ceil(float a)\n"
			"{\n"
			"	return static_cast<int>(a) + 1;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "static int ceil(float) <1:1 <1:12 1:15> 4:1>");
	}

	void test_cxx_parser_finds_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B() <4:2 4:2>");
	}

	void test_cxx_parser_finds_method_declaration_and_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n"
			"B::B()\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 4);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B() <4:2 4:2>");
		TS_ASSERT_EQUALS(client->methods[1], "public void B::B(B const &) <1:7 1:7>");
		TS_ASSERT_EQUALS(client->methods[2], "public void B::B(B &) <1:7 1:7>");
		TS_ASSERT_EQUALS(client->methods[3], "public void B::B() <6:1 <6:4 6:4> 8:1>");
	}

	void test_cxx_parser_finds_virtual_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	virtual void process();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 4);
		TS_ASSERT_EQUALS(client->methods[0], "public virtual void B::process() <4:15 4:21>");
	}

	void test_cxx_parser_finds_pure_virtual_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"protected:\n"
			"	virtual void process() = 0;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 4);
		TS_ASSERT_EQUALS(client->methods[0], "protected pure virtual void B::process() <4:15 4:21>");
	}

	void test_cxx_parser_finds_named_namespace_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "A <1:1 <1:11 1:11> 3:1>");
	}

	void test_cxx_parser_finds_anonymous_namespace_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "anonymous namespace (input.cc) <1:1 3:1>");
	}

	void test_cxx_parser_finds_enum_defined_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "E <1:1 <1:6 1:6> 3:1>");
	}

	void test_cxx_parser_finds_enum_constant_in_global_enum()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"	P\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enumConstants.size(), 1);
		TS_ASSERT_EQUALS(client->enumConstants[0], "E::P <3:2 3:2>");
	}

	void test_cxx_parser_finds_typedef_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "uint <1:22 1:25>");
	}

	void test_cxx_parser_finds_typedef_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace test\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "test::uint <3:23 3:26>");
	}

	void test_cxx_parser_finds_typedef_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "anonymous namespace (input.cc)::uint <3:23 3:26>");
	}

	void test_cxx_parser_finds_macro_define()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->macros.size(), 1);
		TS_ASSERT_EQUALS(client->macros[0], "PI <1:9 <1:9 1:10> 1:8>");
	}

	void test_cxx_parser_finds_type_template_parameter_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T>::T <1:20 1:20>");
	}

	void test_cxx_parser_finds_type_template_parameter_defined_with_class_keyword()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <class T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<class T>::T <1:17 1:17>");
	}

	void test_cxx_parser_finds_non_type_int_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<int T>::T <1:15 1:15>");
	}

	void test_cxx_parser_finds_non_type_bool_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<bool T>::T <1:16 1:16>");
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<P * p>::p <3:14 3:14>");
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<P & p>::p <3:14 3:14>");
	}

	void test_cxx_parser_finds_non_type_template_parameter_that_depends_on_type_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T1, T1& T2>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T1, T1 & T2>::T1 <1:20 1:21>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "A<typename T1, T1 & T2>::T2 <1:28 1:29>");
	}

	void test_cxx_parser_finds_non_type_template_parameter_that_depends_on_template_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename> class T1, T1<int>& T2>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<template<typename> typename T1, T1<int> & T2>::T1<typename> <1:36 1:37>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "A<template<typename> typename T1, T1<int> & T2>::T2 <1:49 1:50>");
	}

	//void _test_cxx_parser_finds_template_argument_of_dependent_non_type_template_parameter()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"template <template<typename> class T1, T1<int>& T2>\n" // test that t1<int> uses int
	//		"class A\n"
	//		"{};\n"
	//	);
	//}

	void test_cxx_parser_finds_template_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"int main()\n"
			"{\n"
			"	B<A> ba;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T>::T <1:20 1:20>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "B<template<typename> typename T>::T<typename> <4:36 4:36>");
	}

	void test_cxx_parser_finds_type_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename... T>::T <1:23 1:23>");
	}



	void test_cxx_parser_finds_non_type_int_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<int... T>::T <1:18 1:18>");
	}

	void test_cxx_parser_finds_template_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename> typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<template<typename> typename... T>::T<typename> <1:42 1:42>");
	}

	void test_cxx_parser_finds_type_template_parameters_of_template_class_with_multiple_parameters()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T, typename U>::T <1:20 1:20>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "A<typename T, typename U>::U <1:32 1:32>");
	}

	void test_cxx_parser_skips_creating_node_for_template_parameter_without_a_name()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A<typename> <1:1 <2:7 2:7> 4:1>");
	}

	void test_cxx_parser_finds_type_template_parameter_of_template_method_definition_outside_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	template <typename U>\n"
			"	U foo();\n"
			"};\n"
			"template <typename T>\n"
			"template <typename U>\n"
			"U A<T>::foo()\n"
			"{}\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 3);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T>::foo<typename U>::U <4:21 4:21>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "A<typename T>::T <1:20 1:20>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[2], "A<typename T>::foo<typename U>::U <8:20 8:20>");
	}

	void test_cxx_parser_finds_class_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A<typename T> <1:1 <2:7 2:7> 4:1>");
		TS_ASSERT_EQUALS(client->classes[1], "A<int> <5:1 <6:7 6:7> 8:1>");
	}

	void test_cxx_parser_finds_class_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A<typename T, typename U> <1:1 <2:7 2:7> 4:1>");
		TS_ASSERT_EQUALS(client->classes[1], "A<typename T, int> <5:1 <6:7 6:7> 8:1>");
	}

	void test_cxx_parser_finds_correct_name_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A<typename T, typename U> <1:1 <2:7 2:7> 4:1>");
		TS_ASSERT_EQUALS(client->classes[1], "A<typename T, int> <5:1 <6:7 6:7> 8:1>");
	}

	void test_cxx_parser_finds_correct_field_member_name_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	int foo;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private A<typename T>::foo <4:6 4:8>");
	}

	void test_cxx_parser_finds_correct_type_of_field_member_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private A<typename T>::foo <4:4 4:6>");
	}

	void test_cxx_parser_finds_correct_method_member_name_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	int foo();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "private int A<typename T>::foo() <4:6 4:8>");
	}

	void test_cxx_parser_finds_type_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "test<typename T>::T <1:20 1:20>");
	}

	void test_cxx_parser_finds_non_type_int_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"int test(int a)\n"
			"{\n"
			"	return a + T;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "test<int T>::T <1:15 1:15>");
	}

	void test_cxx_parser_finds_non_type_bool_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"int test(int a)\n"
			"{\n"
			"	return T ? a : 0;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "test<bool T>::T <1:16 1:16>");
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "test<P * p>::p <3:14 3:14>");
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "test<P & p>::p <3:14 3:14>");
	}

	void test_cxx_parser_finds_template_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateParameterTypes[0], "A<typename T>::T <1:20 1:20>");
		TS_ASSERT_EQUALS(client->templateParameterTypes[1], "test<template<typename> typename T>::T<typename> <4:36 4:36>");
	}

	void test_cxx_parser_finds_function_for_implicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	return test(1);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 3);
		TS_ASSERT_EQUALS(client->functions[0], "T test<typename T>(T) <1:1 <2:3 2:6> 5:1>");
		TS_ASSERT_EQUALS(client->functions[1], "int test<int>(int) <2:1 <2:3 2:6> 5:1>");
		TS_ASSERT_EQUALS(client->functions[2], "int main() <7:1 <7:5 7:8> 10:1>");
	}

	void test_cxx_parser_finds_lambda_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaCaller()\n"
			"{\n"
			"	[](){}();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 2);
		TS_ASSERT_EQUALS(client->functions[0], "void lambdaCaller() <1:1 <1:6 1:17> 4:1>");
		TS_ASSERT_EQUALS(client->functions[1], "void lambdaCaller::lambda at 3:2() <3:5 <3:2 3:2> 3:7>");
		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "void lambdaCaller() -> void lambdaCaller::lambda at 3:2() <3:8 3:8>");
	}

	void test_cxx_parser_finds_definition_of_local_symbol_in_function_parameter_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void test(int a)\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 1);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc::a<1:11>");
	}

	void test_cxx_parser_finds_definition_of_local_symbol_in_function_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void test()\n"
			"{\n"
			"	int a;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 1);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc::a<3:2>");
	}

///////////////////////////////////////////////////////////////////////////////
// test finding nested symbol definitions and declarations

	void test_cxx_parser_finds_class_definition_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:1 <1:7 1:7> 5:1>");
		TS_ASSERT_EQUALS(client->classes[1], "public A::B <4:8 4:8>");
	}

	void test_cxx_parser_finds_class_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace a\n"
			"{\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "a::B <3:8 3:8>");
	}

	void test_cxx_parser_finds_struct_definition_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "private A::B <3:2 <3:9 3:9> 5:2>");
	}

	void test_cxx_parser_finds_struct_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
			);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A::B <3:2 <3:9 3:9> 5:2>");
	}

	void test_cxx_parser_finds_struct_definition_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo(int)\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
			"void foo(float)\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 2);
		TS_ASSERT_EQUALS(client->structs[0], "foo::B <3:2 <3:9 3:9> 5:2>");
		TS_ASSERT_EQUALS(client->structs[1], "foo::B <9:2 <9:9 9:9> 11:2>");
	}

	void test_cxx_parser_finds_variable_definitions_in_namespace_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n"
			"{\n"
			"	int x;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->globalVariables.size(), 1);
		TS_ASSERT_EQUALS(client->globalVariables[0], "n::x <2:6 2:6>");
	}

	void test_cxx_parser_finds_field_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	class C\n"
			"	{\n"
			"	private:\n"
			"		static const int amount;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private B::C::amount <7:20 7:25>");
	}

	void test_cxx_parser_finds_function_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	int sum(int a, int b);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int anonymous namespace (input.cc)::sum(int, int) <3:6 3:8>");
	}

	void test_cxx_parser_finds_method_declared_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"	class C\n"
			"	{\n"
			"		bool isGreat() const;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "private bool B::C::isGreat() const <5:8 5:14>");
	}

	void test_cxx_parser_finds_nested_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"	namespace B\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->namespaces.size(), 2);
		TS_ASSERT_EQUALS(client->namespaces[0], "A <1:1 <1:11 1:11> 6:1>");
		TS_ASSERT_EQUALS(client->namespaces[1], "A::B <3:2 <3:12 3:12> 5:2>");
	}

	void test_cxx_parser_finds_enum_defined_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "public B::Z <4:2 <4:7 4:7> 6:2>");
	}

	void test_cxx_parser_finds_enum_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n\n"
			"{\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "n::Z <3:2 <3:7 3:7> 5:2>");
	}

	void test_cxx_parser_finds_enum_definition_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "private A<typename T>::TestType <4:2 <4:7 4:14> 8:2>");
	}

	void test_cxx_parser_finds_enum_usage_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"	TestType foo;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<typename T>::foo -> A<typename T>::TestType <9:2 9:9>");
	}

	void test_cxx_parser_finds_enum_constants_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enumConstants.size(), 2);
		TS_ASSERT_EQUALS(client->enumConstants[0], "A<typename T>::TestType::TEST_ONE <6:3 6:10>");
		TS_ASSERT_EQUALS(client->enumConstants[1], "A<typename T>::TestType::TEST_TWO <7:3 7:10>");
	}

	void test_cxx_parser_finds_correct_field_member_type_of_nested_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	class B\n"
			"	{\n"
			"		T foo;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<typename T>::B::foo -> A<typename T>::T <6:3 6:3>");
	}




///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

	void test_cxx_parser_finds_type_usage_of_global_variable()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "x -> int <1:1 1:3>");
	}

	void test_cxx_parser_finds_typedefs_type_use()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "uint -> unsigned int <1:9 1:16>");
	}

	void test_cxx_parser_finds_typedef_that_uses_type_defined_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace test\n"
			"{\n"
			"	struct TestStruct{};\n"
			"}\n"
			"typedef test::TestStruct globalTestStruct;\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "globalTestStruct -> test::TestStruct <5:15 5:24>");
	}

	void test_cxx_parser_finds_type_use_of_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
			"uint number;\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[1], "number -> uint <2:1 2:4>");
	}

	void test_cxx_parser_finds_class_default_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : private A <2:11 2:11>");
	}

	void test_cxx_parser_finds_class_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : public A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : public A <2:18 2:18>");
	}

	void test_cxx_parser_finds_class_protected_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : protected A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : protected A <2:21 2:21>");
	}

	void test_cxx_parser_finds_class_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : private A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : private A <2:19 2:19>");
	}

	void test_cxx_parser_finds_class_multiple_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B {};\n"
			"class C\n"
			"	: public A\n"
			"	, private B\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 2);
		TS_ASSERT_EQUALS(client->inheritances[0], "C : public A <4:11 4:11>");
		TS_ASSERT_EQUALS(client->inheritances[1], "C : private B <5:12 5:12>");
	}

	void test_cxx_parser_finds_struct_default_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : public A <2:12 2:12>");
	}

	void test_cxx_parser_finds_struct_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : public A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : public A <2:19 2:19>");
	}

	void test_cxx_parser_finds_struct_protected_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : protected A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : protected A <2:22 2:22>");
	}

	void test_cxx_parser_finds_struct_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : private A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : private A <2:20 2:20>");
	}

	void test_cxx_parser_finds_struct_multiple_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B {};\n"
			"struct C\n"
			"	: public A\n"
			"	, private B\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 2);
		TS_ASSERT_EQUALS(client->inheritances[0], "C : public A <4:11 4:11>");
		TS_ASSERT_EQUALS(client->inheritances[1], "C : private B <5:12 5:12>");
	}

	void test_cxx_parser_finds_method_override_when_virtual()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 1);
		TS_ASSERT_EQUALS(client->overrides[0], "void A::foo() -> void B::foo() <5:7 5:9>");
	}

	void test_cxx_parser_finds_multi_layer_method_overrides()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};\n"
			"class C : public B {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 2);
		TS_ASSERT_EQUALS(client->overrides[0], "void A::foo() -> void B::foo() <5:7 5:9>");
		TS_ASSERT_EQUALS(client->overrides[1], "void B::foo() -> void C::foo() <8:7 8:9>");
	}

	void test_cxx_parser_finds_method_overrides_on_different_return_types()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	int foo();\n"
			"};\n",
			false
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 1);
		TS_ASSERT_EQUALS(client->overrides[0], "void A::foo() -> int B::foo() <5:6 5:8>");
		TS_ASSERT_EQUALS(client->errors.size(), 1);
	}

	void test_cxx_parser_finds_no_method_override_when_not_virtual()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 0);
	}

	void test_cxx_parser_finds_no_method_overrides_on_different_signatures()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo(int a);\n"
			"};\n"
			"class B : public A {\n"
			"	int foo(int a, int b);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 0);
	}

	void test_cxx_parser_finds_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	sum(1, 2);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> int sum(int, int) <7:2 7:4>");
	}

	void test_cxx_parser_finds_call_in_function_with_correct_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"void func()\n"
			"{\n"
			"}\n"
			"void func(bool right)\n"
			"{\n"
			"	sum(1, 2);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "void func(bool) -> int sum(int, int) <10:2 10:4>");
	}

	void test_cxx_parser_finds_call_to_function_with_right_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"float sum(float a, float b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	sum(1, 2);\n"
			"	sum(1.0f, 0.5f);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> int sum(int, int) <11:2 11:4>");
		TS_ASSERT_EQUALS(client->calls[1], "int main() -> float sum(float, float) <12:2 12:4>");
	}

	void test_cxx_parser_finds_function_call_in_function_parameter_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	return sum(1, sum(2, 3));\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> int sum(int, int) <7:9 7:11>");
		TS_ASSERT_EQUALS(client->calls[1], "int main() -> int sum(int, int) <7:16 7:18>");
	}

	void test_cxx_parser_finds_function_call_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"class App\n"
			"{\n"
			"	int main()\n"
			"	{\n"
			"		return sum(1, 2);\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "int App::main() -> int sum(int, int) <9:10 9:12>");
	}

	void test_cxx_parser_finds_implicit_constructor_without_definition_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> void App::App() <6:6 6:8>");
	}

	void test_cxx_parser_finds_explicit_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> void App::App() <8:2 8:4>");
	}

	void test_cxx_parser_finds_explicit_constructor_call_of_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Item\n"
			"{\n"
			"};\n"
			"class App\n"
			"{\n"
			"public:\n"
			"	App() : item() {}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "void App::App() -> void Item::Item() <7:10 7:13>");
	}

	void test_cxx_parser_finds_function_call_in_member_initialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int one() { return 1; }\n"
			"class Item\n"
			"{\n"
			"public:\n"
			"	Item(int n) {}\n"
			"};\n"
			"class App\n"
			"{\n"
			"	App()\n"
			"		: item(one())"
			"	{}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "void App::App() -> void Item::Item(int) <10:5 10:8>");
		TS_ASSERT_EQUALS(client->calls[1], "void App::App() -> int one() <10:10 10:12>");
	}

	void test_cxx_parser_finds_copy_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"	App(const App& other) {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"	App app2(app);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> void App::App() <9:6 9:8>");
		TS_ASSERT_EQUALS(client->calls[1], "int main() -> void App::App(App const &) <10:6 10:9>");
	}

	void test_cxx_parser_finds_global_variable_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"App app;\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "app -> void App::App() <6:5 6:7>");
	}

	void test_cxx_parser_finds_global_variable_function_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int one() { return 1; }\n"
			"int a = one();\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "a -> int one() <2:9 2:11>");
	}

	void test_cxx_parser_finds_operator_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	void operator+(int a)\n"
			"	{\n"
			"	}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"	app + 2;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "int main() -> void App::App() <10:6 10:8>");
		TS_ASSERT_EQUALS(client->calls[1], "int main() -> void App::operator+(int) <11:6 11:6>");
	}

	void test_cxx_parser_finds_usage_of_function_pointer()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void my_int_func(int x)\n"
			"{\n"
			"}\n"
			"\n"
			"void test()\n"
			"{\n"
			"	void (*foo)(int);\n"
			"	foo = &my_int_func;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "void test() -> my_int_func <8:9 8:19>");
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int bar;\n"
			"\n"
			"int main()\n"
			"{\n"
			"	bar = 1;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "int main() -> bar <5:2 5:4>");
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_global_variable_initialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int a = 0;\n"
			"int b[] = {a};\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 2); // FIXME: this usage is recorded twice (by InitList and ImplicitCast)
		TS_ASSERT_EQUALS(client->usages[0], "b -> a <2:12 2:12>");
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int bar;\n"
			"\n"
			"class App\n"
			"{\n"
			"	void foo()\n"
			"	{\n"
			"		bar = 1;\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "void App::foo() -> bar <7:3 7:5>");
	}

	void test_cxx_parser_finds_usage_of_field_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"	void foo()\n"
			"	{\n"
			"		bar = 1;\n"
			"		this->bar = 2;\n"
			"	}\n"
			"	int bar;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 2);
		TS_ASSERT_EQUALS(client->usages[0], "void App::foo() -> App::bar <5:3 5:5>");
		TS_ASSERT_EQUALS(client->usages[1], "void App::foo() -> App::bar <6:9 6:11>");
	}

	void test_cxx_parser_finds_usage_of_field_in_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"	App()\n"
			"		: bar(42)\n"
			"	{}\n"
			"	int bar;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "void App::App() -> App::bar <4:5 4:7>");
	}

	void test_cxx_parser_finds_return_type_use_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"double PI()\n"
			"{\n"
			"	return 3.14159265359;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "double PI() -> double <1:1 1:6>");
	}

	void test_cxx_parser_finds_parameter_type_uses_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void ceil(float a)\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "void ceil(float) -> void <1:1 1:4>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void ceil(float) -> float <1:11 1:15>");
	}

	void test_cxx_parser_finds_parameter_type_uses_in_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	A(int a);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "void A::A(int) -> int <3:4 3:6>");
	}

	void test_cxx_parser_finds_type_uses_in_function_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int main()\n"
			"{\n"
			"	int a = 42;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "int main() -> int <1:1 1:3>");
		TS_ASSERT_EQUALS(client->typeUses[1], "int main() -> int <3:2 3:4>");
	}

	void test_cxx_parser_finds_type_uses_in_method_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	int main()\n"
			"	{\n"
			"		int a = 42;\n"
			"		return a;\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "int A::main() -> int <3:2 3:4>");
		TS_ASSERT_EQUALS(client->typeUses[1], "int A::main() -> int <5:3 5:5>");
	}

	void test_cxx_parser_finds_type_uses_in_loops_and_conditions()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int main()\n"
			"{\n"
			"	if (true)\n"
			"	{\n"
			"		int a = 42;\n"
			"	}\n"
			"	for (int i = 0; i < 10; i++)\n"
			"	{\n"
			"		int b = i * 2;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 4);
		TS_ASSERT_EQUALS(client->typeUses[0], "int main() -> int <1:1 1:3>");
		TS_ASSERT_EQUALS(client->typeUses[1], "int main() -> int <5:3 5:5>");
		TS_ASSERT_EQUALS(client->typeUses[2], "int main() -> int <7:7 7:9>");
		TS_ASSERT_EQUALS(client->typeUses[3], "int main() -> int <9:3 9:5>");
	}

	void test_cxx_parser_finds_type_uses_of_base_class_in_derived_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	A(int n) {}"
			"};\n"
			"class B : public A\n"
			"{\n"
			"public:\n"
			"	B() : A(42) {}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "void A::A(int) -> int <4:4 4:6>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void B::B() -> A <8:8 8:8>");
	}

	void test_cxx_parser_finds_enum_uses_in_global_space()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum A\n"
			"{\n"
			"	B,\n"
			"	C\n"
			"};\n"
			"A a = B;\n"
			"A* aPtr = new A;\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "a -> A::B <6:7 6:7>");
		TS_ASSERT_EQUALS(client->typeUses.size(), 3);
		TS_ASSERT_EQUALS(client->typeUses[0], "a -> A <6:1 6:1>");
		TS_ASSERT_EQUALS(client->typeUses[1], "aPtr -> A <7:1 7:1>");
		TS_ASSERT_EQUALS(client->typeUses[2], "aPtr -> A <7:15 7:15>");
		TS_ASSERT_EQUALS(client->globalVariables.size(), 2);
		TS_ASSERT_EQUALS(client->globalVariables[0], "a <6:3 6:3>");
		TS_ASSERT_EQUALS(client->globalVariables[1], "aPtr <7:4 7:7>");
	}

	void test_cxx_parser_finds_enum_uses_in_function_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum A\n"
			"{\n"
			"	B,\n"
			"	C\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A a = B;\n"
			"	A* aPtr = new A;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "int main() -> A::B <8:8 8:8>");
		TS_ASSERT_EQUALS(client->typeUses.size(), 4);
		TS_ASSERT_EQUALS(client->typeUses[0], "int main() -> int <6:1 6:3>");
		TS_ASSERT_EQUALS(client->typeUses[1], "int main() -> A <8:2 8:2>");
		TS_ASSERT_EQUALS(client->typeUses[2], "int main() -> A <9:2 9:2>");
		TS_ASSERT_EQUALS(client->typeUses[3], "int main() -> A <9:16 9:16>");
	}

	void test_cxx_parser_finds_macro_expand()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI 3.14159265359\n"
			"void test()\n"
			"{\n"
			"double i = PI;"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->macroUses.size(), 1);
		TS_ASSERT_EQUALS(client->macroUses[0], "PI <4:12 4:13>");
	}

	void test_cxx_parser_finds_usage_of_template_template_parameter_of_template_class_specialized_with_concrete_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{\n"
			"	void foo(T<int> parameter)\n"
			"	{}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "void B<template<typename> typename T>::foo(B<template<typename> typename T>::T<int>) -> void <7:2 7:5>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void B<template<typename> typename T>::foo(B<template<typename> typename T>::T<int>) -> B<template<typename> typename T>::T<int> <7:11 7:11>");
	}

	void test_cxx_parser_finds_usage_of_template_template_parameter_of_template_class_specialized_with_template_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{\n"
			"	template <typename U> \n"
			"	void foo(T<U> parameter)\n"
			"	{}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "void B<template<typename> typename T>::foo<typename U>(B<template<typename> typename T>::T<U>) -> void <8:2 8:5>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void B<template<typename> typename T>::foo<typename U>(B<template<typename> typename T>::T<U>) -> B<template<typename> typename T>::T<B<template<typename> typename T>::foo<typename U>::U> <8:11 8:11>");
	}

	void test_cxx_parser_finds_typedef_in_other_class_that_depends_on_own_template_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	typedef T type;\n"
			"};\n"
			"template <typename U>\n"
			"class B\n"
			"{\n"
			"public:\n"
			"	typedef typename A<U>::type type;\n"
			"};\n"
			"B<int>::type f = 0;\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 4);
		TS_ASSERT_EQUALS(client->typedefs[0], "public A<typename T>::type <5:12 5:15>");
		TS_ASSERT_EQUALS(client->typedefs[1], "public A<int>::type <5:12 5:15>");
		TS_ASSERT_EQUALS(client->typedefs[2], "public B<typename U>::type <11:30 11:33>");
		TS_ASSERT_EQUALS(client->typedefs[3], "public B<int>::type <11:30 11:33>");

		TS_ASSERT_EQUALS(client->typeUses.size(), 4);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<typename T>::type -> A<typename T>::T <5:10 5:10>");
		TS_ASSERT_EQUALS(client->typeUses[1], "B<typename U>::type -> A<B<typename U>::U>::type <11:25 11:28>");
		TS_ASSERT_EQUALS(client->typeUses[2], "B<int>::type -> A<int>::type <11:25 11:28>");
		TS_ASSERT_EQUALS(client->typeUses[3], "f -> B<int>::type <13:9 13:12>");
		//TS_ASSERT_EQUALS(client->typeUses[3], "A<int>::type -> int <13:9 13:12>"); TODO: make this work!
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<int>->int <7:4 7:6>");
	}
	void test_cxx_parser_finds_type_template_argument_for_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"   A<int, float>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<<int, float>>->int <7:6 7:8>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<<int, float>>->float <7:11 7:15>");
	}

	void test_cxx_parser_finds_type_template_argument_in_non_default_constructor_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(int data){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int>(5);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<int>->int <9:4 9:6>");
	}

	void test_cxx_parser_finds_type_template_argument_in_default_constructor_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<int>->int <9:4 9:6>");
	}

	void test_cxx_parser_finds_type_template_argument_in_new_expression_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	new A<int>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<int>->int <9:8 9:10>");
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<1> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<true> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_argument_of_implicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"int main()\n"
			"{\n"
			"	A<&g_p> a;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p>->g_p <9:5 9:7>");
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_argument_of_implicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"int main()\n"
			"{\n"
			"	A<g_p> a;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p>->g_p <9:4 9:6>");
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int... T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"   A<1, 2, 33>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_argument_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"int main()\n"
			"{\n"
			"	B<A> ba;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "B<A>->A<typename T> <9:4 9:4>");
	}

	void test_cxx_parser_finds_template_template_argument_for_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <template<typename> typename... T>\n"
			"class B\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	B<A, A>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "B<<A, A>>->A<typename T> <11:4 11:4>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "B<<A, A>>->A<typename T> <11:7 11:7>");
	}

	void test_cxx_parser_finds_template_member_specialization_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	T foo() {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateMemberSpecializations.size(), 1);
		TS_ASSERT_EQUALS(client->templateMemberSpecializations[0], "int A<int>::foo() -> A<typename T>::T A<typename T>::foo() <5:4 5:6>");
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<int>->int <6:9 6:11>");
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<1>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<true>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <>\n"
			"class A<&g_p>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p>->g_p <8:10 8:12>");
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <>\n"
			"class A<g_p>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p>->g_p <8:9 8:11>");
	}

	void test_cxx_parser_finds_template_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"template <>\n"
			"class B<A>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "B<A>->A<typename T> <8:9 8:9>");
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<typename T, int>->A<typename T, int>::T <6:9 6:9>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<typename T, int>->int <6:12 6:14>");
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T, int U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <int U>\n"
			"class A<3, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<3, int U>->A<3, int U>::U <6:12 6:12>");
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T, bool U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <bool U>\n"
			"class A<true, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<true, bool U>->A<true, bool U>::U <6:15 6:15>");
	}

	void test_cxx_parser_finds_template_argument_for_non_type_custom_pointer_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p, P* q>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <P* q>\n"
			"class A<&g_p, q>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p, P * q>->g_p <8:10 8:12>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<&g_p, P * q>->A<&g_p, P * q>::q <8:15 8:15>");
	}

	void test_cxx_parser_finds_template_argument_for_non_type_custom_reference_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p, P& q>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <P& q>\n"
			"class A<g_p, q>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<&g_p, P & q>->g_p <8:9 8:11>"); // why reference here?
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<&g_p, P & q>->A<&g_p, P & q>::q <8:14 8:14>");
	}

	void test_cxx_parser_finds_template_argument_for_template_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T, template<typename> class U>\n"
			"class B\n"
			"{};\n"
			"template <template<typename> class U>\n"
			"class B<A, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "B<A, template<typename> typename U>->A<typename T> <8:9 8:9>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "B<A, template<typename> typename U>->B<A, template<typename> typename U>::U<typename> <8:12 8:12>");
	}

	void test_cxx_parser_finds_non_type_template_argument_that_depends_on_type_template_parameter_of_explicit_partial_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T1, typename T2, T2 T3>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T2, T2 T3>\n"
			"class A<3, T2, T3>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<3, typename T2, T2 T3>->A<3, typename T2, T2 T3>::T2 <6:12 6:13>");
		TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<3, typename T2, T2 T3>->A<3, typename T2, T2 T3>::T3 <6:16 6:17>");
	}

	//void _test_cxx_parser_finds_non_type_template_argument_that_depends_on_template_template_parameter_of_explicit_partial_template_specialization()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"template <int T1, template<typename> class T2, T2<int> T3>\n"
	//		"class A\n"
	//		"{\n"
	//		"};\n"
	//		"template <template<typename> class T2, T2<int> T3>\n"
	//		"class A<3, T2, T3>\n"
	//		"{\n"
	//		"};\n"
	//	);

	//	TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<3, template<typename> typename T2, T2<int> T3>->A<3, template<typename> typename T2, T2<int> T3>::T2<typename> <6:12 6:13>");
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[1], "A<3, template<typename> typename T2, T2<int> T3>->A<3, template<typename> typename T2, T2<int> T3>::T3 <6:16 6:17>");
	//}

	void test_cxx_parser_finds_implicit_template_class_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"A<int> a;\n"
		);

		TS_ASSERT_EQUALS(client->templateSpecializations.size(), 1);
		TS_ASSERT_EQUALS(client->templateSpecializations[0], "A<int> -> A<typename T> <2:7 2:7>");
	}

	void test_cxx_parser_finds_class_inheritance_from_implicit_template_class_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"class B: public A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : public A<int> <7:17 7:17>");
	}

	void test_cxx_parser_finds_template_class_specialization_with_template_argument()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"template <typename U>\n"
			"class B: public A<U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<B<typename U>::U>->B<typename U>::U <8:19 8:19>");
	}

	void test_cxx_parser_finds_template_class_constructor_usage_of_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	A(): foo() {}\n"
			"	T foo;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 1);
		TS_ASSERT_EQUALS(client->usages[0], "void A<typename T>::A<T>() -> A<typename T>::foo <4:7 4:9>");
	}

	void test_cxx_parser_finds_correct_method_return_type_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<typename T>::T A<typename T>::foo() -> A<typename T>::T <4:2 4:2>");
	}

	void test_cxx_parser_finds_type_template_default_argument_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T = int>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes[0], "int -> A<typename T>::T <1:24 1:26>");
	}

	void test_cxx_parser_finds_no_default_argument_type_for_non_type_bool_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T = true>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_default_argument_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T = A>\n"
			"class B\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes[0], "A<typename T> -> B<template<typename> typename T>::T<typename> <4:40 4:40>");
	}

	void test_cxx_parser_finds_implicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	return test(1);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateSpecializations.size(), 1);
		TS_ASSERT_EQUALS(client->templateSpecializations[0], "int test<int>(int) -> T test<typename T>(T) <2:3 2:6>");
	}

	void test_cxx_parser_finds_explicit_specialization_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"template <>\n"
			"int test<int>(int a)\n"
			"{\n"
			"	return a + a;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateSpecializations.size(), 1);
		TS_ASSERT_EQUALS(client->templateSpecializations[0], "int test<int>(int) -> T test<typename T>(T) <8:5 8:8>");
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_explicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test()\n"
			"{\n"
			"};\n"
			"\n"
			"template <>\n"
			"void test<int>()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "void test<int>()->int <7:11 7:13>");
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test(){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test<int>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "void test<int>()->int <6:7 6:9>");
	}

	void test_cxx_parser_finds_no_explicit_non_type_int_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"void test(){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test<33>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_explicit_template_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A {};\n"
			"template <template<typename> class T>\n"
			"void test(){};\n"
			"int main()\n"
			"{\n"
			"	test<A>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "void test<A>()->A<typename T> <7:7 7:7>");
	}

	void test_cxx_parser_finds_no_implicit_type_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test(T data){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test(1);\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_function_call_in_var_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(){ return 1; }\n"
			"\n"
			"class A\n"
			"{\n"
			"	int foo = test<int>();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "int test<int>()->int <6:17 6:19>");
	}

	void test_cxx_parser_finds_no_implicit_type_template_argument_of_function_call_in_var_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T i){ return i; }\n"
			"\n"
			"class A\n"
			"{\n"
			"	int foo = test(1);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_type_template_default_argument_type_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T = int>\n"
			"void test()\n"
			"{\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes[0], "int -> test<typename T>::T <1:24 1:26>");
	}

	void test_cxx_parser_does_not_find_default_argument_type_for_non_type_bool_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T = true>\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_default_argument_type_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T = A>\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 1);
		TS_ASSERT_EQUALS(
			client->templateDefaultArgumentTypes[0],
			"A<typename T> -> test<template<typename> typename T>::T<typename> <4:40 4:40>");
	}

	void test_cxx_parser_finds_lambda_calling_a_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void func() {}\n"
			"void lambdaCaller()\n"
			"{\n"
			"	[]()\n"
			"	{\n"
			"		func();\n"
			"	}();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "void lambdaCaller() -> void lambdaCaller::lambda at 4:2() <7:3 7:3>");
		TS_ASSERT_EQUALS(client->calls[1], "void lambdaCaller::lambda at 4:2() -> void func() <6:3 6:6>");
	}

	void test_cxx_parser_finds_template_argument_of_unresolved_lookup_expression_as_type_use()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void a()\n"
			"{\n"
			"}\n"
			"\n"
			"template<typename MessageType>\n"
			"void dispatch()\n"
			"{\n"
			"	a<MessageType>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 3);
		TS_ASSERT_EQUALS(client->typeUses[0], "void a<typename T>() -> void <2:1 2:4>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void dispatch<typename MessageType>() -> void <7:1 7:4>");
		TS_ASSERT_EQUALS(client->typeUses[2], "void dispatch<typename MessageType>() -> dispatch<typename MessageType>::MessageType <9:4 9:14>");
	}

	///////////////////////////////////////////////////////////////////////////////
	// test finding symbol locations

	void test_cxx_parser_finds_correct_location_of_explicit_constructor_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n\n"
			"{\n"
			"	class App\n"
			"	{\n"
			"	public:\n"
			"		App(int i) {}\n"
			"	};\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	n::App a = n::App(2);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[1], "int main() -> void n::App::App(int) <11:16 11:18>");
	}

	void test_cxx_parser_finds_macro_define_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define MAX(a,b) \\\n"
			"	((a)>(b)?(a):(b))"
		);

		TS_ASSERT_EQUALS(client->macros.size(), 1);
		TS_ASSERT_EQUALS(client->macros[0], "MAX <1:9 <1:9 1:11> 2:17>");
	}

	//void __test_cxx_parser_finds_type_template_argument_of_static_cast_expression()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"int main()\n"
	//		"{\n"
	//		"	return static_cast<int>(4.0f);"
	//		"}\n"
	//	);

	//	TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[0], "A<1>->int <0:0 0:0>");
	//}

	//void _test_cxx_parser_finds_implicit_constructor_call_in_initialization()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"class A\n"
	//		"{\n"
	//		"};\n"
	//		"class B\n"
	//		"{\n"
	//		"	B(){}\n"
	//		"	A m_a;\n"
	//		"};\n"
	//	);

	//	TS_ASSERT_EQUALS(client->calls.size(), 1);
	//	TS_ASSERT_EQUALS(client->calls[0], "void B::B() -> A::A() <6:2 6:2>");
	//}

	void test_cxx_parser_parses_multiple_files()
	{
		TestFileManager fm;
		TestParserClient client;
		CxxParser parser(&client, &fm);

		std::vector<FilePath> filePaths;
		filePaths.push_back(FilePath("data/CxxParserTestSuite/header.h"));
		filePaths.push_back(FilePath("data/CxxParserTestSuite/code.cpp"));

		Parser::Arguments args;
		args.language = "c++";
		args.languageStandard = "1z";

		parser.parseFiles(filePaths, args);

		TS_ASSERT_EQUALS(client.errors.size(), 0);

		TS_ASSERT_EQUALS(client.typedefs.size(), 1);
		TS_ASSERT_EQUALS(client.classes.size(), 4);
		TS_ASSERT_EQUALS(client.enums.size(), 1);
		TS_ASSERT_EQUALS(client.enumConstants.size(), 2);
		TS_ASSERT_EQUALS(client.functions.size(), 2);
		TS_ASSERT_EQUALS(client.fields.size(), 4);
		TS_ASSERT_EQUALS(client.globalVariables.size(), 2);
		TS_ASSERT_EQUALS(client.methods.size(), 15);
		TS_ASSERT_EQUALS(client.namespaces.size(), 2);
		TS_ASSERT_EQUALS(client.structs.size(), 1);

		TS_ASSERT_EQUALS(client.inheritances.size(), 1);
		TS_ASSERT_EQUALS(client.calls.size(), 3);
		TS_ASSERT_EQUALS(client.usages.size(), 3);
		TS_ASSERT_EQUALS(client.typeUses.size(), 17);

		TS_ASSERT_EQUALS(client.files.size(), 3);
		TS_ASSERT_EQUALS(client.includes.size(), 1);
	}

	void test_cxx_parser_catches_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int a = b;\n",
			false
		);

		TS_ASSERT_EQUALS(client->errors.size(), 1);
		TS_ASSERT_EQUALS(client->errors[0], "use of undeclared identifier \'b\' <1:9 1:9>");
	}

	void test_cxx_parser_finds_location_of_line_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"// this is a line comment\n"
		);

		TS_ASSERT_EQUALS(client->comments.size(), 1);
		TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 1:26>");
	}

	void test_cxx_parser_finds_location_of_block_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"/* this is a\n"
			"block comment */\n"
		);

		TS_ASSERT_EQUALS(client->comments.size(), 1);
		TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 2:17>");
	}

	// void _test_TEST()
	// {
	// 	std::shared_ptr<TestParserClient> client = parseCode(
	// 		"template <template<template<typename> class> class T>\n"
	// 		"class A {\n"
	// 		"T<>\n"
	// 		"};\n"
	// 		"template <template<typename> class T>\n"
	// 		"class B {};\n"
	// 		"template <typename T>\n"
	// 		"class C {};\n"
	// 		"A<B> a;\n"
	// 	);
	// 	int ofo = 0;
	// }

private:
	class TestParserClient: public ParserClient
	{
	public:
		virtual void startParsing()
		{
		}

		virtual void finishParsing()
		{
		}

		virtual void startParsingFile(const FilePath& filePath)
		{
		}

		virtual void finishParsingFile(const FilePath& filePath)
		{
		}

		virtual void onError(const ParseLocation& location, const std::string& message, bool fatal)
		{
			errors.push_back(addLocationSuffix(message, location));
		}

		virtual Id onTypedefParsed(
			const ParseLocation& location, const NameHierarchy& typedefName, AccessType access, bool isImplicit
		)
		{
			std::string str = addAccessPrefix(typedefName.getQualifiedName(), access);
			typedefs.push_back(addLocationSuffix(str, location));
			return 0;
		}

		virtual Id onClassParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			classes.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
			return 0;
		}

		virtual Id onStructParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			structs.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
			return 0;
		}

		virtual Id onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit)
		{
			globalVariables.push_back(addLocationSuffix(variable.getQualifiedName(), location));
			return 0;
		}

		virtual Id onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessType access, bool isImplicit)
		{
			fields.push_back(addLocationSuffix(addAccessPrefix(field.getQualifiedName(), access), location));
			return 0;
		}

		virtual Id onFunctionParsed(
			const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit)
		{
			functions.push_back(addLocationSuffix(function.getQualifiedNameWithSignature(), location, scopeLocation));
			return 0;
		}

		virtual Id onMethodParsed(
			const ParseLocation& location, const NameHierarchy& method, AccessType access, AbstractionType abstraction,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			std::string str = method.getQualifiedNameWithSignature();
			str = addAbstractionPrefix(str, abstraction);
			str = addAccessPrefix(str, access);
			str = addLocationSuffix(str, location, scopeLocation);
			methods.push_back(str);
			return 0;
		}

		virtual Id onNamespaceParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation, bool isImplicit)
		{
			namespaces.push_back(addLocationSuffix(nameHierarchy.getQualifiedName(), location, scopeLocation));
			return 0;
		}

		virtual Id onEnumParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			enums.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
			return 0;
		}

		virtual Id onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit)
		{
			enumConstants.push_back(addLocationSuffix(nameHierarchy.getQualifiedName(), location));
			return 0;
		}

		virtual Id onTemplateParameterTypeParsed(
			const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit)
		{
			templateParameterTypes.push_back(
				addLocationSuffix(templateParameterTypeNameHierarchy.getQualifiedName(), location)
				);
			return 0;
		}

		virtual Id onInheritanceParsed(
			const ParseLocation& location, const NameHierarchy& childNameHierarchy,
			const NameHierarchy& parentNameHierarchy, AccessType access)
		{
			std::string str = childNameHierarchy.getQualifiedName() + " : " + addAccessPrefix(parentNameHierarchy.getQualifiedName(), access);
			inheritances.push_back(addLocationSuffix(str, location));
			return 0;
		}

		virtual Id onMethodOverrideParsed(
			const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider)
		{
			overrides.push_back(addLocationSuffix(overridden.getQualifiedNameWithSignature() + " -> " + overrider.getQualifiedNameWithSignature(), location));
			return 0;
		}

		virtual Id onCallParsed(
			const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee)
		{
			std::string callerSignature = caller.getQualifiedNameWithSignature();
			calls.push_back(addLocationSuffix(caller.getQualifiedNameWithSignature() + " -> " + callee.getQualifiedNameWithSignature(), location));
			return 0;
		}

		virtual Id onUsageParsed(
			const ParseLocation& location, const NameHierarchy& userName, int usedType, const NameHierarchy& usedName)
		{
			usages.push_back(addLocationSuffix(userName.getQualifiedNameWithSignature() + " -> " + usedName.getQualifiedName(), location));
			return 0;
		}

		virtual Id onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used)
		{
			typeUses.push_back(addLocationSuffix(user.getQualifiedNameWithSignature() + " -> " + used.getQualifiedNameWithSignature(), location));
			return 0;
		}

		virtual Id onTemplateArgumentTypeParsed(
			const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
			const NameHierarchy& templateNameHierarchy)
		{
			templateArgumentTypes.push_back(
				addLocationSuffix(templateNameHierarchy.getQualifiedNameWithSignature() + "->" + argumentTypeNameHierarchy.getQualifiedName(), location)
			);
			return 0;
		}

		virtual Id onTemplateDefaultArgumentTypeParsed(
			const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
			const NameHierarchy& templateParameterNameHierarchy)
		{
			templateDefaultArgumentTypes.push_back(
				addLocationSuffix(defaultArgumentTypeNameHierarchy.getQualifiedNameWithSignature() + " -> " + templateParameterNameHierarchy.getQualifiedName(), location)
			);
			return 0;
		}

		virtual Id onTemplateSpecializationParsed(
			const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
			const NameHierarchy& specializedFromNameHierarchy)
		{
			templateSpecializations.push_back(addLocationSuffix(
				specializedNameHierarchy.getQualifiedNameWithSignature() + " -> " + specializedFromNameHierarchy.getQualifiedNameWithSignature(), location
			));
			return 0;
		}

		virtual Id onTemplateMemberFunctionSpecializationParsed(
			const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction)
		{
			templateMemberSpecializations.push_back(addLocationSuffix(
				instantiatedFunction.getQualifiedNameWithSignature() + " -> " + specializedFunction.getQualifiedNameWithSignature(), location
			));
			return 0;
		}

		virtual Id onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
		{
			localSymbols.push_back(name);
			return 0;
		}


		virtual Id onFileParsed(const FileInfo& fileInfo)
		{
			files.push_back(fileInfo.path.str());
			return 0;
		}

		virtual Id onFileIncludeParsed(
			const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo)
		{
			includes.push_back(fileInfo.path.str() + " -> " + includedFileInfo.path.str());
			return 0;
		}

		virtual Id onMacroDefineParsed(
			const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation)
		{
			macros.push_back(addLocationSuffix(macroNameHierarchy.getQualifiedName(), location, scopeLocation));
			return 0;
		}

		virtual Id onMacroExpandParsed(const ParseLocation& location, const NameHierarchy& macroNameHierarchy)
		{
			macroUses.push_back(addLocationSuffix(macroNameHierarchy.getQualifiedName() ,location));
			return 0;
		}

		virtual Id onCommentParsed(const ParseLocation& location)
		{
			comments.push_back(addLocationSuffix("comment", location));
			return 0;

		}

		std::vector<std::string> errors;

		std::vector<std::string> typedefs;
		std::vector<std::string> classes;
		std::vector<std::string> enums;
		std::vector<std::string> enumConstants;
		std::vector<std::string> functions;
		std::vector<std::string> fields;
		std::vector<std::string> globalVariables;
		std::vector<std::string> methods;
		std::vector<std::string> namespaces;
		std::vector<std::string> structs;
		std::vector<std::string> macros;

		std::vector<std::string> inheritances;
		std::vector<std::string> overrides;
		std::vector<std::string> calls;
		std::vector<std::string> usages;	// for variables
		std::vector<std::string> typeUses;	// for types
		std::vector<std::string> macroUses;
		std::vector<std::string> templateParameterTypes;
		std::vector<std::string> templateArgumentTypes;
		std::vector<std::string> templateDefaultArgumentTypes;
		std::vector<std::string> templateSpecializations;
		std::vector<std::string> templateMemberSpecializations;
		std::vector<std::string> localSymbols;

		std::vector<std::string> files;
		std::vector<std::string> includes;

		std::vector<std::string> comments;
	};

	std::shared_ptr<TestParserClient> parseCode(std::string code, bool logErrors = true)
	{
		if (!m_args.headerSearchPaths.size())
		{
			std::shared_ptr<ApplicationSettings> settings = ApplicationSettings::getInstance();
			settings->load("data/TestSettings.xml");
			m_args.headerSearchPaths = settings->getHeaderSearchPaths();
		}

		m_args.logErrors = logErrors;
		m_args.language = "c++";
		m_args.languageStandard = "1z";

		TestFileManager fm;
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client.get(), &fm);
		parser.parseFile("input.cc", TextAccess::createFromString(code), m_args);
		return client;
	}

	Parser::Arguments m_args;
};

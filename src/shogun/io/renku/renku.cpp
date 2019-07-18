#include <shogun/io/renku/renku.h>
#include <shogun/io/renku/RenkuSerializer.h>
#include <shogun/io/ShogunErrc.h>
#include <shogun/io/fs/FileSystem.h>
#include <shogun/io/fs/Path.h>
#include <shogun/io/stream/FileOutputStream.h>

#include <rapidjson/writer.h>

#include <string>
#include <string_view>
#include <sstream>

using namespace shogun;
using namespace shogun::renku;
using namespace std;

void shogun::renku::to_mls(const shared_ptr<Machine>& m, const shared_ptr<Features>& input_features)
{
	auto fs = env();
	std::error_condition ec;
	std::unique_ptr<io::WritableFile> file;
	string renku_project_root(".renku");
	if (const char* renku_root = std::getenv("RENKU_HOME"))
		renku_project_root = renku_root;

	if ((ec = fs->is_directory(renku_project_root)))
		throw io::to_system_error(ec);

	auto renku_mls_path = io::join_path(renku_project_root, "ml");
	if (fs->is_directory(renku_mls_path))
		fs->create_dir(renku_mls_path);

	auto run_dir = io::join_path(renku_mls_path, to_string(m->hash()));
	if (fs->is_directory(run_dir))
		fs->create_dir(run_dir);


	if ((ec = fs->new_writable_file(io::join_path(run_dir, "metadata.jsonld"), &file)))
		throw io::to_system_error(ec);


	auto fos = make_shared<io::FileOutputStream>(file.get());
	auto serializer = make_shared<RenkuSerializer>();
	serializer->attach(fos);
	serializer->write(m);

	// inputs

	// outputs
	/*
	for (const auto& e: evaluations)
	{
		auto eval_name = generate_key("sml", e.first->get_name();
		// specify eval metric
		writer.StartObject();
		writer.Key(kID.data());
		writer.String(eval_name.c_str());
		writer.Key(kType.data());
		writer.StartArray();
		writer.String(generate_key("owl", "NamedIndividual").c_str());
		writer.String(generate_key("mls", "EvaluationMeasure").c_str());
		writer.EndArray();
		writer.EndObject();

		writer.StartObject();
		writer.Key(kID.data());
		writer.String(generate_key("sml", e.first->get_name().c_str()));
		writer.Key(kType.data());
		writer.StartArray();
		writer.String(generate_key("owl", "NamedIndividual").c_str());
		writer.String(generate_key("mls", "ModelEvaluation").c_str());
		writer.EndArray();
		writer.Key(generate_key("mls", "hasValue").c_str());
		writer.StartObject();
		writer.Key(kType.data());
		writer.String(generate_key("xsd", "decimal").c_str());
		writer.Key(kValue.data());
		writer.String(e.second);
		writer.EndObject();
		writer.Key(generate_key("mls", "specifiedBy").c_str());
		writer.StartObject();
		writer.Key(kID.data());
		writer.String(eval_name.c_str());
		writer.EndObject();
		writer.EndObject();
	}
*/
	// model output
	/*
	writer.StartObject();
	stringstream ss;
	ss << object->get_name() << object->hash();
	writer.Key(kId.data());
	writer.String(generate_key("sml", ss.str()).c_str());
	writer.Key(kType.data());
	writer.StartArray();
	writer.String(generate_key("mls", "Model").c_str());
	writer.String(generate_key("owl", "NamedIndividual").c_str());
	writer.EndArray();
	writer.EndObject();
	*/

	// define run

}
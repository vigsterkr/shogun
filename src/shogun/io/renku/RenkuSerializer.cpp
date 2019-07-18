/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Viktor Gal
 */

#include <memory>
#include <stack>
#include <string_view>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <shogun/io/ShogunErrc.h>
#include <shogun/io/renku/RenkuSerializer.h>
#include <shogun/machine/Machine.h>
#include <shogun/util/converters.h>

#include <rapidjson/writer.h>

using namespace rapidjson;
using namespace shogun;
using namespace shogun::io;
using namespace shogun::renku;
using namespace std;

static constexpr string_view kContextKey("@context");
static unordered_map<string_view, string_view> kContext = {{
	{"mls", "http://www.w3.org/ns/mls#"},
    {"owl", "http://www.w3.org/2002/07/owl#"},
    {"rdfs", "http://www.w3.org/2000/01/rdf-schema#"},
    {"xsd", "http://www.w3.org/2001/XMLSchema#"},
    {"_id", "@id"},
    {"identifier", "schema:identifier"},
    {"executes", "mls:executes"},
    {"input_values", "mls:hasInput"},
    {"version", "dcterms:hasVersion"},
    {"name", "dcterms:title"},
    {"implements", "mls:implements"}
}};

static constexpr string_view kId("_id");
static constexpr string_view kType("@type");
static constexpr string_view kValue("@value");
static constexpr string_view kExecutes("executes");
static constexpr string_view kParameters("parameters");
static constexpr string_view kImplements("implements");
static constexpr string_view kInputValues("input_values");

template<typename Writer>
static void write_context(Writer& writer)
{
	writer.Key(kContextKey.data());
	writer.StartObject();
	for (const auto& kv: kContext)
	{
		writer.Key(kv.first.data());
		writer.String(kv.second.data());
	}
	writer.EndObject();
}

static const string generate_key(const string_view& context, const string_view& attr)
{
	stringstream ss;
	ss << context << ":" << attr;
	return ss.str();
}

struct OutputStreamAdapter
{
	typedef char Ch;
	void Put(Ch c)
	{
		if(auto ec = m_stream->write(&c, 1))
			throw io::to_system_error(ec);
	}

	void Flush()
	{
		m_stream->flush();
	}

	shared_ptr<OutputStream> m_stream;
};

template<typename Writer> void write_object(Writer& writer, const shared_ptr<SGObject>& object);

template<class Writer>
class MLSWriterVisitor : public AnyVisitor
{
	const int64_t in_object = -1;
public:
	MLSWriterVisitor(Writer& jw):
		AnyVisitor(), m_json_writer(jw) {}

	~MLSWriterVisitor() override {}

	void on(bool* v) override
	{
		SG_DEBUG("writing bool with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "boolean").c_str());
		m_json_writer.Key(kValue.data());
		string value = ((*v) == true) ? "true" : "false";
		m_json_writer.String(value.c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(std::vector<bool>::reference* v) override
	{
		SG_DEBUG("writing bool with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "boolean").c_str());
		m_json_writer.Key(kValue.data());
		string value = ((*v) == true) ? "true" : "false";
		m_json_writer.String(value.c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(char* v) override
	{
		SG_DEBUG("writing char with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "byte").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(int8_t* v) override
	{
		SG_DEBUG("writing int8_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "byte").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(uint8_t* v) override
	{
		SG_DEBUG("writing uint8_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "unsignedByte").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(int16_t* v) override
	{
		SG_DEBUG("writing int16_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "short").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(uint16_t* v) override
	{
		SG_DEBUG("writing uint16_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "unsignedShort").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(int32_t* v) override
	{
		SG_DEBUG("writing int32_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "int").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(uint32_t* v) override
	{
		SG_DEBUG("writing uint32_t with value %d\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "unsignedInt").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(int64_t* v) override
	{
		SG_DEBUG("writing int64_t with value %" PRId64 "\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "long").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(uint64_t* v) override
	{
		SG_DEBUG("writing uint64_t with value %" PRIu64 "\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "unsignedLong").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(float* v) override
	{
		SG_DEBUG("writing float with value %f\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "float").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(float64_t* v) override
	{
		SG_DEBUG("writing double with value %f\n", *v);
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "double").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(to_string(*v).c_str());
		m_json_writer.EndObject();
		close_container();
	}
	void on(floatmax_t* v) override
	{
	}
	void on(complex128_t* v) override
	{
	}
	void on(string* v) override
	{
		SG_DEBUG("writing string with value %s\n", v->c_str());
		m_json_writer.StartObject();
		m_json_writer.Key(kType.data());
		m_json_writer.String(generate_key("xsd", "string").c_str());
		m_json_writer.Key(kValue.data());
		m_json_writer.String(v->c_str());
		m_json_writer.EndObject();
	}
	void on(shared_ptr<SGObject>* v) override
	{
		if (*v)
		{
			SG_DEBUG("writing SGObject: %s\n", (*v)->get_name());
			object_to_mls(m_json_writer, this, *v);
		}
		else
		{
			// nullptr
			m_json_writer.Null();
		}
		close_container();
	}
	void enter_matrix(index_t* rows, index_t* cols) override
	{
	}
	void enter_vector(index_t* size) override
	{
		m_json_writer.StartObject();
		m_json_writer.Key("@list");
		m_json_writer.StartArray();
		if (*size == 0)
			m_json_writer.EndArray();
		else
			m_remaining.emplace(utils::safe_convert<int64_t>(*size), 0LL);

	}
	void enter_std_vector(size_t* size) override
	{
		m_json_writer.StartObject();
		m_json_writer.Key("@list");
		m_json_writer.StartArray();
		if (*size == 0)
			m_json_writer.EndArray();
		else
			m_remaining.emplace(utils::safe_convert<int64_t>(*size), 0LL);
	}
	void enter_map(size_t* size) override
	{
	}

	void start_object()
	{
		m_remaining.emplace(in_object, 0LL);
	}
	void end_object()
	{
		assert(get<0>(m_remaining.top()) == in_object);
		m_remaining.pop();
	}

	void enter_matrix_row(index_t *rows, index_t *cols) override {}
	void exit_matrix_row(index_t *rows, index_t *cols) override {}
	void exit_matrix(index_t* rows, index_t* cols) override {}
	void exit_vector(index_t* size) override
	{
		m_json_writer.EndObject();
	}
	void exit_std_vector(size_t* size) override
	{
		m_json_writer.EndObject();
	}
	void exit_map(size_t* size) override {}
private:
	inline void close_container()
	{
		if (m_remaining.empty() || get<0>(m_remaining.top()) == in_object)
			return;

		auto& remaining = get<0>(m_remaining.top());
		if (remaining > 0 && --remaining == 0)
		{
			m_remaining.pop();
			m_json_writer.EndArray();

			auto& cols_remaining = get<1>(m_remaining.top());
			if (cols_remaining > 0)
			{
				if (--cols_remaining == 0)
				{
					m_remaining.pop();
					m_json_writer.EndArray();
				}
				else
				{
					m_remaining.emplace(get<0>(m_remaining.top()), 0LL);
					m_json_writer.StartArray();
				}
			}
			else
			{
					close_container();
			}
		}
	}
private:
	Writer& m_json_writer;
	stack<tuple<int64_t, int64_t>> m_remaining;
	SG_DELETE_COPY_AND_ASSIGN(MLSWriterVisitor);
};


template<typename Writer>
void write_params(Writer& writer, MLSWriterVisitor<Writer>* visitor, const map<string, shared_ptr<const AnyParameter>>& params) noexcept(false)
{
	// set hyperparameters
	for (const auto& p: params)
	{
		if (p.second->get_value().visitable())
		{
			writer.StartObject();
			writer.Key(kId.data());
			auto setting_str = p.first + "Setting";
			writer.String(generate_key("_", setting_str).c_str());
			writer.Key(kType.data());
			writer.StartArray();
			writer.Key(generate_key("owl", "NamedIndividual").c_str());
			writer.Key(generate_key("mls", "HyperParameterSetting").c_str());
			writer.EndArray();

			// serialize value
			writer.Key(generate_key("mls", "hasValue").c_str());
			p.second->get_value().visit(visitor);

			// link specification
			writer.Key(generate_key("mls", "specifiedBy").c_str());
			writer.StartObject();
			writer.Key(kId.data());
			writer.String(generate_key("_", p.first).c_str());
			writer.EndObject();
			writer.EndObject();
		}
	}
}

template <typename Collection,typename Predicate>
Collection filter(Collection col, Predicate predicate)
{
	Collection filtered;
	for (auto const& kv: col)
		if (predicate(kv))
			filtered.emplace(kv.first, kv.second);
    return filtered;
}

template<typename Writer>
const string write_machine_type(Writer& writer, const shared_ptr<SGObject>& object)
{
	auto o = dynamic_pointer_cast<Machine>(object);
	string machine_type;
	if (o != nullptr)
	{
		switch (o->get_machine_problem_type())
		{
			case PT_BINARY:
				machine_type = "binaryClassifier";
				break;
			case PT_REGRESSION:
				machine_type = "regressor";
			case PT_CLASS:
			case PT_MULTICLASS:
				machine_type = "multiclassClassifier";
			default:
				return "";
		}

		writer.Key(kImplements.data());
		writer.StartObject();
		writer.Key(kId.data());
		writer.String(generate_key("_", machine_type).c_str());
		writer.Key(kType.data());
		writer.StartArray();
		writer.String(generate_key("mls", "Algorithm").c_str());
		writer.String(generate_key("owl", "NamedIndividual").c_str());
		writer.EndArray();
		writer.EndObject();

		return machine_type;
	}
	return "";
}


template<typename Writer>
void object_to_mls(Writer& writer, MLSWriterVisitor<Writer>* visitor, const shared_ptr<SGObject>& object) noexcept(false)
{
	pre_serialize(object);
	visitor->start_object();

	writer.Key(kExecutes.data());
	writer.StartObject();

	// write node type
	writer.Key(kType.data());
	writer.StartArray();
	writer.String(generate_key("mls", "Implementation").c_str());
	writer.String(generate_key("owl", "NamedIndividual").c_str());
	writer.EndArray();

	auto params = filter(object->get_params(), [] (const auto& kv) {
			auto p = kv.second->get_properties();
			return (p.has_property(ParameterProperties::HYPER) || p.has_property(ParameterProperties::SETTING));
		});

	// write parameters definitions
	writer.Key(kParameters.data());
	writer.StartArray();
	if (params.size())
	{
		writer.StartArray();
		for (const auto& p: params)
		{
			if (p.second->get_value().visitable())
			{
				writer.StartObject();
				writer.Key(kId.data());
				writer.String(generate_key("_", p.first).c_str());
				writer.Key(kType.data());
				writer.StartArray();
				writer.Key(generate_key("owl", "NamedIndividual").c_str());
				writer.Key(generate_key("mls", "HyperParameter").c_str());
				writer.EndArray();
				writer.EndObject();
			}
		}
		writer.EndArray();
	}
	writer.EndArray();

	// write machine implemention type
	write_machine_type(writer, object);

	writer.EndObject(); // end Executes

	// write params settings
	writer.Key(kInputValues.data());
	writer.StartArray();
	if (params.size())
		write_params(writer, visitor, params);
	writer.EndArray(); // end InputValues

	visitor->end_object();
	post_serialize(object);
}

using JsonWriter = Writer<OutputStreamAdapter, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag>;

RenkuSerializer::RenkuSerializer() : Serializer()
{
}

RenkuSerializer::~RenkuSerializer()
{
}

void RenkuSerializer::write(const shared_ptr<SGObject>& object) noexcept(false)
{
	OutputStreamAdapter adapter { stream() };
	JsonWriter writer(adapter);
	auto writer_visitor =
		make_unique<MLSWriterVisitor<JsonWriter>>(writer);
	writer.StartObject();
	write_context(writer);
	object_to_mls(writer, writer_visitor.get(), object);

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
	writer.EndObject();
}

#include <shogun/base/Parameter.h>
#include <shogun/base/SGObject.h>
#include <shogun/base/range.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/SGSparseMatrix.h>
#include <shogun/lib/SGSparseVector.h>
#include <shogun/lib/SGString.h>
#include <shogun/lib/SGVector.h>

namespace shogun
{
	/** @brief Mock class to test clone and equals for SGObject.
	 * Serves as a parameter of type SGObject for @see CCloneEqualsMock.
	 */
	template <class T>
	class CloneEqualsMockParameter : public SGObject
	{
	public:
		CloneEqualsMockParameter()
		{
			SG_SDEBUG("Creating new %s, at %p.\n", get_name(), this)
			m_was_cloned = false;
			m_some_value = 1;

			watch_param("some_value", &m_some_value);
			m_parameters->add(&m_some_value, "some_value", "Some value");
		}
		const char* get_name() const override
		{
			return "CloneEqualsMockParameter";
		}

		virtual std::shared_ptr<SGObject> create_empty() const override
		{
			return std::make_shared<CloneEqualsMockParameter>();
		}

		virtual std::shared_ptr<SGObject> clone() const override
		{
			auto clone = SGObject::clone();
			auto casted = clone->template as<CloneEqualsMockParameter>();
			casted->m_was_cloned = true;
			return clone;
		}

		bool m_was_cloned;
		T m_some_value;
	};

	/** @brief Mock class to test clone and equals for SGObject.
	 * Has members that cover (hopefully) all possibilities of parameters.
	 */
	template <class T>
	class CloneEqualsMock : public SGObject
	{
	public:
		CloneEqualsMock()
		{
			init_single();
			init_sg_vector_matrix();
			init_sg_sparse_vector_matrix();
			init_raw_vector();
			init_raw_matrix();
		}

		~CloneEqualsMock()
		{
			free_single();
			free_raw_vector();
			free_raw_matrix();
		}

		void init_single()
		{
			m_basic = 1;
			watch_param("basic", &m_basic);
			m_parameters->add(&m_basic, "basic", "The basic guy");

			m_object = std::make_shared<CCloneEqualsMockParameter<T>>();
			watch_param("object", &m_object);
			m_parameters->add(
			    (SGObject**)&m_object, "object", "The object (tm)");

			m_string = "Shogun rocks!";
			watch_param("string", &m_string);
		}

		void free_single()
		{
			m_object.reset();
		}

		void init_sg_vector_matrix()
		{
			m_sg_vector = SGVector<T>(2);
			m_sg_vector.set_const(m_basic);
			watch_param("sg_vector", &m_sg_vector);
			m_parameters->add(&m_sg_vector, "sg_vector", "The SGVector");

			m_sg_matrix = SGMatrix<T>(3, 4);
			m_sg_matrix.set_const(m_basic);
			watch_param("sg_matrix", &m_sg_matrix);
			m_parameters->add(&m_sg_matrix, "sg_matrix", "The SGMatrix");
		}

		void init_sg_sparse_vector_matrix()
		{
			m_sg_sparse_vector = SGSparseVector<T>(4);
			for (auto i : range(m_sg_sparse_vector.num_feat_entries))
			{
				SGSparseVectorEntry<T> entry;
				entry.feat_index = i * 2;
				entry.entry = 2;
				m_sg_sparse_vector.features[i] = entry;
			}
			watch_param("sg_sparse_vector", &m_sg_sparse_vector);
			m_parameters->add(
			    &m_sg_sparse_vector, "sg_sparse_vector", "The SGSparseVector");

			m_sg_sparse_matrix =
			    SGSparseMatrix<T>(m_sg_sparse_vector.num_feat_entries, 6);
			for (auto i : range(m_sg_sparse_matrix.num_vectors))
			{
				SGSparseVector<T> vec(m_sg_sparse_matrix.num_features);
				for (auto j : range(vec.num_feat_entries))
				{
					SGSparseVectorEntry<T> entry;
					entry.feat_index = i * 2;
					entry.entry = 3;
					vec.features[j] = entry;
				}
				m_sg_sparse_matrix.sparse_matrix[i] = vec;
			}
			watch_param("sg_sparse_matrix", &m_sg_sparse_matrix);
			m_parameters->add(
			    &m_sg_sparse_matrix, "sg_sparse_matrix", "The SGSparseMatrix");
		}

		void init_raw_vector()
		{
			m_raw_vector_basic_len = 5;
			m_raw_vector_basic = new T[m_raw_vector_basic_len];
			for (auto i : range(m_raw_vector_basic_len))
				m_raw_vector_basic[i] = m_basic;
			watch_param(
			    "raw_vector_basic", &m_raw_vector_basic,
			    &m_raw_vector_basic_len);
			m_parameters->add_vector(
			    &m_raw_vector_basic, &m_raw_vector_basic_len,
			    "raw_vector_basic", "The raw vector basic");

			m_raw_vector_sg_string_len = 7;
			m_raw_vector_sg_string =
			    new SGString<T>[m_raw_vector_sg_string_len];
			for (auto i : range(m_raw_vector_sg_string_len))
			{
				m_raw_vector_sg_string[i] = SGString<T>(i + 1, true);
				for (auto j : range(m_raw_vector_sg_string[i].slen))
					m_raw_vector_sg_string[i].string[j] = 1;
			}
			watch_param(
			    "raw_vector_sg_string", &m_raw_vector_sg_string,
			    &m_raw_vector_sg_string_len);
			m_parameters->add_vector(
			    &m_raw_vector_sg_string, &m_raw_vector_sg_string_len,
			    "raw_vector_sg_string", "The raw vector sg_string");

			m_raw_vector_object_len = 6;
			m_raw_vector_object =
			    new CCloneEqualsMockParameter<T>*[m_raw_vector_object_len];
			for (auto i : range(m_raw_vector_object_len))
				m_raw_vector_object[i] = std::make_shared<CCloneEqualsMockParameter<T>>();
			watch_param(
			    "raw_vector_object", &m_raw_vector_object,
			    &m_raw_vector_object_len);
			m_parameters->add_vector(
			    (SGObject***)&m_raw_vector_object, &m_raw_vector_object_len,
			    "raw_vector_object", "The raw vector object");
		}

		void free_raw_vector()
		{
			delete[] m_raw_vector_basic;

			for (auto i : range(m_raw_vector_object_len))
				m_raw_vector_object[i].reset();
			delete[] m_raw_vector_object;

			for (auto i : range(m_raw_vector_sg_string_len))
				m_raw_vector_sg_string[i].free_string();
			delete[] m_raw_vector_sg_string;
		}

		void init_raw_matrix()
		{
			m_raw_matrix_basic_rows = 2;
			m_raw_matrix_basic_cols = 3;
			auto size =
			    int64_t(m_raw_matrix_basic_rows) * m_raw_matrix_basic_cols;
			m_raw_matrix_basic = new T[size];
			for (auto i : range(size))
				m_raw_matrix_basic[i] = 1;
			watch_param(
			    "raw_matrix_basic", &m_raw_matrix_basic,
			    &m_raw_matrix_basic_rows, &m_raw_matrix_basic_cols);
			m_parameters->add_matrix(
			    &m_raw_matrix_basic, &m_raw_matrix_basic_rows,
			    &m_raw_matrix_basic_cols, "raw_matrix_basic", "The raw matrix");
		}

		void free_raw_matrix()
		{
			delete[] m_raw_matrix_basic;
		}

		const char* get_name() const override
		{
			return "CloneEqualsMock";
		}

		virtual std::shared_ptr<SGObject> create_empty() const override
		{
			return std::make_shared<CCloneEqualsMock>();
		}

		T m_basic;
		std::shared_ptr<CloneEqualsMockParameter<T>> m_object;

		SGVector<T> m_sg_vector;
		SGMatrix<T> m_sg_matrix;

		SGSparseVector<T> m_sg_sparse_vector;
		SGSparseMatrix<T> m_sg_sparse_matrix;

		T* m_raw_vector_basic;
		index_t m_raw_vector_basic_len;

		SGString<T>* m_raw_vector_sg_string;
		index_t m_raw_vector_sg_string_len;

		CloneEqualsMockParameter<T>** m_raw_vector_object;
		index_t m_raw_vector_object_len;

		T* m_raw_matrix_basic;
		index_t m_raw_matrix_basic_rows;
		index_t m_raw_matrix_basic_cols;

		std::string m_string;
	};

	/** @brief Used to test the tags-parameter framework
	 * Allows testing of registering new member and avoiding
	 * non-registered member variables using tags framework.
	 */
	class MockObject : public SGObject
	{
	public:
		MockObject() : SGObject()
		{
			init_params();
		}

		virtual ~MockObject()
		{
		}

		const char* get_name() const override
		{
			return "MockObject";
		}

		void set_watched(int32_t value)
		{
			m_watched = value;
		}

		int32_t get_watched() const
		{
			return m_watched;
		}

		auto get_object() const
		{
			return m_object;
		}

		int some_method() const
		{
			return 42;
		}

	protected:
		void init_params()
		{
			float64_t decimal = 0.0;
			register_param("vector", SGVector<float64_t>());
			register_param("int", m_integer);
			register_param("float", decimal);

			watch_param(
			    "watched_int", &m_watched,
			    AnyParameterProperties(
			        "Integer"));

			watch_param(
			    "watched_object", &m_object,
			    AnyParameterProperties(
			        "Object"));

			watch_method("some_method", &CMockObject::some_method);
		}

		virtual std::shared_ptr<SGObject> create_empty() const
		{
			return std::make_shared<MockObject>();
		};

	private:
		int32_t m_integer = 0;
		int32_t m_watched = 0;

		std::shared_ptr<MockObject> m_object = nullptr;
	};
}

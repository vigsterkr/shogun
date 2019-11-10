// all the code that depends on visitors
#if defined(SWIGPYTHON) || defined(SWIGR)
%include "Visitors_implementation.i"
%define VISITOR_GETTER(INTERFACE_BASETYPE, VISITOR_NAME)
INTERFACE_BASETYPE get(const std::string& name) const
{
	INTERFACE_BASETYPE result = nullptr;
	try
	{
		const auto params = $self->get_params();
		auto find_iter = params.find(name);
		auto enum_map = $self->get_string_to_enum_map();

		if (enum_map.count(name))
		{
			auto enum_as_string = $self->get<std::string>(name);
			result = SWIG_From_std_string(enum_as_string);
			return result;
		}

		if (find_iter == params.end())
		{
			error("Could not find parameter {}::{}.", $self->get_name(), name.c_str());
			return nullptr;
		}

		auto visitor = VISITOR_NAME(result);
		find_iter->second->get_value().visit(&visitor);
		if (!result)
		{
			error("Could not cast parameter {}::{}!", $self->get_name(), name.c_str());
			return nullptr;
		}
	}
	catch(ShogunException& e)
	{
		SWIG_Error(SWIG_SystemError, const_cast<char*>(e.what()));
		return nullptr;
	}
	return result;
}
INTERFACE_BASETYPE get(const std::string& name, int index) const
{
	INTERFACE_BASETYPE result = nullptr;
	try
	{
		std::shared_ptr<SGObject> obj = $self->get(name, index);
		std::shared_ptr<shogun::SGObject> *smartresult = obj ? new std::shared_ptr<shogun::SGObject>(obj) : 0;
#if defined(SWIGPYTHON)
		result = SWIG_Python_NewPointerObj(nullptr, SWIG_as_voidptr(smartresult), SWIGTYPE_p_std__shared_ptrT_shogun__SGObject_t, SWIG_POINTER_OWN);
#elif defined(SWIGR)
		result = SWIG_R_NewPointerObj(SWIG_as_voidptr(smartresult), SWIGTYPE_p_std__shared_ptrT_shogun__SGObject_t, SWIG_POINTER_OWN);
#endif
	}
	catch(ShogunException& e)
	{
		SWIG_Error(SWIG_SystemError, const_cast<char*>(e.what()));
		return nullptr;
	}
	return result;
}
%enddef

#endif // SWIGPYTHON || SWIGR


namespace shogun
{
	%extend SGObject
	{
#ifdef SWIGPYTHON
		VISITOR_GETTER(PyObject*, PythonVisitor)
#elif SWIGR
		VISITOR_GETTER(SEXP, RVisitor)
#endif

		std::vector<std::string> parameter_names() const {
			std::vector<std::string> result;
			for (auto const& each: $self->get_params()) {
				result.push_back(std::string(each.first));
			}
			return result;
		}

		std::string parameter_type(const std::string& name) const {
			auto params = $self->get_params();
			if (params.find(name) != params.end()) {
				return params[name].get()->get_value().type();
			}
			else {
				error("There is no parameter called '{}' in {}", name.c_str(), $self->get_name());
			}
		}

		bool parameter_is_sg_base(const std::string& name) const {
			auto params = $self->get_params();
			if (params.find(name) != params.end()) {
				if ($self->get(name, std::nothrow) != nullptr)
					return true;
				else
					return false;
			}
			else
			{
				error("There is no parameter called '{}' in {}", name.c_str(), $self->get_name());
			}
		}

		std::vector<std::string> param_options(const std::string& name) const {
			std::vector<std::string> result;

			auto param_to_enum_map = $self->get_string_to_enum_map();

			if (param_to_enum_map.find(name) == param_to_enum_map.end())
			{
				error("There are no options for parameter {}::{}", $self->get_name(), name.c_str());
			}

			auto string_to_enum_map = param_to_enum_map[name];

			for (auto const& each: string_to_enum_map)
				result.push_back(std::string(each.first));

			return result;
		}


		/** Untyped getter for an object class parameter, identified by a name.
		 * Will attempt to get specified object of appropriate internal type.
		 * If this is not possible it will raise a ShogunException.
		 *
		 * @param name name of the parameter
		 * @return object parameter
		 */
		std::shared_ptr<SGObject> get(std::string_view name) const noexcept(false)
		{
			if (!has(name))
			{
				error("Parameter {}::{} does not exist.", get_name(), name.data());
			}
			if (auto result = get(name, std::nothrow))
			{
				return result;
			}
			error(
					"Cannot get parameter {}::{} of type {} as object.",
					get_name(), name.data(),
					self->map[BaseTag(name.data())].get_value().type().c_str());
			return nullptr;
		}

		/** Untyped getter for an object class parameter, identified by a name.
		 * Does not throw an error if class parameter object cannot be casted
		 * to appropriate internal type.
		 *
		 * @param name name of the parameter
		 * @return object parameter
		 */
		std::shared_ptr<SGObject> get(std::string_view name, std::nothrow_t) const noexcept
		{
			return sgo_details::get_by_tag(shared_from_this(), name, sgo_details::GetByName());
		}

		/** Untyped getter for an object array class parameter, identified by a name
		 * and an index.
		 * Will attempt to get specified object of appropriate internal type.
		 * If this is not possible it will raise a ShogunException.
		 *
		 * @param name name of the parameter
		 * @index index of the parameter
		 * @return object parameter
		 */
		std::shared_ptr<SGObject> get(std::string_view name, index_t index) const
		{
			auto result = sgo_details::get_by_tag(shared_from_this(), name, sgo_details::GetByNameIndex(index));
			if (!result && has(name))
			{
				error(
					"Cannot get array parameter {}::{}[{}] of type {} as object.",
					get_name(), name.data(), index,
					self->map[BaseTag(name)].get_value().type().c_str());
			}
			return result;
		}

#ifdef SWIGPYTHON
		std::string __str__() const
		{
			return $self->to_string();
		}

		std::string __repr__() const
		{
			return $self->to_string();
		}

		PyObject* __reduce_ex__(int proto)
		{
			pickle_ascii = (proto==0) ? 1 : 0;
			Py_RETURN_NONE;
		}

		PyObject* __getstate__()
		{
			std::shared_ptr<io::Serializer> serializer = nullptr;
			if (pickle_ascii)
				serializer = std::make_shared<io::JsonSerializer>();
			else
				serializer = std::make_shared<io::BitserySerializer>();
			auto byte_stream = std::make_shared<io::ByteArrayOutputStream>();
			serializer->attach(byte_stream);
			serializer->write(std::shared_ptr<SGObject>($self));
			auto serialized_obj = byte_stream->content();
			PyObject* str=PyBytes_FromStringAndSize(serialized_obj.data(), serialized_obj.size());
			PyObject* tuple=PyTuple_New(2);
			PyTuple_SetItem(tuple, 0, PyBool_FromLong(pickle_ascii));
			PyTuple_SetItem(tuple, 1, str);
			return tuple;
		}

		void __setstate__(PyObject* state)
		{
			PyObject* py_ascii = PyTuple_GetItem(state,0);
			pickle_ascii= (py_ascii == Py_True) ? 1 : 0;
			PyObject* py_str = PyTuple_GetItem(state,1);
			char* str=NULL;
			Py_ssize_t len=0;

			PyBytes_AsStringAndSize(py_str, &str, &len);
			std::shared_ptr<io::Deserializer> deser = nullptr;
			if (pickle_ascii)
				deser = std::make_shared<io::JsonDeserializer>();
			else
				deser = std::make_shared<io::BitseryDeserializer>();

			auto byte_input_stream = std::make_shared<io::ByteArrayInputStream>(str, len);
			deser->attach(byte_input_stream);
			$self->deserialize(deser);
		}

		/*int getbuffer(PyObject *obj, Py_buffer *view, int flags) { return 0; }*/
#endif //SWIGPYTHON
	}
}

#ifdef SWIGPYTHON
%pythoncode %{
try:
	import copy_reg
except ImportError:
	import copyreg as copy_reg
def _sg_reconstructor(cls, base, state):
	try:
		if isinstance(cls, str) and cls.startswith('shogun.'):
			if base is object:
				import shogun
				return eval(cls+'()')
			else:
				base.__new__(cls, state)
				if base.__init__ != object.__init__:
					base.__init__(obj, state)
			return obj
		if isinstance(cls(), SGObject):
			if base is object:
				 obj = cls()
			else:
				obj = base.__new__(cls, state)
				if base.__init__ != object.__init__:
					base.__init__(obj, state)
			return obj

		return _py_orig_reconstructor(cls, base, state)
	except:
		return _py_orig_reconstructor(cls, base, state)

def _sg_reduce_ex(self, proto):
	try:
		if not isinstance(self, SGObject):
			return _py_orig_reduce_ex(self, proto)
	except:
		return _py_orig_reduce_ex(self, proto)

	base = object
	state = None
	args = ('shogun.' + self.get_name(), base, state)


	try:
		getstate = self.__getstate__
	except AttributeError:
		if getattr(self, "__slots__", None):
			raise TypeError("a class that defines __slots__ without "
							"defining __getstate__ cannot be pickled")
		try:
			dict = self.__dict__
		except AttributeError:
			dict = None
	else:
		dict = getstate()
	if dict:
		return _sg_reconstructor, args, dict
	else:
		return _sg_reconstructor, args

_py_orig_reduce_ex=copy_reg._reduce_ex
_py_orig_reconstructor=copy_reg._reconstructor

copy_reg._reduce_ex=_sg_reduce_ex
copy_reg._reconstructor=_sg_reconstructor
%}

#endif /* SWIGPYTHON  */

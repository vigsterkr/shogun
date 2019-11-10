/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Gil Hoben, Heiko Strathmann, Sergey Lisitsyn
 */

/* base includes required by any module */
%include <stdint.i>
%include <std_string.i>
%include <exception.i>

#ifdef SWIGJAVA
%typemap(javainterfaces) shogun::SGObject "java.io.Externalizable"

%typemap(javaimports) shogun::SGObject
%{
import org.shogun.JsonSerializer;
import org.shogun.JsonDeserializer;
import org.shogun.ByteArrayOutputStream;
import org.shogun.ByteArrayInputStream;
import java.lang.StringBuffer;
import org.jblas.*;
%}
%typemap(javacode) shogun::SGObject
%{
public void writeExternal(java.io.ObjectOutput out) throws java.io.IOException {
    ByteArrayOutputStream byteArrayOS = new ByteArrayOutputStream();
    JsonSerializer jsonSerializer = new JsonSerializer();
    jsonSerializer.attach(byteArrayOS);
    jsonSerializer.write(this);

    String obj_serialized = byteArrayOS.as_string();
    out.write(obj_serialized.getBytes());
}

public void readExternal(java.io.ObjectInput in) throws java.io.IOException, java.lang.ClassNotFoundException {
    StringBuffer sb = new StringBuffer();
    int ch;
    while ((ch=in.read()) != -1) {
        sb.append((char)ch);
    }
    ByteArrayInputStream bis = new ByteArrayInputStream(sb.toString());
    JsonDeserializer jsonDeserializer = new JsonDeserializer();
    jsonDeserializer.attach(bis);
    this.deserialize(jsonDeserializer);
}
    %}
#endif

%{
#ifdef SWIGRUBY
 extern "C" {
  #include <ruby.h>
  #include <narray.h>
  #include <stdlib.h>
  #include <stdio.h>
 }
 VALUE (*na_to_array_dl)(VALUE);
 VALUE (*na_to_narray_dl)(VALUE);
 VALUE cNArray;
 #include <dlfcn.h>
#endif
#if defined(SWIGPERL) && defined(HAVE_PDL)
#ifdef __cplusplus
  extern "C" {
#endif
#include <pdlcore.h>

#include <ppport.h>

#ifdef __cplusplus
  }
#endif
#endif
 /* required for python */
 #define SWIG_FILE_WITH_INIT

 #include <shogun/base/ShogunEnv.h>
 #include <shogun/lib/common.h>
 #include <shogun/io/SGIO.h>
 #include <shogun/lib/exception/ShogunException.h>
 #include <shogun/lib/DataType.h>
 #include <shogun/base/Version.h>
 #include <shogun/base/Parallel.h>
 #include <shogun/base/SGObject.h>
 #include <shogun/lib/StoppableSGObject.h>

 extern void sg_global_print_message(FILE* target, const char* str);
 extern void sg_global_print_warning(FILE* target, const char* str);
 extern void sg_global_print_error(FILE* target, const char* str);

#ifdef SWIGR
 #include <Rdefines.h>
#endif

#ifdef SWIGPYTHON

 #include <shogun/io/serialization/BitserySerializer.h>
 #include <shogun/io/serialization/BitseryDeserializer.h>
 #include <shogun/io/serialization/JsonSerializer.h>
 #include <shogun/io/serialization/JsonDeserializer.h>
 #include <shogun/io/stream/ByteArrayInputStream.h>
 #include <shogun/io/stream/ByteArrayOutputStream.h>

 static int pickle_ascii;
#endif

 using namespace shogun;

%}

#if  defined (SWIGPERL) && defined(HAVE_PDL)
%header %{
  SV* CoreSV;
  Core* PDL;
%}
#endif

%init %{

#if  defined (SWIGPERL) && defined(HAVE_PDL)
  //check Core.xs //Core* PDL_p = pdl__Core_get_Core();
  //PDL_COMMENT("Get pointer to structure of core shared C routines")
  //PDL_COMMENT("make sure PDL::Core is loaded")

  perl_require_pv("PDL::Core");
  CoreSV = perl_get_sv("PDL::SHARE",FALSE);
  //  PDL_COMMENT("SV* value")
  if (CoreSV == NULL)
    Perl_croak(aTHX_ "Can't load PDL::Core module");
  PDL = INT2PTR(Core*, SvIV( CoreSV ));
  //  PDL_COMMENT("Core* value")
#endif


#ifdef SWIGPYTHON
        import_array();
#endif

#ifdef SWIGRUBY
        rb_require("narray");
        cNArray = rb_const_get(rb_cObject, rb_intern("NArray"));

        char* error=NULL;

        void* handle = dlopen(NARRAY_LIB, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "%s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        dlerror();    /* Clear any existing error */

        *(void **) (&na_to_array_dl) = dlsym(handle, "na_to_array");
        if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "na_to_array %s\n", error);
                exit(EXIT_FAILURE);
        }

        /*if (cNArray==0)
        {
            void (*Init_narray)();
            *(void **) (&Init_narray) = dlsym(handle, "Init_narray");
            if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "Init_narray %s\n", error);
                exit(EXIT_FAILURE);
            }

            fprintf(stderr, "initing narray\n");
            (*Init_narray)();
        }*/

        *(void **) (&na_to_narray_dl) = dlsym(handle, "na_to_narray");
        if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "na_to_narray %s\n", error);
                exit(EXIT_FAILURE);
        }

/*        cNArray = (*(VALUE*)(dlsym(handle, "cNArray")));
        if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "cNArray %s\n", error);
                exit(EXIT_FAILURE);
        }*/
#endif

%}

#ifdef SWIGPYTHON
%{
        static int print_sgobject(PyObject *pyobj, FILE *f, int flags) {
            void *argp;
            int res;
            res = SWIG_ConvertPtr(pyobj, &argp, SWIGTYPE_p_std__shared_ptrT_shogun__SGObject_t, 0);
            if (!SWIG_IsOK(res)) {
                SWIG_Error(SWIG_ArgError(res), "in method 'SGObject::tp_print', argument 1 of type 'std::shared_ptr<SGObject>'");
                return SWIG_ERROR;
            }

            auto obj = reinterpret_cast<std::shared_ptr<SGObject>*>(argp);
            std::string s = (*obj)->to_string();
            fprintf(f, "%s", s.c_str());
            return 0;
        }
%}

%feature("nothread") _swig_monkey_patch;
%feature("docstring", "Adds a Python object (such as a function) \n"
					  "to a class (method) or to a module. \n"
					  "If the name of the function conflicts with \n"
	   				  "another Python object in the same scope\n"
                      "raises a TypeError.") _swig_monkey_patch;

// taken from https://github.com/swig/swig/issues/723#issuecomment-230178855
%typemap(out) void _swig_monkey_patch "$result = PyErr_Occurred() ? NULL : SWIG_Py_Void();"
%inline %{
	static void _swig_monkey_patch(PyObject *type, PyObject *name, PyObject *object) {
		PyObject *dict = NULL;
		if (!PyUnicode_Check(name))
			{
				PyErr_SetString(PyExc_TypeError, "name is not a string");
				return;
			}

		if (PyType_Check(type)) {
			PyTypeObject *pytype = (PyTypeObject *)type;
			dict = pytype->tp_dict;
		}
		else if (PyModule_Check(type)) {
			dict = PyModule_GetDict(type);
		}
		else {
			PyErr_SetString(PyExc_TypeError, "type is not a Python type or module");
			return;
		}
		if (PyDict_Contains(dict, name))
		{
			PyErr_SetString(PyExc_ValueError, "function name already exists in the given scope");
			return;
		}
		PyDict_SetItem(dict, name, object);

	  }
%}

%typemap(out) PyObject* __reduce_ex__(int proto)
{
    return PyObject_CallMethod(self, (char*) "__reduce__", (char*) "");
}

%typemap(in) __setstate__(PyObject* state) {
    $1 = $input;
}

%typemap(out) PyObject* __getstate__()
{
    $result=$1;
}

#elif defined(SWIGPERL)
%{
  static int print_sgobject(SV* pobj, FILE *f, int flags) {
    void *argp;
    int res;
    res = SWIG_ConvertPtr(pobj, &argp, SWIGTYPE_p_shogun__SGObject, 0);
    if (!SWIG_IsOK(res)) {
      SWIG_Error(SWIG_ArgError(res), "in method 'SGObject::tp_print', argument 1 of type 'SGObject *'");
      return SWIG_ERROR;
    }
    SGObject *obj = reinterpret_cast<SGObject*>(argp);
    std::string s = obj->to_string();
    fprintf(f, "%s", s.c_str());
    return 0;
  }
%}
#endif

#ifdef SWIGCSHARP
%typemap(cscode) shogun::SGObject %{
    // enable "putting" enums without casting
    public void put(string name, Enum value) {
        put(name, Convert.ToInt32(value));
    }
%}
#endif // SWIGCSHARP

%exception
{
    try
    {
        $action
    }
#if defined(SWIGPYTHON) && defined(USE_SWIG_DIRECTORS)
    catch (Swig::DirectorException &e)
    {
        SWIG_fail;
    }
#endif
    catch (std::bad_alloc&)
    {
        SWIG_exception(SWIG_MemoryError, const_cast<char*>("Out of memory error.\n"));
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
        SWIG_fail;
#endif
    }
    catch (shogun::ShogunException& e)
    {
        SWIG_exception(SWIG_SystemError, const_cast<char*>(e.what()));
#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
        SWIG_fail;
#endif
    }
    SWIG_CATCH_STDEXCEPT
}

%ignore NUM_LOG_LEVELS;
%ignore FBUFSIZE;
%ignore sg_print_message;
%ignore sg_print_warning;
%ignore sg_print_error;
%ignore sg_cancel_computations;

%shared_ptr(shogun::SGObject)
%shared_ptr(shogun::Seedable<shogun::SGObject>)
%shared_ptr(shogun::RandomMixin<shogun::SGObject, std::mt19937_64>)
%shared_ptr(shogun::StoppableSGObject)

%include <shogun/lib/common.h>
%include <shogun/lib/exception/ShogunException.h>

%include "typemaps_utilities.i"
%include "swig_typemaps.i"

%include <std_vector.i>
namespace std {
    %template(IntStdVector) vector<int32_t>;
    %template(DoubleStdVector) vector<float64_t>;
    %template(StringStdVector) vector<string>;
}

%include <shogun/base/SGObject.h>
RANDOM_INTERFACE(SGObject)

%include <shogun/io/SGIO.h>
%include <shogun/base/Version.h>
%include <shogun/base/Parallel.h>
%include <shogun/lib/StoppableSGObject.h>

%include "SGObject_extensions.i"

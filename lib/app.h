/*
 * Copyright (c) 2008-2016 the MRtrix3 contributors
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 * 
 * MRtrix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * For more details, see www.mrtrix.org
 * 
 */

#ifndef __app_h__
#define __app_h__

#include <string.h>
#include <string>
#include <vector>
#include <limits>

#ifdef None
# undef None
#endif

#include "cmdline_option.h"
#include "file/path.h"


extern void usage ();
extern void run ();

namespace MR
{
  namespace App
  {


    extern const char* mrtrix_version;
    extern int log_level;
    extern std::string NAME;
    extern bool overwrite_files;
    extern void (*check_overwrite_files_func) (const std::string& name);
    extern bool fail_on_warn;
    extern bool terminal_use_colour;

    extern int argc;
    extern char** argv;
    extern bool stderr_to_file;

    extern const char* project_version;
    extern const char* build_date;


    const char* argtype_description (ArgType type);

    std::string help_head (int format);
    std::string help_tail (int format);
    std::string help_syntax (int format);
    




    //! \addtogroup CmdParse
    // @{

    //! vector of strings to hold the command-line description
    class Description : public std::vector<const char*>
    {
      public:
        Description& operator+ (const char* text) {
          push_back (text);
          return *this;
        }

        std::string syntax (int format) const;
    };




    //! a class to hold the list of Argument's
    class ArgumentList : public std::vector<Argument> {
      public:
        ArgumentList& operator+ (const Argument& argument) {
          push_back (argument);
          return *this;
        }

        std::string syntax (int format) const;
    };





    //! a class to hold the list of option groups
    class OptionList : public std::vector<OptionGroup> {
      public:
        OptionList& operator+ (const OptionGroup& option_group) {
          push_back (option_group);
          return *this;
        }

        OptionList& operator+ (const Option& option) {
          back() + option;
          return *this;
        }

        OptionList& operator+ (const Argument& argument) {
          back() + argument;
          return *this;
        }

        OptionGroup& back () {
          if (empty())
            push_back (OptionGroup());
          return std::vector<OptionGroup>::back();
        }

        std::string syntax (int format) const;
    };




    inline void check_overwrite (const std::string& name) 
    {
      if (Path::exists (name) && !overwrite_files) {
        if (check_overwrite_files_func)
          check_overwrite_files_func (name);
        else 
          throw Exception ("output file \"" + name + "\" already exists (use -force option to force overwrite)");
      }
    }




    //! initialise MRtrix and parse command-line arguments
    /*! this function must be called from within main(), immediately after the
     * argument and options have been specified, and before any further
     * processing takes place. */
    void init (int argc, char** argv);

    //! do the actual parsing of the command-line [used internally]
    void parse ();

    //! sort command-line tokens into arguments and options [used internally]
    void sort_arguments (int argc, const char* const* argv);

    //! uniquely match option stub to Option
    const Option* match_option (const char* stub);

    //! dump formatted help page [used internally]
    std::string full_usage ();





    class ParsedArgument
    {
      public:
        operator std::string () const { return p; }

        const char* as_text () const { return p; }
        bool as_bool () const { return to<bool> (p); }
        int64_t as_int () const;
        uint64_t as_uint () const { return uint64_t (as_int()); }
        default_type as_float () const;

        std::vector<int> as_sequence_int () const {
          assert (arg->type == IntSeq);
          try { return parse_ints (p); }
          catch (Exception& e) { error (e); }
          return std::vector<int>();
        }

        std::vector<default_type> as_sequence_float () const {
          assert (arg->type == FloatSeq);
          try { return parse_floats (p); }
          catch (Exception& e) { error (e); }
          return std::vector<default_type>();
        }

        operator bool () const { return as_bool(); }
        operator int () const { return as_int(); }
        operator unsigned int () const { return as_uint(); }
        operator long int () const { return as_int(); }
        operator long unsigned int () const { return as_uint(); }
        operator long long int () const { return as_int(); }
        operator long long unsigned int () const { return as_uint(); }
        operator float () const { return as_float(); }
        operator double () const { return as_float(); }
        operator std::vector<int> () const { return as_sequence_int(); }
        operator std::vector<default_type> () const { return as_sequence_float(); }

        const char* c_str () const { return p; }

      private:
        const Option* opt;
        const Argument* arg;
        const char* p;

        ParsedArgument (const Option* option, const Argument* argument, const char* text) :
          opt (option), arg (argument), p (text) {
          assert (text);
        }

        void error (Exception& e) const {
          std::string msg ("error parsing token \"");
          msg += p;
          if (opt) msg += std::string ("\" for option \"") + opt->id + "\"";
          else msg += std::string ("\" for argument \"") + arg->id + "\"";
          throw Exception (e, msg);
        }

        friend class ParsedOption;
        friend class Options;
        friend void  MR::App::init (int argc, char** argv);
        friend void  MR::App::parse ();
        friend void  MR::App::sort_arguments (int argc, const char* const* argv);
    };




    //! object storing information about option parsed from command-line
    /*! this is the object stored in the App::options vector, and the type
     * returned by App::get_options(). */
    class ParsedOption
    {
      public:
        ParsedOption (const Option* option, const char* const* arguments) :
          opt (option), args (arguments) { }

        //! reference to the corresponding Option entry in the OPTIONS section
        const Option* opt;
        //! pointer into \c argv corresponding to the option's first argument
        const char* const* args;

        const ParsedArgument operator[] (size_t num) const {
          assert (num < opt->size());
          return ParsedArgument (opt, & (*opt) [num], args[num]);
        }

        //! check whether this option matches the name supplied
        bool operator== (const char* match) const {
          std::string name = lowercase (match);
          return name == opt->id;
        }

    };


    //! the list of arguments parsed from the command-line
    extern std::vector<ParsedArgument> argument;
    //! the list of options parsed from the command-line
    extern std::vector<ParsedOption> option;

    //! the description of the command
    /*! This is designed to be used within each command's usage() function. Add
     * a paragraph to the description using the '+' operator, e.g.:
     * \code
     * void usage() {
     *   DESCRIPTION 
     *   + "A command to do stuff. It can be used in lots of ways "
     *     "and very versatile."
     *
     *   + "More description in this paragraph. It has lots of options "
     *     "and arguments.";
     * }
     * \endcode
     */
    extern Description DESCRIPTION;


    //! the arguments expected by the command
    /*! This is designed to be used within each command's usage() function. Add
     * argument and their description using the Argument class and the'+'
     * operator, e.g.:
     * \code
     * void usage() {
     *   ...
     *
     *   ARGUMENTS
     *   + Argument ("in", "the input image").type_image_in()
     *   + Argument ("factor", "the factor to use in the analysis").type_float()
     *   + Argument ("out", "the output image").type_image_out(); 
     * }
     * \endcode
     */
    extern ArgumentList ARGUMENTS;

    //! the options accepted by the command
    /*! This is designed to be used within each command's usage() function. Add
     * options, their arguments, and their description using the Option and
     * Argument classes and the'+' operator, e.g.:
     * \code
     * void usage() {
     *   ...
     *
     *   OPTIONS
     *   + Option ("advanced", "use advanced analysis")
     *
     *   + Option ("range", "the range to use in the analysis")
     *   +   Argument ("min").type_float()
     *   +   Argument ("max").type_float();
     * }
     * \endcode
     */
    extern OptionList OPTIONS;

    //! set to false if command can operate with no arguments
    /*! By default, the help page is shown command is invoked without
     * arguments. Some commands (e.g. MRView) can operate without arguments. */
    extern bool REQUIRES_AT_LEAST_ONE_ARGUMENT;

    //! set the author of the command
    /*! By default, this is set to "J-Donald Tournier (jdtournier@gmail.com)".
     * Set to your own name & email to override. */
    extern const char* AUTHOR;

    //! set the copyright notice if different from that used in MRtrix
    extern const char* COPYRIGHT;

    //! add references to command help page
    /*! Like the description, use the '+' operator to add paragraphs (typically
     * one citation per paragraph)." */
    extern Description REFERENCES;


    //! the group of standard options for all commands
    extern OptionGroup __standard_options;



    //! return all command-line options matching \c name
    /*! This returns a vector of vectors, where each top-level entry
     * corresponds to a distinct instance of the option, and each entry within
     * a top-level entry corresponds to a argument supplied to that option.
     *
     * Individual options can be retrieved easily using the as_* methods, or
     * implicit type-casting.  Any relevant range checks are performed at this
     * point, based on the original App::Option specification. For example:
     * \code
     * Options opt = get_options ("myopt");
     * if (opt.size()) {
     *    std::string arg1 = opt[0][0];
     *    int arg2 = opt[0][1];
     *    float arg3 = opt[0][2];
     *    std::vector<int> arg4 = opt[0][3];
     *    auto values = opt[0][4].as_sequence_float();
     * }
     * \endcode */
    const std::vector<ParsedOption> get_options (const std::string& name);
    
    
    //! Returns the option value if set, and the default otherwise.
    /*! Returns the value of (the first occurence of) option \c name
     *  or the default value provided as second argument.
     * 
     * Use:
     * \code
     *  float arg1 = get_option_value("myopt", arg1_default);
     *  int arg2 = get_option_value("myotheropt", arg2_default);
     * \endcode
     */
    template <typename T>
    inline T get_option_value (const std::string& name, const T default_value)
    {
      auto opt = get_options(name);
      T r = (opt.size()) ? opt[0][0] : default_value;
      return r;
    }
    

    //! convenience function provided mostly to ease writing Exception strings
    inline std::string operator+ (const char* left, const App::ParsedArgument& right)
    {
      std::string retval (left);
      retval += std::string (right);
      return retval;
    }


    inline std::ostream& operator<< (std::ostream& stream, const App::ParsedArgument& arg)
    {
      stream << std::string (arg);
      return stream;
    }

  }

  //! @}
}

#endif
